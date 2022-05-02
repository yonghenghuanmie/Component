# Component
## ConstraintType
### Overview
ConstraintType is a header library to constraint type in template parameters. It is useful and convenient for generic programming and provide backward compatibility from C++ 14 to C++ 20. When you use C++ 20 or greater standards it will use concept instead of constant expression.  
### Usage
```c++
using namespace ConstraintType;

ConstructBasicEligibleType(BasicEligibleType, char, short, int, long long);

template<BasicEligibleType T>
void TestBasicEligibleType(const T& c) {}

AddTypeLayer(1, std::list);
AddTypeLayer(1, std::deque);
AddTypeLayer(1, std::vector);
AddTypeLayer(1, std::forward_list);
ConstructEligibleType(EligibleType1, 1, 1, char, short, int, long long);

template<EligibleType1 T>
void TestEligibleType(const T& c) {}


int main()
{
	TestBasicEligibleType(1.0);			//Error
	TestBasicEligibleType(1);			//OK

	TestEligibleType(std::set<int>());		//Error
	TestEligibleType(std::vector<double>());	//Error
	TestEligibleType(std::vector<int>());		//OK
	return 0;
}
```  
For more usage and more powerful functionalities you can see [`ConstraintType/ConstraintType.cpp`](ConstraintType/ConstraintType.cpp) for concept or [`ConstraintType/ConstraintTypeBackwardCompatibility.cpp`](ConstraintType/ConstraintTypeBackwardCompatibility.cpp) for legacy.  
