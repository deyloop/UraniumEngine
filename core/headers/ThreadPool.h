#pragma once

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>

namespace u92 {
	namespace core {
		class ThreadPool {
		public:
			ThreadPool (size_t);
			template<class F,class... Args>
			auto enqueue (F&& f,Args&&... args)
				->std::future<typename std::result_of<F (Args...)>::type>;
			template<class F,class... Args>
				auto enqueue_theadSpecific (int threadIndex,F&& f,Args&&... args)
				->std::future<typename std::result_of<F (Args...)>::type>;
			~ThreadPool ( );
		private:
			// need to keep track of threads so we can join them
			std::vector< std::thread > workers;
			std::vector< std::queue<std::function<void ( )>>> threadSpecificQueues;
			std::vector<std::unique_ptr< std::mutex>> threadSpecificQueueMutexes;
			// the task queue
			std::queue< std::function<void ( )> > tasks;

			// synchronization
			std::mutex queue_mutex;
			std::condition_variable condition;
			bool stop;
		};

		// the constructor just launches some amount of workers
		inline ThreadPool::ThreadPool (size_t threads)
			: stop (false) {
			threadSpecificQueues.resize (threads);
			threadSpecificQueueMutexes.resize (threads);

			for (size_t i = 0; i<threads; ++i) {
				threadSpecificQueueMutexes[i] = std::make_unique<std::mutex> ( );
				workers.emplace_back (
					[this,i] {
					for (;;) {
						std::function<void ( )> task;

						// first we check the thread specific queue
						bool cont = false;
						{
							std::lock_guard<std::mutex> lock (*(this->threadSpecificQueueMutexes[i]));
							if (!this->stop&&!this->threadSpecificQueues[i].empty ( )) {
								task = std::move (this->threadSpecificQueues[i].front ( ));
								this->threadSpecificQueues[i].pop ( );
								cont = true;
							}
						}

						if (cont) {
							task ( );
							continue;
						}

						{
							std::unique_lock<std::mutex> lock (this->queue_mutex);
							this->condition.wait (lock,
												  [this,i] { return this->stop||!this->tasks.empty ( ) || !this->threadSpecificQueues[i].empty(); });
						if (this->stop && this->tasks.empty ( ))
								return;
							if (!this->threadSpecificQueues[i].empty ( )) continue;
							task = std::move (this->tasks.front ( ));
							this->tasks.pop ( );
						}

						task ( );
					}
				});
			}
		}

		// add new work item to the pool
		template<class F,class... Args>
		auto ThreadPool::enqueue (F&& f,Args&&... args)
			-> std::future<typename std::result_of<F (Args...)>::type> {
			using return_type = typename std::result_of<F (Args...)>::type;

			auto task = std::make_shared< std::packaged_task<return_type ( )> > (
				std::bind (std::forward<F> (f),std::forward<Args> (args)...)
				);

			std::future<return_type> res = task->get_future ( );
			{
				std::unique_lock<std::mutex> lock (queue_mutex);

				// don't allow enqueueing after stopping the pool
				if (stop)
					throw std::runtime_error ("enqueue on stopped ThreadPool");

				tasks.emplace ([task]( ) { (*task)(); });
			}
			condition.notify_one ( );
			return res;
		}

		template<class F,class ...Args>
		inline auto ThreadPool::enqueue_theadSpecific (int threadIndex,F && f,Args && ...args) 
			-> std::future<typename std::result_of<F (Args ...)>::type> {
			using return_type = typename std::result_of<F (Args...)>::type;

			auto task = std::make_shared< std::packaged_task<return_type ( )> > (
				std::bind (std::forward<F> (f),std::forward<Args> (args)...)
				);

			std::future<return_type> res = task->get_future ( );
			{
				std::lock_guard<std::mutex> lock (*(threadSpecificQueueMutexes[threadIndex]));
				//don't allow enqueing after stopping the pool
				if (stop)
					throw std::runtime_error ("enqueue on stopped ThreadPool");

				threadSpecificQueues[threadIndex].emplace ([task]( ) {(*task)(); });
			}
			condition.notify_all ( );
			return res;
		}

		// the destructor joins all threads
		inline ThreadPool::~ThreadPool ( ) {
			{
				std::unique_lock<std::mutex> lock (queue_mutex);
				stop = true;
			}
			condition.notify_all ( );
			for (std::thread &worker:workers)
				worker.join ( );
		}
	}
}
#endif

