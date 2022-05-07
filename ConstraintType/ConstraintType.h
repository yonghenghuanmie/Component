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
	struct ErrorType;

	template<typename T, _Layer layer, _Index index>
	struct _NotEligibleType :std::false_type
	{
		// ErrorType will provide template parameter data when it goes wrong.
		constexpr static ErrorType<T, layer, index> _;
	};

	template<typename T, _Layer layer, _Index index>
	struct _IsAny :_NotEligibleType <T, layer, index> {};

	template<template<typename...> typename T, _Layer layer, _Index index, typename... Rest>
	struct _IsAny<T<Rest...>, layer, index> :std::conditional_t<_IsEligibleType<Any<Rest...>, layer, index>::value, std::true_type, _NotEligibleType<T<Rest...>, layer, index>>
	{
		using underlying_type = std::tuple_element_t<index, std::tuple<Rest...>>;
	};

	template<typename T, _Layer layer, _Index index>
	struct _IsEligibleType :_IsAny<T, layer, index> {};


	// Meta macro programming infrastructure
	// Support at most 8 layers.
#define _LayerZero_0	1
#define _LayerZero_1	0
#define _LayerZero_2	0
#define _LayerZero_3	0
#define _LayerZero_4	0
#define _LayerZero_5	0
#define _LayerZero_6	0
#define _LayerZero_7	0
#define _ZeroResolverAUX(value) _LayerZero_##value
#define _ZeroResolver(value) _ZeroResolverAUX(value)
#define _IsZero(value)	_ZeroResolver(value)

#define _LayerCalculate_1sub1	0
#define _LayerCalculate_2sub1	1
#define _LayerCalculate_3sub1	2
#define _LayerCalculate_4sub1	3
#define _LayerCalculate_5sub1	4
#define _LayerCalculate_6sub1	5
#define _LayerCalculate_7sub1	6
#define _LayerCalculate_8sub1	7
#define _Decrease1(x) _LayerCalculate_##x##sub1

#define _LayerCalculate_0add1	1
#define _LayerCalculate_1add1	2
#define _LayerCalculate_2add1	3
#define _LayerCalculate_3add1	4
#define _LayerCalculate_4add1	5
#define _LayerCalculate_5add1	6
#define _LayerCalculate_6add1	7
#define _LayerCalculate_7add1	8
#define _Increase1(x) _LayerCalculate_##x##add1

#define _If0(output1,output2) output2
#define _If1(output1,output2) output1
#define _IfConditionAUX(condition) _If##condition
#define _IfCondition(condition) _IfConditionAUX(condition)
#define _If(condition,output1,output2) _IfCondition(condition)(output1,output2)

#define _empty()
#define _defer(...) __VA_ARGS__ _empty()
#define _obstruct(...) __VA_ARGS__ _defer(_empty)()
#define _eval(...)  _eval1(_eval1(__VA_ARGS__))
#define _eval1(...) _eval2(_eval2(__VA_ARGS__))
#define _eval2(...) _eval3(_eval3(__VA_ARGS__))
#define _eval3(...) __VA_ARGS__

#define PP_ARG_N(																														\
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10,																						\
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20,																						\
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30,																						\
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40,																						\
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50,																						\
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60,																						\
         _61,_62,_63,N,...) N

#define _SizeOf(...) 																													\
	_eval(PP_ARG_N(__VA_ARGS__, 63,62,61,60,																							\
         59,58,57,56,55,54,53,52,51,50,																									\
         49,48,47,46,45,44,43,42,41,40,																									\
         39,38,37,36,35,34,33,32,31,30,																									\
         29,28,27,26,25,24,23,22,21,20,																									\
         19,18,17,16,15,14,13,12,11,10,																									\
         9,8,7,6,5,4,3,2,1,0))

