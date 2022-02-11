#include <cstdlib>
#include <thread>
#include <chrono>
#include <iostream>
#include <functional>
#include "Elapse.h"

using namespace std::literals;

int main()
{
	{
		elapse e{ [](std::chrono::system_clock::duration duration) {std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " milliseconds has elapsed." << std::endl; } };
		std::this_thread::sleep_for(3000ms);
	}
	return 0;
}