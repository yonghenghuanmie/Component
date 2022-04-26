# Component
## ConstraintType
### Overview
ConstraintType is a header library to constraint type in template parameters based on concept. It is useful and convenient for generic programming.  
### Usage
`using namespace ConstraintType;

ConstructBasicEligibleType(BasicEligibleType, char, short, int, long long);

template<BasicEligibleType T>
void TestBasicEligibleType(const T& c) {}

AddTypeLayer(1, std::list);
AddTypeLayer(1, std::deque);
AddTypeLayer(1, std::vector);
AddTypeLayer(1, std::forward_list);
Construct1LayerEligibleType(EligibleType1, 1, char, short, int, long long);

template<EligibleType1 T>
static void TestEligibleType(const T& c) {}


int main()
{
	TestBasicEligibleType(1.0);					//Error
	TestBasicEligibleType(1);					//OK

	TestEligibleType(std::set<int>());			//Error
	TestEligibleType(std::vector<double>());	//Error
	TestEligibleType(std::vector<int>());		//OK
	return 0;
}`  
For more usage and more powerful functionalities you can see https://github.com/yonghenghuanmie/Component/blob/master/ConstraintType/ConstraintType.cpp.
