/*
*	@author huanmie<yonghenghuanmie@gmail.com>
*	@date 2022.4.26
*/


#if __cplusplus >= 202002L

// Don't forget <tuple> and <concept>
#include <set>
#include <list>
#include <tuple>
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

template<typename T1, typename T2>
struct A {};

template<typename T1, typename T2>
struct B {};

namespace ConstraintType
{
	ConstructBasicEligibleType(BasicEligibleType, Byte, Short, Integer, Long);

	// Layer:[1]
	AddTypeLayer(1, std::list);
	AddTypeLayer(1, std::deque);
	AddTypeLayer(1, std::vector);
	AddTypeLayer(1, std::forward_list);
	// For example: std::vector<Byte>
	ConstructEligibleType(EligibleType1, 1, 1, Byte, Short, Integer, Long);

	AddTypeLayer(2, std::set);
	// Layer:[2,1]. Use common layer with EligibleType1.
	// For example: std::set<std::vector<Integer>>
	ConstructEligibleType(EligibleType2, 2, 2, Byte, Short, Integer, Long);

	// Layer:[11,10]. Use different layer to avoid collision.
	AddTypeLayer(10, std::vector);
	AddTypeLayer(11, std::vector);
	// For example: std::vector<std::vector<Short>>
	ConstructEligibleType(EligibleType3, 2, 11, Byte, Short, Integer, Long);

	AddTypeLayerWithPosition(0x10, 1, A);
	// A<?,Byte>
	ConstructEligibleTypeWithPosition(EligibleType4, 1, 0x10, 1, Byte);

	// Same with AddTypeLayer(0x11, B); if B<T2> has default template parameter.
	AddTypeLayerWithPosition(0x11, 0, B);
	// Layer:[0x11,0x10]. Check B<0,1> at 0x11, then get B<0> parameter to check with A<0,1> at 0x10, finally check A<1> with Long.
	// So, arguments are 0x11,0,1.
	// B<A<?,Long>,?>
	ConstructEligibleTypeWithPosition(EligibleType5, 2, 0x11, 0, 1, Long);
	// equivalent to
	//template<typename T>
	//concept EligibleType5 = _EligibleUnderlyingType<std::enable_if_t<_IsEligibleType<std::enable_if_t<_IsEligibleType<T, 0x11 - 1 + 1, 0>::value, typename _IsEligibleType<T, 0x11 - 1 + 1, 0>::underlying_type>, 0x11 - 2 + 1, 1>::value, typename _IsEligibleType<std::enable_if_t<_IsEligibleType<T, 0x11 - 1 + 1, 0>::value, typename _IsEligibleType<T, 0x11 - 1 + 1, 0>::underlying_type>, 0x11 - 2 + 1, 1>::underlying_type>, Long>;;

	AddTypeLayerWithPosition(0x11, 1, B);
	// B<?,int>
	// You can combine EligibleType5 and EligibleType6 to construct more complicated constraints.
	ConstructEligibleTypeWithPosition(EligibleType6, 1, 0x11, 1, int);

	// Ignore the upper layer just focus on underlying type.
	// ?<Byte||Short||Integer||Long>
	AddTypeLayer(0x20, Any);
	ConstructEligibleType(EligibleType7, 1, 0x20, Byte, Short, Integer, Long);
}

class Test
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

	template<ConstraintType::EligibleType4 T>
	static void TestEligibleType4(const T& c) {}

	template<ConstraintType::EligibleType5 T>
	static void TestEligibleType5(const T& c) {}

	// B<A<?,Long>,int>
	template<typename T> requires ConstraintType::EligibleType5<T>&& ConstraintType::EligibleType6<T>
	static void TestCombinedEligibleType(const T& c) {}

	template<ConstraintType::EligibleType7 T>
	static void TestEligibleType7(const T& c) {}
};

int main()
{
	//Test::TestBasicEligibleType(1);									// Error: int not in basic eligible type set.
	Test::TestBasicEligibleType(Long{ 1 });								// OK

	//Test::TestEligibleType1(std::vector<int>());						// Error: int not in basic eligible type set.
	//Test::TestEligibleType1(std::set<Byte>());						// Error: std::set not in layer1's eligible type set
	Test::TestEligibleType1(std::vector<Byte>{ { 1 }, { 2 }, { 3 } });	// OK

	//Test::TestEligibleType2(std::vector<std::vector<Integer>>());		// Error: std::vector not in layer2's eligible type set
	//Test::TestEligibleType2(std::set<std::set<Integer>>());			// Error: std::set not in layer1's eligible type set
	//Test::TestEligibleType2(std::set<std::vector<int>>());			// Error: int not in basic eligible type set.
	Test::TestEligibleType2(std::set<std::vector<Integer>>());			// OK

	Test::TestEligibleType3(std::vector<std::vector<Short>>());			// OK

	//Test::TestEligibleType4(A<int, Long>());							// Error: second template parameter not in eligible type set
	Test::TestEligibleType4(A<int, Byte>());

	// B<A<?,Long>,?>													   
	//Test::TestEligibleType5(B<A<int, int>, int>());					// Error: int not in basic eligible type set.
	//Test::TestEligibleType5(B<B<int,Long>,int>());					// Error: B not in layer1's eligible type set
	//Test::TestEligibleType5(A<A<int, Long>, int>());					// Error: A not in layer2's eligible type set
	Test::TestEligibleType5(B<A<int, Long>, int>());					// OK
	Test::TestEligibleType5(B<A<double, Long>, int>());					// OK
	Test::TestEligibleType5(B<A<int, Long>, char>());					// OK

	// B<A<?,Long>,int>													   
	//Test::TestCombinedEligibleType(B<A<int, Long>, double>());		// Error: double doesn't meet EligibleType6's requirements(int)
	//Test::TestCombinedEligibleType(B<A<int, double>, int>());			// Error: double doesn't meet EligibleType5's requirements(Long)
	Test::TestCombinedEligibleType(B<A<int, Long>, int>());				// OK

	// ?<Byte||Short||Integer||Long>
	//Test::TestEligibleType7(std::vector<int>());						// Error: int not in basic eligible type set.
	Test::TestEligibleType7(std::vector<Long>());						// OK. Any
	Test::TestEligibleType7(A<Long, Byte>());							// OK. Any
	return 0;
}

#endif // __cplusplus >= 202002L
