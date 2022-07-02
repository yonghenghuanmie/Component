#pragma once

/*
*	@author huanmie<yonghenghuanmie@gmail.com>
*	@date 2022.4.26
*/

namespace ConstraintType
{
	/************************************************************************************************************************/
	/*												Internal implementation													*/
	/************************************************************************************************************************/

	// check if concept available.
#define _BackwardCompatibility
#if __cplusplus >= 202002L
#ifdef __cpp_concepts
#ifdef __cpp_lib_concepts

#undef _BackwardCompatibility

#endif // __cpp_lib_concepts
#endif // __cpp_concepts
#endif // __cplusplus >= 202002L


	// Type check infrastructure
	template<typename... T> struct Any;
#ifdef _BackwardCompatibility

#if __cplusplus < 201402L

#error "Only compilers that implement standards greater than C++14 are supported!"

#elif __cplusplus < 201703L
	template<typename... Rest>
	struct disjunction :std::false_type {};

	template<typename T, typename... Rest>
	struct disjunction<T, Rest...> :std::conditional_t<T::value, T, disjunction<Rest...>> {};

	template<typename T>
	struct disjunction<T> :T {};

	template<typename... Rest>
	constexpr bool disjunction_v = disjunction<Rest...>::value;

#elif __cplusplus < 202002L

	using std::disjunction_v;

#endif // __cplusplus < 201402L

#define Concept constexpr bool
	template<typename T, typename... Rest>
	Concept _EligibleUnderlyingType = disjunction_v<std::is_same<T, Rest>...> || disjunction_v<std::is_same<Any<void>, Rest>...>;

#else

#define Concept concept
	template<typename T, typename... Rest>
	Concept _EligibleUnderlyingType = ((std::same_as<T, Rest> || std::same_as<Any<void>, Rest>) || ...);

#endif // _BackwardCompatibility



	using _Layer = std::size_t;
	using _Index = std::size_t;
	template<typename T, _Layer layer, _Index index>
	struct _IsEligibleType;

	template<typename T, _Layer layer, _Index index>
#ifdef ShowErrorMessage
	struct ErrorType;
#else
	struct ErrorType {};
#endif // ShowErrorMessage

	template<typename T, _Layer layer, _Index index>
	struct _NotEligibleType :std::false_type
	{
		// ErrorType will provide template parameter data when it goes wrong.
		constexpr static ErrorType<T, layer, index> _;
	};

	template<typename T, _Layer layer, _Index index>
	struct _IsAny :_NotEligibleType <T, layer, index> {};

	template<_Layer layer, _Index index, typename... Rest>
	struct _IsAny<Any<Rest...>, layer, index> :std::false_type {};

	template<template<typename...> typename T, _Layer layer, _Index index, typename... Rest>
	struct _IsAny<T<Rest...>, layer, index>
		:std::conditional_t<_IsEligibleType<Any<Rest...>, layer, index>::value, std::true_type, _NotEligibleType<T<Rest...>, layer, index>>
	{
		using underlying_type = std::tuple_element_t<index, std::tuple<Rest...>>;
	};

	template<typename T, _Layer layer, _Index index>
	struct _IsEligibleType :_IsAny<T, layer, index> {};



	constexpr std::size_t _default_index = 0;
	template<std::size_t TotalLayer, std::size_t StartLayer, typename T>
	struct _ConstructGetUnderlyingType :
		_ConstructGetUnderlyingType<TotalLayer - 1, StartLayer - 1, typename _IsEligibleType<T, StartLayer, _default_index>::underlying_type> {};

	template<std::size_t StartLayer, typename T>
	struct _ConstructGetUnderlyingType<0, StartLayer, T>
	{
		using final_type = T;
	};

	template<std::size_t TotalLayer, std::size_t StartLayer, typename T>
	using _ConstructGetUnderlyingType_t = typename _ConstructGetUnderlyingType<TotalLayer, StartLayer, T>::final_type;

	template<std::size_t TotalLayer, std::size_t StartLayer, typename T, std::size_t First, std::size_t... Rest>
	struct _ConstructGetUnderlyingTypeWithPosition :
		_ConstructGetUnderlyingTypeWithPosition<TotalLayer - 1, StartLayer - 1, typename _IsEligibleType<T, StartLayer, First>::underlying_type, Rest...>
	{
		static_assert(TotalLayer != 0, "TotalLayer must greater than 0, please check your input!");
	};

	template<std::size_t StartLayer, typename T, std::size_t First>
	struct _ConstructGetUnderlyingTypeWithPosition<1, StartLayer, T, First>
	{
		using final_type = typename _IsEligibleType<T, StartLayer, First>::underlying_type;
	};

