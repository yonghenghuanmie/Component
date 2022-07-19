#include <cstdlib>
#include <memory>
#include <string>
#include <numeric>
#include <optional>
#include <iostream>
#include <execution>
#include <algorithm>
#include <functional>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include "Graph.h"
#include "StateMachine.h"

enum class Thread
{
	running, block, destroy
};

class A :public StateMachine<Thread, std::string, A>
{
public:
	A() :StateMachine(Thread::running, "",
		Thread::running, "to_block", Thread::block,
		Thread::block, "to_running", Thread::running,
		Thread::running, "to_destroy", Thread::destroy,
		Thread::block, "to_destroy", Thread::destroy
	)
	{
		auto print_state = [](A*, Thread old_state, Thread new_state) {std::cout << "transform from " << (int)old_state << " to " << (int)new_state << std::endl; return true; };
		RegisterCallback(Thread::running, print_state, Thread::block, print_state, Thread::destroy, print_state);
	}
};

int main()
{
	A a;
	std::cout << std::boolalpha
		<< a.PerformOperation("to_block") << "\n"				// true
		<< a.PerformOperation("to_block") << "\n"				// false
		<< a.PerformOperation("to_destroy") << "\n"				// true
		<< a.PerformOperation("to_running") << std::endl;		// false
	return 0;
}