#include "Core.h"
#include "MessageBus.h"
#include "OSFramework.h"
#include "OSMessages.h"
#include <string>
#include "System.h"
#include "TickMessage.h"
#include <chrono>
#include "RenderMessage.h"
#include <fstream>

namespace u92 {
	namespace core {
		Core::Core ( ) {
			m_pMessageBus  = nullptr;
			m_pOSFramework = nullptr;
			m_running = false;
		}
		
		Core::~Core ( ) {
			release ( );
		}

		ErrorCode Core::init ( ){
			m_pMessageBus = new MessageBus;
			m_pMessageBus->registerClient (*this);

			m_pOSFramework = OSFramework::getInstance ( );
			m_pMessageBus->registerClient (*m_pOSFramework);

			LoadSystems ( );

			return E_CODE_SUCCESS;
		}

		void Core::release ( ) {
			if (m_pMessageBus) {
				delete m_pMessageBus;
				m_pMessageBus = nullptr;
			}
		}

		double time_in_seconds ( ) {
			return (double)std::chrono::high_resolution_clock::period::num/std::chrono::high_resolution_clock::period::den;
		}

		void Core::run ( ) {
			using namespace std::chrono;
			using namespace std::chrono_literals;
	
			m_running = true;

			unsigned long long int tick		= 0;
			unsigned long long int frame	= 0;

			duration<double> t = 0.0ms;
			const duration<double> dt = 10ms;

			high_resolution_clock::time_point currentTime = high_resolution_clock::now ( );
			duration<double> acc = 0.0ms;

			while (m_running) {
				high_resolution_clock::time_point newTime = high_resolution_clock::now ( );
				duration<double> frametime = newTime-currentTime;
				currentTime = newTime;

				acc += frametime;
				while (acc>=dt) {
					postMessage<TickMessage> ({ tick , dt},0);

					int result = m_pOSFramework->handleOSMessages ( );
					if (result==E_CODE_QUIT_MESSAGE) {
						m_running = false;
						break;
					}

					m_pMessageBus->syncMessages ( );
					m_pMessageBus->proccessMessages ( );

					tick++;
					acc -= dt;
					t += dt;
				}
				
				postMessage<RenderMessage> ({ frame },0);

				m_pMessageBus->syncMessages ( );
				m_pMessageBus->proccessMessages ( );

				frame++;
			}
		}

		void Core::LoadSystems ( ) {
			std::ifstream f ("..\\Data\\systems.txt");
			std::string mod = "";
			while (!f.eof ( )) {
				f>>mod;
				initSystem (mod.c_str ( ));
			}
		}

		void Core::initSystem (const char * systemModuleName) {
			System* pSys = m_pOSFramework->loadSystemModule (systemModuleName);
			pSys->init (m_pOSFramework);
			m_pMessageBus->registerClient (*pSys);
		}


	}
}