	template<std::size_t TotalLayer, std::size_t StartLayer, typename T, std::size_t... Rest>
	using _ConstructGetUnderlyingTypeWithPosition_t = typename _ConstructGetUnderlyingTypeWithPosition<TotalLayer, StartLayer, T, Rest...>::final_type;



	// Value check infrastructure
#if __cplusplus >= 201703L

	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
#ifdef ShowErrorMessage
	struct ErrorValue;
#else
	struct ErrorValue {};
#endif // ShowErrorMessage

	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
	struct _NotEligibleValue :std::false_type
	{
		// ErrorValue will provide template parameter data when it goes wrong.
		constexpr static ErrorValue<ValueToBeChecked, Operator, ValueUserProvided> _;
	};

#if __cplusplus < 202002L
	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
	struct _IsEligibleValue
		:std::conditional_t < Operator{}(ValueToBeChecked, ValueUserProvided), std::true_type, _NotEligibleValue<ValueToBeChecked, Operator, ValueUserProvided >> {};

	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
	constexpr bool _EligibleUnderlyingValue = _IsEligibleValue<ValueToBeChecked, Operator, ValueUserProvided>::value;
#else
	template<auto ValueToBeChecked, auto Operator, auto ValueUserProvided>
	struct _IsEligibleValue
		:std::conditional_t < Operator(ValueToBeChecked, ValueUserProvided), std::true_type, _NotEligibleValue<ValueToBeChecked, decltype(Operator), ValueUserProvided >> {};

	/// @notice _CompareRequirement is for Rest... template parameters.
	/// @param Every object in Rest... must have first and second member,
	/// first is a callable object which it needs two parameters and return boolean type,
	/// second is an object passed to first as an argument.
	template<auto... Rest>
	concept _CompareRequirement = ((requires{ { Rest.first(Rest.second, Rest.second) }->std::same_as<bool>; })&&...);

	template<auto Value, auto... Rest> requires _CompareRequirement<Rest...>
	constexpr bool _EligibleUnderlyingValue = std::conjunction_v<_IsEligibleValue<Value, Rest.first, Rest.second>...>;
#endif // __cplusplus < 202002L

#endif // __cplusplus >= 201703L



	// Generate template for Value
#define _PARAM_T typename
#define _PARAM_V auto
#define _GetLongName_IMPL(x, i)		v(_PARAM_##x x##i)
#define _GetLongName_ARITY			2
#define _GetShortName_IMPL(x, i)	v(x##i)
#define _GetShortName_ARITY			2

#define _ConstructTemplateParameters(...) ML99_LIST_EVAL_COMMA_SEP(ML99_listMapI(v(_GetLongName), ML99_list(__VA_ARGS__)))
#define _ConstructSpecializeParameters(...) ML99_LIST_EVAL_COMMA_SEP(ML99_listMapI(v(_GetShortName), ML99_list(__VA_ARGS__)))


	template<std::size_t TotalLayer, std::size_t StartLayer, typename T, std::size_t First, std::size_t... Rest>
	struct _ConstructGetUnderlyingValueWithPosition :
		_ConstructGetUnderlyingValueWithPosition<TotalLayer - 1, StartLayer - 1, typename _IsEligibleType<T, StartLayer, First>::underlying_type, Rest...>
	{
		static_assert(TotalLayer != 0, "TotalLayer must greater than 0, please check your input!");
	};

	template<std::size_t StartLayer, typename T, std::size_t First>
	struct _ConstructGetUnderlyingValueWithPosition<1, StartLayer, T, First>
	{
		constexpr static auto final_value = _IsEligibleType<T, StartLayer, First>::underlying_value;
	};

	template<std::size_t TotalLayer, std::size_t StartLayer, typename T, std::size_t... Rest>
	constexpr auto _ConstructGetUnderlyingValueWithPosition_v = _ConstructGetUnderlyingValueWithPosition<TotalLayer, StartLayer, T, Rest...>::final_value;



	/************************************************************************************************************************/
	/*												External interface														*/
	/************************************************************************************************************************/

	/// @notice Any is used to skip current type constraint. Only for intermediate layer use(means except for underlying type check).
	template<typename... T>
	struct Any {};

	/// @notice Any<void> is used to skip underlying type constraint.
	template<>
	struct Any<void> {};

	/// @notice For single dimension type you can directly use this. Like int, std::thread.
	/// @param Name is name of either concept for newer version or constant expression for older version which you defined.
	/// @param ... is a type list to constraint type.
#define ConstructBasicEligibleType(Name,...)																														\
	template<typename T>																																			\
	Concept Name = _EligibleUnderlyingType<T,##__VA_ARGS__>;

