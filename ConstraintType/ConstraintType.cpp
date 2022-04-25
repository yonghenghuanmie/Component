/*
*	@author huanmie<yonghenghuanmie@gmail.com>
*	@date 2022.4.26
*/

#include <set>
#include <list>
#include <deque>
#include <vector>
#include <concepts>
#include <iostream>
#include <forward_list>
#include "ConstraintType.h"

struct Byte { signed char value; };
struct Short { short value; };
struct Integer { int value; };
struct Long { long long value; };

namespace ConstraintType
{
	ConstructBasicEligibleType(BasicEligibleType, Byte, Short, Integer, Long);

	//[1]
	AddTypeLayer(1, std::list);
	AddTypeLayer(1, std::deque);
	AddTypeLayer(1, std::vector);
	AddTypeLayer(1, std::forward_list);
	Construct1LayerEligibleType(EligibleType1, 1, Byte, Short, Integer, Long);

	//[2,1]
	AddTypeLayer(2, std::set);
	Construct2LayerEligibleType(EligibleType2, 2, Byte, Short, Integer, Long);

	//Use different layer to avoid collision.[11,10]
	AddTypeLayer(10, std::vector);
	AddTypeLayer(11, std::vector);
	Construct2LayerEligibleType(EligibleType3, 11, Byte, Short, Integer, Long);
}

class A
{
public:
	template<ConstraintType::BasicEligibleType T>
	static void TestBasicEligibleType(const T& c) {}

	template<ConstraintType::EligibleType1 T>
	static void TestEligibleType1(const T& c)
	{
		for (auto&& i : c)
		{
			std::cout << (int)i.value << " ";
		}
	}

	template<ConstraintType::EligibleType2 T>
	static void TestEligibleType2(const T& c) {}

	template<ConstraintType::EligibleType3 T>
	static void TestEligibleType3(const T& c) {}
};

int main()
{
	//A::TestBasicEligibleType(1);										//Wrong: int not in basic eligible type set.
	A::TestBasicEligibleType(Long{ 1 });								//OK

	//A::TestEligibleType1(std::vector<int>());							//Wrong: int not in basic eligible type set.
	//A::TestEligibleType1(std::set<Byte>());							//Wrong: std::set not in layer1's eligible type set
	A::TestEligibleType1(std::vector<Byte>{ { 1 }, { 2 }, { 3 } });		//OK

	//A::TestEligibleType2(std::vector<std::vector<Integer>>());		//Wrong: std::vector not in layer2's eligible type set
	//A::TestEligibleType2(std::set<std::set<Integer>>());				//Wrong: std::set not in layer1's eligible type set
	//A::TestEligibleType2(std::set<std::vector<int>>());				//Wrong: int not in basic eligible type set.
	A::TestEligibleType2(std::set<std::vector<Integer>>());				//OK

	A::TestEligibleType3(std::vector<std::vector<Short>>());			//OK
	return 0;
}
