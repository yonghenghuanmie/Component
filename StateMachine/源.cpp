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
	running,block,empty
};

class A :public StateMachine<Thread,A>
{
public:
	A()
	{
		Register_Callable("stop", [](A*) {std::cout << "stop" << std::endl; },
			"destroy",[](A*) {std::cout << "destroy" << std::endl; });
	}
private:
};

int main()
{
	A a;
	return 0;
}