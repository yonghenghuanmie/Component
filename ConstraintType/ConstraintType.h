#pragma once

/*
*	@author huanmie<yonghenghuanmie@gmail.com>
*	@date 2022.4.26
*/

namespace ConstraintType
{
	// check if concept available.
#define BackwardCompatibility
#if __cplusplus >= 202002L
#ifdef __cpp_concepts
#ifdef __cpp_lib_concepts

#undef BackwardCompatibility

#endif // __cpp_lib_concepts
#endif // __cpp_concepts
#endif // __cplusplus >= 202002L


#ifdef BackwardCompatibility

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
#endif // BackwardCompatibility


	using Layer = std::size_t;
	using Index = std::size_t;
	template<typename T, Layer layer, Index index>
	struct _IsEligibleType :std::false_type {};

	constexpr std::size_t default_index = 0;
#define _GetUnderlyingType(LayerNumber,CurrentType)														\
	std::enable_if_t<_IsEligibleType<CurrentType,LayerNumber,default_index>::value,						\
	typename _IsEligibleType<CurrentType,LayerNumber,default_index>::underlying_type>

#define _GetUnderlyingTypeWithPosition(LayerNumber,CurrentType,TypeIndex)								\
	std::enable_if_t<_IsEligibleType<CurrentType,LayerNumber,TypeIndex>::value,							\
	typename _IsEligibleType<CurrentType,LayerNumber,TypeIndex>::underlying_type>



	// Specialize _IsEligibleType
#define AddTypeLayer(LayerNumber,Type)																	\
	template<typename T>																				\
	struct _IsEligibleType<Type<T>,LayerNumber,default_index> :std::true_type							\
	{																									\
		using underlying_type = T;																		\
	};

#define ConstructBasicEligibleType(Name,...)															\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<T,##__VA_ARGS__>;

	// Please make sure StartLayer - layer number -1 >= 0
#define Construct1LayerEligibleType(Name,StartLayer,...)												\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_GetUnderlyingType(StartLayer,T),##__VA_ARGS__>;

#define Construct2LayerEligibleType(Name,StartLayer,...)												\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_GetUnderlyingType(StartLayer-1,_GetUnderlyingType(StartLayer,T)),##__VA_ARGS__>;

#define Construct3LayerEligibleType(Name,StartLayer,...)												\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_GetUnderlyingType(StartLayer-2,_GetUnderlyingType(StartLayer-1,_GetUnderlyingType(StartLayer,T))),##__VA_ARGS__>;



	// Specialize _IsEligibleType
#define AddTypeLayerWithPosition(LayerNumber,TypeIndex,Type)											\
	template<typename... Rest>																			\
	struct _IsEligibleType<Type<Rest...>,LayerNumber,TypeIndex> :std::true_type							\
	{																									\
		using underlying_type = std::tuple_element_t<TypeIndex,std::tuple<Rest...>>;					\
	};

	// Please make sure StartLayer - layer number -1 >= 0
#define Construct1LayerEligibleTypeWithPosition(Name,StartLayer,TypeIndex1,...)							\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_GetUnderlyingTypeWithPosition(StartLayer,T,TypeIndex1),##__VA_ARGS__>;

#define Construct2LayerEligibleTypeWithPosition(Name,StartLayer,TypeIndex1,TypeIndex2,...)				\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_GetUnderlyingTypeWithPosition(StartLayer-1,		\
	_GetUnderlyingTypeWithPosition(StartLayer,T,TypeIndex1),TypeIndex2),##__VA_ARGS__>;

#define Construct3LayerEligibleTypeWithPosition(Name,StartLayer,TypeIndex1,TypeIndex2,TypeIndex3,...)	\
	template<typename T>																				\
	Concept Name = _EligibleUnderlyingType<_GetUnderlyingTypeWithPosition(StartLayer-2,		\
	_GetUnderlyingTypeWithPosition(StartLayer-1,_GetUnderlyingTypeWithPosition(StartLayer,T,TypeIndex1),TypeIndex2),TypeIndex3),##__VA_ARGS__>;
}