	/// @notice For multi-dimension type you need to AddTypeLayer first then ConstructEligibleType,
	/// they are methods without specify the index of this type.
	/// If you look for methods which can specify index then go look WithPosition version.
	/// @param LayerNumber is which layer you want to add.
	/// @param Type is which type add to the layer.
#define AddTypeLayer(LayerNumber,Type)																																\
	template<typename... Rest>																																		\
	struct _IsEligibleType<Type<Rest...>,LayerNumber,_default_index> :std::true_type																				\
	{																																								\
		using underlying_type = std::tuple_element_t<_default_index,std::tuple<Rest...>>;																			\
	};

	/// @notice For multi-dimension type you need to AddTypeLayer first then ConstructEligibleType,
	/// they are methods without specify the index of this type.
	/// If you look for methods which can specify index then go look WithPosition version.
	/// @param Name is name of either concept for newer version or constant expression for older version which you defined.
	/// @param TotalLayer is total layer you want to use.
	/// @param StartLayer is start form which layer, you should add type to the correspond layer first.
	/// e.g: If TotalLayer is 2 and StartLayer is 10, then it will use 10,9. So you should call AddTypeLayer first.
	/// @param ... this rest of parameters are list of basic type, which equivalent to ConstructBasicEligibleType.
	/// 
	/// e.g: If you want to constraint type to std::vector<int> then TotalLayer should be 1, StartLayer should
	/// be the same layer you called AddTypeLayer with Type std::vector and the rest of parameter is int.
#define ConstructEligibleType(Name,TotalLayer,StartLayer,...)																										\
	template<typename T>																																			\
	Concept Name = _EligibleUnderlyingType<_ConstructGetUnderlyingType_t<TotalLayer, StartLayer, T>, ##__VA_ARGS__>;


	/// @notice For multi-dimension type you need to AddTypeLayerWithPosition first then ConstructEligibleTypeWithPosition.
	/// @param LayerNumber is which layer you want to add.
	/// @param TypeIndex is the index you specified for the Type. The AddTypeLayer's default TypeIndex is 0
	/// which means they always constraint the first parameter in template parameter list.
	/// e.g: You want to constraint T2 in A<T1,T2> you can't do that without WithPosition version. 
	/// @param Type is which type add to the layer.
#define AddTypeLayerWithPosition(LayerNumber,TypeIndex,Type)																										\
	template<typename... Rest>																																		\
	struct _IsEligibleType<Type<Rest...>,LayerNumber,TypeIndex> :std::true_type																						\
	{																																								\
		using underlying_type = std::tuple_element_t<TypeIndex,std::tuple<Rest...>>;																				\
	};

	/// @notice For multi-dimension type you need to AddTypeLayerWithPosition first then ConstructEligibleTypeWithPosition.
	/// @param Name is name of either concept for newer version or constant expression for older version which you defined.
	/// @param TotalLayer is total layer you want to use.
	/// @param StartLayer is start form which layer, you should add type to the correspond layer first.
	/// e.g: If TotalLayer is 2 and StartLayer is 10, then it will use 10,9. So you should call AddTypeLayerWithPosition first.
	/// @param ... the first n element are TypeIndex, n equal to the TotalLayer.
	/// TypeIndex is the index you specified for the Type. The ConstructEligibleType's default TypeIndex is 0
	/// which means they always constraint the first parameter in template parameter list.
	/// e.g: You want to constraint T2 in A<T1,T2> you can't do that without WithPosition version. 
	/// The rest of parameters are list of basic type, which equivalent to ConstructBasicEligibleType.
	/// 
	/// e.g: If you want to constraint type to A<?,int> then TotalLayer should be 1,
	/// first element of ... is the same you specified in AddTypeLayerWithPosition's TypeIndex equals 1,
	/// StartLayer should be the same layer you called AddTypeLayerWithPosition with Type A and the rest of parameter is int.
#define ConstructEligibleTypeWithPosition(Name,TotalLayer,StartLayer,...)																							\
	template<typename T>																																			\
	Concept Name = _EligibleUnderlyingType<																															\
		_ConstructGetUnderlyingTypeWithPosition_t<TotalLayer,StartLayer,T,ML99_LIST_EVAL_COMMA_SEP(ML99_listTake(v(TotalLayer),ML99_list(v(__VA_ARGS__))))>,		\
		ML99_LIST_EVAL_COMMA_SEP(ML99_listDrop(v(TotalLayer),ML99_list(v(__VA_ARGS__))))																			\
	>;

#if __cplusplus >= 201703L

#if __cplusplus < 202002L

	/// @notice For non-type-involving constant expression you can directly use this. Like 8.
	/// In C++17 you can only impose 1 restrict per a macro call.
	/// @param Name is name of constant expression which you defined.
	/// @param Operator is a operator type like std::greater.
	/// NOTICE: This version's interface is different with C++20 version, you should pass a Operator TYPE NOT a Operator OBJECT.
	/// @param ValueUserProvided is the value you want to compared with.
	/// e.g: ConstructBasicEligibleValue(EligibleValue, std::greater<void>, 5); is equivalent to ValueToBeChecked > 5.
#define ConstructBasicEligibleValue(Name, Operator, ValueUserProvided)																								\
	template<auto ValueToBeChecked>																																	\
	constexpr bool Name = _EligibleUnderlyingValue<ValueToBeChecked, Operator, ValueUserProvided>;

#else