#define _GetFirst(first,...) first
#define _GetFirstIndirect() _GetFirst
#define _GetRest(first,...) __VA_ARGS__
#define _GetBeginFrom(n,...) _If(_IsZero(n),__VA_ARGS__,_obstruct(_GetBeginFromIndirect)()(_Decrease1(n),_GetRest(__VA_ARGS__)))
#define _GetBeginFromIndirect() _GetBeginFrom
#define _GetNthElement(n,...) _obstruct(_GetFirstIndirect)()(_GetBeginFrom(n,##__VA_ARGS__))


	// Generate template for Type
	constexpr std::size_t _default_index = 0;
#define _GetUnderlyingType(LayerNumber,CurrentType)																						\
	std::enable_if_t<_IsEligibleType<CurrentType,LayerNumber,_default_index>::value,													\
	typename _IsEligibleType<CurrentType,LayerNumber,_default_index>::underlying_type>													

#define _GetUnderlyingTypeWithPosition(LayerNumber,CurrentType,TypeIndex)																\
	std::enable_if_t<_IsEligibleType<CurrentType,LayerNumber,TypeIndex>::value,															\
	typename _IsEligibleType<CurrentType,LayerNumber,TypeIndex>::underlying_type>														

#define _ConstructGetUnderlyingType(TotalLayer,StartLayer,T)																			\
	_GetUnderlyingType(StartLayer-TotalLayer+1,_If(_IsZero(_Decrease1(TotalLayer)),T,													\
	_obstruct(_ConstructGetUnderlyingTypeIndirect)()(_Decrease1(TotalLayer),StartLayer,T)))												
#define _ConstructGetUnderlyingTypeIndirect() _ConstructGetUnderlyingType																

