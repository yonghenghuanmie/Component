#include <cstdlib>
#include <deque>
#include <mutex>
#include <thread>
#include <future>
#include <optional>
#include <iostream>
#include <condition_variable>
#include "ProducerConsumerQueue.h"

class A
{
public:
	A()
	{
		memory = new char[12]{ "hello,world" };
	}
	A(const A &a)
	{
		std::cout << "copy construct" << std::endl;
		memory = new char[12] { "hello,world" };
	}
	A(A &&a)
	{
		//std::cout << "move construct" << std::endl;
		memory = a.memory;
		a.memory = nullptr;
	}
	A& operator=(const A &a)
	{
		std::cout << "copy operator" << std::endl;
		//...
	}
	A& operator=(A &&a)
	{
		//std::cout << "move operator" << std::endl;
		delete[] memory;
		memory = a.memory;
		a.memory = nullptr;
	}
	operator char*()
	{
		return memory;
	}
	~A() { delete[] memory; }
private:
	char *memory = nullptr;
};

int main()
{
	ProducerConsumerQueue<A> pcq;
	//std::async([](ProducerConsumerQueue<int> &pcq){std::cout << pcq.Pop() << std::endl; },std::ref(pcq));
	//std::thread([](ProducerConsumerQueue<A> &pcq) {while (true) { std::cout << pcq.Pop() << std::endl; } }, std::ref(pcq)).detach();
	std::thread([](ProducerConsumerQueue<A> &pcq) {while (true) { auto &&temp = pcq.TryPop();if(temp) std::cout << temp.value() << std::endl; } }, std::ref(pcq)).detach();
	pcq.Push(A());
	A aaa;
	pcq.Push(aaa);
	std::system("pause");
	return 0;
}