	/// @notice For non-type-involving constant expression you can directly use this. Like 8.
	/// @param Name is name of constant expression which you defined.
	/// @param ... is a std::pair list contains all the restrict for the value.
	/// The first value of pair should be operator object like std::greater{},
	/// the second value of pair is the value you want to compared with.
	/// e.g: ConstructBasicEligibleValue(EligibleValue, std::pair{ std::greater{}, 5 }); is equivalent to ValueToBeChecked > 5.
#define ConstructBasicEligibleValue(Name,...)																														\
	template<auto ValueToBeChecked>																																	\
	constexpr bool Name = _EligibleUnderlyingValue<ValueToBeChecked, ##__VA_ARGS__>;

#endif // __cplusplus < 202002L

	/// @notice NOTE: For multi-dimension type which contains constant expression you MUST call to AddValueLayerWithPosition first,
	/// then you can call ConstructEligibleTypeWithPosition or ConstructEligibleValueWithPosition depends on what you want get.
	/// @param LayerNumber is which layer you want to add.
	/// @param ValueIndex is the index you specified for the value.
	/// @param Type is which type add to the layer.
	/// @param ... is a list to describe layout of template parameters of a specific type you add. Use T and V to refer to Type and Value respective.
	/// e.g: If you want to constraint A<Type,Type,Value> then you should call macro like this AddValueLayerWithPosition(LayerNumber,2,Type,T,T,V);
#define AddValueLayerWithPosition(LayerNumber,ValueIndex,Type,...)																									\
	template<_ConstructTemplateParameters(v(__VA_ARGS__))>																											\
	struct _IsEligibleType<Type<_ConstructSpecializeParameters(v(__VA_ARGS__))>,LayerNumber,ValueIndex>																\
	{																																								\
		constexpr static auto underlying_value = ML99_EVAL(ML99_cat(ML99_listGet(v(ValueIndex),ML99_list(v(__VA_ARGS__))),v(ValueIndex)));							\
		using underlying_type = decltype(underlying_value);																											\
	};

	/// @notice For multi-dimension type you need to AddValueLayerWithPosition first then ConstructEligibleValueWithPosition.
	/// @param Name is name of either concept for newer version or constant expression for older version which you defined.
	/// @param TotalLayer is total layer you want to use.
	/// @param StartLayer is start form which layer, you should add type to the correspond layer first.
	/// e.g: If TotalLayer is 2 and StartLayer is 10, then it will use 10,9. So you should call AddValueLayerWithPosition first.
	/// @param ... the first n element are ValueIndex, n equal to the TotalLayer. ValueIndex is the index you specified for the Value.
	/// The rest of parameters are constraints you want to applied to the value. NOTE: C++17 version's ConstructEligibleValueWithPosition
	/// is different with C++20. In C++17 version you can only pass an operator and a value to this macro. Plus operator MUST be TYPE in C++17.
	/// 
	/// e.g: If you want to constraint A<Type,Type,Value>, make sure it's value is in range(5,10), 
	/// then you should call AddValueLayerWithPosition first, next call
	/// ConstructEligibleValueWithPosition(Name,1,StartLayer,std::pair{ std::greater{}, 5 }, std::pair{ std::less{}, 10 });
#define ConstructEligibleValueWithPosition(Name,TotalLayer,StartLayer,...)																							\
	template<typename T>																																			\
	constexpr bool Name = _EligibleUnderlyingValue<																													\
		_ConstructGetUnderlyingValueWithPosition_v<TotalLayer,StartLayer,T,ML99_LIST_EVAL_COMMA_SEP(ML99_listTake(v(TotalLayer),ML99_list(v(__VA_ARGS__))))>,		\
		ML99_LIST_EVAL_COMMA_SEP(ML99_listDrop(v(TotalLayer),ML99_list(v(__VA_ARGS__))))																			\
	>;

	/// @notice A macro function to help you to get value in template parameter. All the parameters is same with
	/// ConstructEligibleValueWithPosition except there is no rest part in ... .
#define ConstructGetUnderlyingValue(FunctionName,TotalLayer,StartLayer,...)																							\
	template<typename T>																																			\
	constexpr auto FunctionName(T&&)																																\
	{																																								\
		return _ConstructGetUnderlyingValueWithPosition_v<TotalLayer, StartLayer, T, __VA_ARGS__>;																	\
	}

#endif // __cplusplus >= 201703L
}