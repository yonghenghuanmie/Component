# Component
## ConstraintType
### Overview
ConstraintType is a header library to constraint type in template parameters. It is useful and convenient for generic programming and provide backward compatibility from C++ 14 to C++ 20. When you use C++ 20 or greater standards it will use concept instead of constant expression.  
### Introduction
#### Constraint non-generic type with a type list
```c++
ConstructBasicEligibleType(BasicEligibleType, int, A, std::thread);	->	int||A||std::thread
```
#### Constraint multi-dimension generic type(support for at most 8 dimensions within one type)
```c++
AddTypeLayer(0, std::list);
AddTypeLayer(0, std::deque);
AddTypeLayer(1, std::vector);
ConstructEligibleType(EligibleType, 2, 1, int);				->	std::vector<std::list<int>||std::deque<int>>
```
#### Constraint template parameter with index
```c++
template<typename T1, typename T2>
struct A {};
AddTypeLayerWithPosition(0, 1, A);
ConstructEligibleTypeWithPosition(EligibleType, 1, 0, 1, double);	->	A<?,double>
```
#### Use Any to skip matching on specified layer
```c++
AddTypeLayer(0, Any);
ConstructEligibleType(EligibleType, 1, 0, char, short, int, long long);	->	?<char||short||int||long long>
```
### Usage
This is a single file header library, just inlucde [`ConstraintType.h`](ConstraintType/ConstraintType.h).  
For more example you can see [`ConstraintType/ConstraintType.cpp`](ConstraintType/ConstraintType.cpp) for concept or [`ConstraintType/ConstraintTypeBackwardCompatibility.cpp`](ConstraintType/ConstraintTypeBackwardCompatibility.cpp) for legacy.  
