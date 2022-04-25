#include <cstdlib>
#include <memory>
#include <string>
#include <optional>
#include <iostream>
#include <functional>
#include <string_view>
#include <unordered_map>
#include "StateMachine.h"

enum class Thread
{
	running, block, destroy
};

class A :public StateMachine<Thread, std::string, A>
{
public:
	A()
	{
		Register_Callable(Thread::running, [](A*) {std::cout << "running" << std::endl; return true; },
			Thread::block, [](A*) {std::cout << "block" << std::endl; return true; },
			Thread::destroy, [](A*) {std::cout << "destroy" << std::endl; return true; });

		Register_Relation(Thread::running, "to_block", Thread::block,
			Thread::block, "to_running", Thread::running,
			Thread::running, "to_destroy", Thread::destroy,
			Thread::block, "to_destroy", Thread::destroy);

		InitialState(Thread::running);
	}
private:
};

int main()
{
	A a;
	std::cout << a.PerformOperation("to_block") << std::endl;
	std::cout << a.PerformOperation("to_block") << std::endl;
	std::cout << a.RepeatOperation() << std::endl;
	std::cout << a.PerformOperation("to_destroy") << std::endl;
	std::cout << a.PerformOperation("to_running") << std::endl;
	std::system("pause");
	return 0;
}