#define _ConstructGetUnderlyingTypeWithPosition(TotalLayer,StartLayer,T,...)															\
	_GetUnderlyingTypeWithPosition(StartLayer-TotalLayer+1,_If(_IsZero(_Decrease1(TotalLayer)),T,										\
	_obstruct(_ConstructGetUnderlyingTypeWithPositionIndirect)()(_Decrease1(TotalLayer),StartLayer,T,##__VA_ARGS__)),					\
	_GetNthElement(_Decrease1(TotalLayer),##__VA_ARGS__))
#define _ConstructGetUnderlyingTypeWithPositionIndirect() _ConstructGetUnderlyingTypeWithPosition


	// Value check infrastructure
#if __cplusplus >= 201703L

	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
	struct ErrorValue;

	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
	struct _NotEligibleValue :std::false_type
	{
		// ErrorValue will provide template parameter data when it goes wrong.
		constexpr static ErrorValue<ValueToBeChecked, Operator, ValueUserProvided> _;
	};

#if __cplusplus < 202002L
	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
	struct _IsEligibleValue :std::conditional_t < Operator{}(ValueToBeChecked, ValueUserProvided), std::true_type, _NotEligibleValue<ValueToBeChecked, Operator, ValueUserProvided >> {};

	template<auto ValueToBeChecked, typename Operator, auto ValueUserProvided>
	constexpr bool _EligibleUnderlyingValue = _IsEligibleValue<ValueToBeChecked, Operator, ValueUserProvided>::value;
#else
	template<auto ValueToBeChecked, auto Operator, auto ValueUserProvided>
	struct _IsEligibleValue :std::conditional_t < Operator(ValueToBeChecked, ValueUserProvided), std::true_type, _NotEligibleValue<ValueToBeChecked, decltype(Operator), ValueUserProvided >> {};

	// Rest must have first and second.
	template<auto Value, auto... Rest>
	constexpr bool _EligibleUnderlyingValue = std::conjunction_v<_IsEligibleValue<Value, Rest.first, Rest.second>...>;
#endif // __cplusplus < 202002L

#endif // __cplusplus >= 201703L


	// Generate template for Value
#define _TShortNameAUX(n) T##n
#define _TShortName(n) _TShortNameAUX(n)
#define _VShortNameAUX(n) V##n
#define _VShortName(n) _VShortNameAUX(n)
#define _ShortNameAUX(str) _##str##ShortName
#define _ShortName(str) _ShortNameAUX(str)
#define _GetShortName(n,str) _ShortName(str)(n)

#define _TLongNameAUX(n) typename T##n
#define _TLongName(n) _TLongNameAUX(n)
#define _VLongNameAUX(n) auto V##n
#define _VLongName(n) _VLongNameAUX(n)
#define _LongNameAUX(str) _##str##LongName
#define _LongName(str) _LongNameAUX(str)
#define _GetLongName(n,str) _LongName(str)(n)

#define _ConcatComma(x) ,##x

#define _ConstructTemplateParameters(Total,Current,first,...)																			\
	_GetLongName(Current,first) _If(_IsZero(_Decrease1(Total)),_empty(),																\
	_ConcatComma(_obstruct(_ConstructTemplateParametersIndirect)()(_Decrease1(Total),_Increase1(Current),__VA_ARGS__)))
#define _ConstructTemplateParametersIndirect() _ConstructTemplateParameters

#define _ConstructSpecializeParameters(Total,Current,first,...)																			\
	_GetShortName(Current,first) _If(_IsZero(_Decrease1(Total)),_empty(),																\
	_ConcatComma(_obstruct(_ConstructSpecializeParametersIndirect)()(_Decrease1(Total),_Increase1(Current),__VA_ARGS__)))
#define _ConstructSpecializeParametersIndirect() _ConstructSpecializeParameters

#define _GetUnderlyingValueWithPosition(LayerNumber,CurrentType,ValueIndex)																\
	_IsEligibleType<CurrentType,LayerNumber,ValueIndex>::underlying_value

#define _ConstructGetUnderlyingValueWithPosition(TotalLayer,StartLayer,T,...)															\
	_GetUnderlyingValueWithPosition(StartLayer-TotalLayer+1,_If(_IsZero(_Decrease1(TotalLayer)),T,										\
	_ConstructGetUnderlyingTypeWithPosition(_Decrease1(TotalLayer),StartLayer,T,##__VA_ARGS__)),										\
	_GetNthElement(_Decrease1(TotalLayer),##__VA_ARGS__))


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
#define ConstructBasicEligibleType(Name,...)																							\
	template<typename T>																												\
	Concept Name = _EligibleUnderlyingType<T,##__VA_ARGS__>;

	/// @notice For multi-dimension type you need to AddTypeLayer first then ConstructEligibleType,
	/// they are methods without specify the index of this type.
	/// If you look for methods which can specify index then go look WithPosition version.
	/// @param LayerNumber is which layer you want to add.
	/// @param Type is which type add to the layer.
#define AddTypeLayer(LayerNumber,Type)																									\
	template<typename... Rest>																											\
	struct _IsEligibleType<Type<Rest...>,LayerNumber,_default_index> :std::true_type													\
	{																																	\
		using underlying_type = std::tuple_element_t<_default_index,std::tuple<Rest...>>;												\
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
#define ConstructEligibleType(Name,TotalLayer,StartLayer,...)																			\
	template<typename T>																												\
	Concept Name = _EligibleUnderlyingType<_eval(_ConstructGetUnderlyingType(TotalLayer,StartLayer,T)),##__VA_ARGS__>;


	/// @notice For multi-dimension type you need to AddTypeLayerWithPosition first then ConstructEligibleTypeWithPosition.
	/// @param LayerNumber is which layer you want to add.
	/// @param TypeIndex is the index you specified for the Type. The AddTypeLayer's default TypeIndex is 0
	/// which means they always constraint the first parameter in template parameter list.
	/// e.g: You want to constraint T2 in A<T1,T2> you can't do that without WithPosition version. 
	/// @param Type is which type add to the layer.
#define AddTypeLayerWithPosition(LayerNumber,TypeIndex,Type)																			\
	template<typename... Rest>																											\
	struct _IsEligibleType<Type<Rest...>,LayerNumber,TypeIndex> :std::true_type															\
	{																																	\
		using underlying_type = std::tuple_element_t<TypeIndex,std::tuple<Rest...>>;													\
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
#define ConstructEligibleTypeWithPosition(Name,TotalLayer,StartLayer,...)																\
	template<typename T>																												\
	Concept Name = _EligibleUnderlyingType<_eval(_ConstructGetUnderlyingTypeWithPosition(TotalLayer,StartLayer,T,##__VA_ARGS__)),		\
		_eval(_GetBeginFrom(TotalLayer,##__VA_ARGS__))>;


#if __cplusplus >= 201703L

#if __cplusplus < 202002L

	/// @notice For non-type-involving constant expression you can directly use this. Like 8.
	/// In C++17 you can only impose 1 restrict per a macro call.
	/// @param Name is name of constant expression which you defined.
	/// @param Operator is a operator type like std::greater.
	/// NOTICE: This version's interface is different with C++20 version, you should pass a Operator TYPE NOT a Operator OBJECT.
	/// @param ValueUserProvided is the value you want to compared with.
	/// e.g: ConstructBasicEligibleValue(EligibleValue, std::greater<void>, 5); is equivalent to ValueToBeChecked > 5.
#define ConstructBasicEligibleValue(Name, Operator, ValueUserProvided)																	\
	template<auto ValueToBeChecked>																										\
	constexpr bool Name = _EligibleUnderlyingValue<ValueToBeChecked, Operator, ValueUserProvided>;

#else

	/// @notice For non-type-involving constant expression you can directly use this. Like 8.
	/// @param Name is name of constant expression which you defined.
	/// @param ... is a std::pair list contains all the restrict for the value.
	/// The first value of pair should be operator object like std::greater{},
	/// the second value of pair is the value you want to compared with.
	/// e.g: ConstructBasicEligibleValue(EligibleValue, std::pair{ std::greater{}, 5 }); is equivalent to ValueToBeChecked > 5.
#define ConstructBasicEligibleValue(Name,...)																							\
	template<auto ValueToBeChecked>																										\
	constexpr bool Name = _EligibleUnderlyingValue<ValueToBeChecked, ##__VA_ARGS__>;

#endif // __cplusplus < 202002L

	/// @notice NOTE: For multi-dimension type which contains constant expression you MUST call to AddValueLayerWithPosition first,
	/// then you can call ConstructEligibleTypeWithPosition or ConstructEligibleValueWithPosition depends on what you want get.
	/// @param LayerNumber is which layer you want to add.
	/// @param ValueIndex is the index you specified for the value.
	/// @param Type is which type add to the layer.
	/// @param ... is a list to describe layout of template parameters of a specific type you add. Use T and V to refer to Type and Value respective.
	/// e.g: If you want to constraint A<Type,Type,Value> then you should call macro like this AddValueLayerWithPosition(LayerNumber,2,Type,T,T,V);
#define AddValueLayerWithPosition(LayerNumber,ValueIndex,Type,...)																		\
	template<_eval(_ConstructTemplateParameters(_SizeOf(__VA_ARGS__),0,__VA_ARGS__))>													\
	struct _IsEligibleType<Type<_eval(_ConstructSpecializeParameters(_SizeOf(__VA_ARGS__),0,__VA_ARGS__))>,LayerNumber,ValueIndex>		\
	{																																	\
		constexpr static auto underlying_value = _GetShortName(ValueIndex,_eval(_GetNthElement(ValueIndex,__VA_ARGS__)));				\
		using underlying_type = decltype(underlying_value);																				\
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
#define ConstructEligibleValueWithPosition(Name,TotalLayer,StartLayer,...)																\
	template<typename T>																												\
	constexpr bool Name = _EligibleUnderlyingValue<																						\
		_eval(_ConstructGetUnderlyingValueWithPosition(TotalLayer,StartLayer,T,##__VA_ARGS__)),											\
		_eval(_GetBeginFrom(TotalLayer,##__VA_ARGS__))>;

	/// @notice A macro function to help you to get value in template parameter. All the parameters is same with
	/// ConstructEligibleValueWithPosition except there is no rest part in ... .
#define GetUnderlyingValue(Name,TotalLayer,StartLayer,...)																				\
	template<typename T>																												\
	constexpr auto Name = _eval(_ConstructGetUnderlyingValueWithPosition(TotalLayer,StartLayer,T,##__VA_ARGS__));

#endif // __cplusplus >= 201703L
}