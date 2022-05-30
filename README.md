# Component
## ConstraintType
### Overview
ConstraintType is a header library to constraint type in template parameters. It is useful and convenient for generic programming and provide backward compatibility from C++ 14 to C++ 20. When you use C++ 20 or greater standards it will use concept instead of constant expression.  
### Introduction
#### Constraint non-generic type with a type list
```c++
ConstructBasicEligibleType(BasicEligibleType, int, A, std::thread);   // ->int||A||std::thread
```
#### Constraint multi-dimension generic type
```c++
AddTypeLayer(0, std::list);
AddTypeLayer(0, std::deque);
AddTypeLayer(1, std::vector);
ConstructEligibleType(EligibleType, 2, 1, int);   // ->std::vector<std::list<int>||std::deque<int>>
```
#### Constraint template parameter with index
```c++
template<typename T1, typename T2>
struct A {};
AddTypeLayerWithPosition(0, 1, A);
ConstructEligibleTypeWithPosition(EligibleType, 1, 0, 1, double);   // ->A<?,double>
```
#### Use Any to skip matching on specified layer or underlying type
```c++
AddTypeLayer(0, Any);
ConstructEligibleType(EligibleType, 1, 0, char, short, int, long long);   // ->?<char||short||int||long long>
```
#### Constraint or get constant expression
```c++
template<typename T1, typename T2, int V>
struct A {};
AddValueLayerWithPosition(0, 2, A, T, T, V);
ConstructEligibleValueWithPosition(EligibleValue, 1, 0, 2, std::pair{ std::greater{}, 5 }, std::pair{ std::less{}, 10 }); // ->A<?,?,(5,10)>
```
#### Error message
`ErrorType<Type,Layer,Index>` will tell you compile failed on what Type which Layer and which Index.
`ErrorValue<ValueToBeChecked,Operator,ValueUserProvided>` will tell you compile failed on what ValueToBeChecked what Operator and what ValueUserProvided.
### Installation
If you are using cmake, then just add  
```cmake
# Show actual C++ version for MSVC
# Use compliant C11 preprocessor for MSVC
if(MSVC)
    target_compile_options(TARGET_NAME PRIVATE /Zc:__cplusplus /Zc:preprocessor)
    target_compile_definitions(TARGET_NAME PRIVATE ML99_ALLOW_POOR_DIAGNOSTICS)
# Metalang99 relies on heavy macro machinery. To avoid useless macro expansion
# errors, please write this:
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(TARGET_NAME PRIVATE -fmacro-backtrace-limit=1)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(TARGET_NAME PRIVATE -ftrack-macro-expansion=0)
endif()

include(FetchContent)
FetchContent_Declare(
    ConstraintType
    URL https://github.com/yonghenghuanmie/Component/archive/refs/tags/v1.3.0.tar.gz # v1.3.0
    SOURCE_SUBDIR ConstraintType
)
FetchContent_MakeAvailable(ConstraintType)
target_link_libraries(TARGET_NAME PRIVATE ConstraintType)
```
to your CMakeLists.txt and replace TARGET_NAME with correct target name.  
Or you can just download [`ConstraintType.h`](ConstraintType/ConstraintType.h) and download requirement [`metalang99`](https://github.com/Hirrolot/metalang99).
### Usage
For more example you can see [`ConstraintType/ConstraintType.cpp`](ConstraintType/ConstraintType.cpp) for concept or [`ConstraintType/ConstraintTypeBackwardCompatibility.cpp`](ConstraintType/ConstraintTypeBackwardCompatibility.cpp) for legacy.  
