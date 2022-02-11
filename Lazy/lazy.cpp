#include <cmath>
#include <typeinfo>
#include <iostream>
#include <functional>
#include <type_traits>
#include "Lazy.h"

int main()
{
	/*auto y00 = (lazy::Value(10) * 10)();
	auto y01 = (lazy::Value(10) * []() {return 10; })();
	auto y10 = (lazy::Value([]() {return 10; }) * 10)();
	auto y11 = (lazy::Value([]() {return 10; }) * []() {return 10; })();*/
	auto func_y1 = lazy::pow(1, lazy::pow(10, 2) * 5) * 10;
	auto func_y2 = (lazy::Value(1.0) + 10) * 20 / lazy::log(2) - []() {return 4 + 8; } - lazy::pow(4, 2);
	auto y1 = func_y1();
	auto y2 = func_y2();
	return 0;
}