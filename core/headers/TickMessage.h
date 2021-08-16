#pragma once
#include <chrono>

struct TickMessage {
	unsigned long long int tick_number;
	std::chrono::duration<double> dt;
};