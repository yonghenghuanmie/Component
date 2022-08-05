#include <variant>
#include <iostream>
#include <type_traits>
#include "TypeCast.h"

struct Base { virtual ~Base() {} };
struct Derived1 :Base {};
struct Derived2 :Base {};
struct Derived3 :Derived1 {};
struct Derived4 :Derived3 {};

struct Dispatcher
{
	void operator()(Derived1*) { std::cout << "Derived1\n"; }
	void operator()(Derived2*) { std::cout << "Derived2\n"; }
	void operator()(Derived3*) { std::cout << "Derived3\n"; }
	void operator()(Derived4*) { std::cout << "Derived4\n"; }
	template<typename T>
	void operator()(T*) { std::cout << "Not found\n"; }
};

int main()
{
	Base base;
	Derived4 derived4;
	std::visit(Dispatcher{}, TypeCast::InstanceOf<Derived1*, Derived2*, Derived3*, Derived4*>((Base*)&derived4));
	std::visit(Dispatcher{}, TypeCast::InstanceOf<Derived4*, Derived3*, Derived2*, Derived1*>((Base*)&derived4));
	std::visit(Dispatcher{}, TypeCast::InstanceOf<Derived1*, Derived2*, Derived3*, Derived4*>(&base));
	std::visit(Dispatcher{}, TypeCast::InstanceOf<>((Base*)&derived4));
	return 0;
}