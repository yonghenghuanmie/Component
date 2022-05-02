#pragma once

/*
*	@author huanmie<yonghenghuanmie@gmail.com>
*	@date 2022.4.26
*/

namespace ConstraintType
{
	// check if concept available.
#define _BackwardCompatibility
#if __cplusplus >= 202002L
#ifdef __cpp_concepts
#ifdef __cpp_lib_concepts

#undef _BackwardCompatibility

#endif // __cpp_lib_concepts
#endif // __cpp_concepts
#endif // __cplusplus >= 202002L


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
	Concept _EligibleUnderlyingType = disjunction_v<std::is_same<T, Rest>...>;

#else

#define Concept concept
	template<typename T, typename... Rest>
	Concept _EligibleUnderlyingType = (std::same_as<T, Rest> || ...);

#endif // _BackwardCompatibility


	using _Layer = std::size_t;
	using _Index = std::size_t;
	template<typename T, _Layer layer, _Index index>
	struct _IsEligibleType :std::false_type {};

	constexpr std::size_t default_index = 0;
#define _GetUnderlyingType(LayerNumber,CurrentType)														\
	std::enable_if_t<_IsEligibleType<CurrentType,LayerNumber,default_index>::value,						\
	typename _IsEligibleType<CurrentType,LayerNumber,default_index>::underlying_type>

#define _GetUnderlyingTypeWithPosition(LayerNumber,CurrentType,TypeIndex)								\
	std::enable_if_t<_IsEligibleType<CurrentType,LayerNumber,TypeIndex>::value,							\
	typename _IsEligibleType<CurrentType,LayerNumber,TypeIndex>::underlying_type>


#define _LayerZero_0 1
#define _LayerZero_1 0
#define _LayerZero_2 0
#define _LayerZero_3 0
#define _LayerZero_4 0
#define _LayerZero_5 0
#define _LayerZero_6 0
#define _LayerZero_7 0
	// Support at most 8 layers.
#define _LayerCalculate_1_1 0
#define _LayerCalculate_2_1 1
#define _LayerCalculate_3_1 2
#define _LayerCalculate_4_1 3
#define _LayerCalculate_5_1 4
#define _LayerCalculate_6_1 5
#define _LayerCalculate_7_1 6
#define _LayerCalculate_8_1 7
#define _ZeroResolverAUX(value) _LayerZero_##value
#define _ZeroResolver(value) _ZeroResolverAUX(value)
#define _IsZero(value)	_ZeroResolver(value)
#define _Decrease1(x) _LayerCalculate_##x##_1
#define _If0(output1,output2) output2
#define _If1(output1,output2) output1
#define _IfConditionAUX(condition) _If##condition
#define _IfCondition(condition) _IfConditionAUX(condition)
#define _If(condition,output1,output2) _IfCondition(condition)(output1,output2)

#define _empty()
#define _defer(...) __VA_ARGS__ _empty()
#define _obstruct(...) __VA_ARGS__ _defer(_empty)()
#define _eval(...)  _eval1(_eval1(_eval1(__VA_ARGS__)))
#define _eval1(...) _eval2(_eval2(_eval2(__VA_ARGS__)))
#define _eval2(...) __VA_ARGS__

#define _GetBeginFrom(n,first,...) _If(_IsZero(n),first,_obstruct(_GetBeginFromIndirect)()(_Decrease1(n),##__VA_ARGS__))
#define _GetBeginFromIndirect() _GetBeginFrom
#define _GetFirst(first,...) first
#define _GetNthElement(n,...) _GetFirst(_GetBeginFrom(n,##__VA_ARGS__))

#define _ConstructGetUnderlyingType(TotalLayer,StartLayer,T)											\
	_GetUnderlyingType(StartLayer-TotalLayer+1,_If(_IsZero(_Decrease1(TotalLayer)),T,_obstruct(_ConstructGetUnderlyingTypeIndirect)()(_Decrease1(TotalLayer),StartLayer,T)))
#define _ConstructGetUnderlyingTypeIndirect() _ConstructGetUnderlyingType

#define _ConstructGetUnderlyingTypeWithPosition(TotalLayer,StartLayer,T,...)							\
	_GetUnderlyingTypeWithPosition(StartLayer-TotalLayer+1,_If(_IsZero(_Decrease1(TotalLayer)),T,_obstruct(_ConstructGetUnderlyingTypeWithPositionIndirect)()(_Decrease1(TotalLayer),StartLayer,T,##__VA_ARGS__)),_GetNthElement(_Decrease1(TotalLayer),##__VA_ARGS__))
#define _ConstructGetUnderlyingTypeWithPositionIndirect() _ConstructGetUnderlyingTypeWithPosition


	/// @notice For single dimension type you can directly use this. Like int, std::thread.
	/// @param Name is name of either concept for newer version or constant expression for older version which you defined.
	/// @param ... is a type list to constraint type.
#define ConstructBasicEligibleType(Name,...)															\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<T,##__VA_ARGS__>;

	/// @notice For multi-dimension type you need to AddTypeLayer first then ConstructEligibleType,
	/// they are methods without specify the index of this type.
	/// If you look for methods which can specify index then go look WithPosition version.
	/// @param LayerNumber is which layer you want to add.
	/// @param Type is which type add to the layer.
#define AddTypeLayer(LayerNumber,Type)																	\
	template<typename T>																				\
	struct _IsEligibleType<Type<T>,LayerNumber,default_index> :std::true_type							\
	{																									\
		using underlying_type = T;																		\
	};

	/// @notice For multi-dimension type you need to AddTypeLayer first then ConstructEligibleType,
	/// they are methods without specify the index of this type.
	/// If you look for methods which can specify index then go look WithPosition version.
	/// @param Name is name of either concept for newer version or constant expression for older version which you defined.
	/// @param TotalLayer is total layer you want to use.
	/// @param StartLayer is start for which layer, you should add type to the correspond layer first.
	/// e.g If TotalLayer is 2 and StartLayer is 10, then it will use 10,9. So you should call AddTypeLayer first.
	/// @param ... this rest of parameters are list of basic type, which equivalent to ConstructBasicEligibleType.
	/// 
	/// e.g If you want to constraint type to std::vector<int> then TotalLayer should be 1, StartLayer should
	/// be the same layer you called AddTypeLayer with Type std::vector and the rest of parameter is int.
#define ConstructEligibleType(Name,TotalLayer,StartLayer,...)											\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_eval(_ConstructGetUnderlyingType(TotalLayer,StartLayer,T)),##__VA_ARGS__>;


	/// @notice For multi-dimension type you need to AddTypeLayerWithPosition first then ConstructEligibleTypeWithPosition.
	/// @param LayerNumber is which layer you want to add.
	/// @param TypeIndex is the index you specified for the Type. The AddTypeLayer's default TypeIndex is 0
	/// which means they always constraint the first parameter in template parameter list.
	/// e.g You want to constraint T2 in A<T1,T2> you can't do that without WithPosition version. 
	/// @param Type is which type add to the layer.
#define AddTypeLayerWithPosition(LayerNumber,TypeIndex,Type)											\
	template<typename... Rest>																			\
	struct _IsEligibleType<Type<Rest...>,LayerNumber,TypeIndex> :std::true_type							\
	{																									\
		using underlying_type = std::tuple_element_t<TypeIndex,std::tuple<Rest...>>;					\
	};

	/// @notice For multi-dimension type you need to AddTypeLayerWithPosition first then ConstructEligibleTypeWithPosition.
	/// @param Name is name of either concept for newer version or constant expression for older version which you defined.
	/// @param TotalLayer is total layer you want to use.
	/// @param StartLayer is start for which layer, you should add type to the correspond layer first.
	/// e.g If TotalLayer is 2 and StartLayer is 10, then it will use 10,9. So you should call AddTypeLayerWithPosition first.
	/// @param ... the first n element are TypeIndex, n equal to the TotalLayer.
	/// TypeIndex is the index you specified for the Type. The ConstructEligibleType's default TypeIndex is 0
	/// which means they always constraint the first parameter in template parameter list.
	/// e.g You want to constraint T2 in A<T1,T2> you can't do that without WithPosition version. 
	/// The rest of parameters are list of basic type, which equivalent to ConstructBasicEligibleType.
	/// 
	/// e.g If you want to constraint type to A<?,int> then TotalLayer should be 1,
	/// first element of ... is the same you specified in AddTypeLayerWithPosition's TypeIndex equals 1,
	/// StartLayer should be the same layer you called AddTypeLayerWithPosition with Type A and the rest of parameter is int.
#define ConstructEligibleTypeWithPosition(Name,TotalLayer,StartLayer,...)								\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_eval(_ConstructGetUnderlyingTypeWithPosition(TotalLayer,StartLayer,T,##__VA_ARGS__)),_eval(_GetBeginFrom(TotalLayer,##__VA_ARGS__))>;
}