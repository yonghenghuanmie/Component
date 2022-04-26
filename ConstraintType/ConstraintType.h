#pragma once

/*
*	@author huanmie<yonghenghuanmie@gmail.com>
*	@date 2022.4.26
*/

namespace ConstraintType
{
	template<typename T, typename... Rest>
	concept _EligibleUnderlyingType = (std::same_as<T, Rest> || ...);

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

#define AddTypeLayer(LayerNumber,Type)																	\
	template<typename T>																				\
	struct _IsEligibleType<Type<T>,LayerNumber,default_index> :std::true_type							\
	{																									\
		using underlying_type = T;																		\
	};

#define ConstructBasicEligibleType(Name,...)															\
	template<typename T>																				\
	concept Name = _EligibleUnderlyingType<T,##__VA_ARGS__>;

	//Please make sure StartLayer - layer number >=0
#define Construct1LayerEligibleType(Name,StartLayer,...)												\
	template<typename T>																				\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingType(StartLayer,T),##__VA_ARGS__>;

#define Construct2LayerEligibleType(Name,StartLayer,...)												\
	template<typename T>																				\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingType(StartLayer-1,_GetUnderlyingType(StartLayer,T)),##__VA_ARGS__>;

#define Construct3LayerEligibleType(Name,StartLayer,...)												\
	template<typename T>																				\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingType(StartLayer-2,_GetUnderlyingType(StartLayer-1,_GetUnderlyingType(StartLayer,T))),##__VA_ARGS__>;


#define AddTypeLayerWithPosition(LayerNumber,TypeIndex,Type)											\
	template<typename... Rest>																			\
	struct _IsEligibleType<Type<Rest...>,LayerNumber,TypeIndex> :std::true_type							\
	{																									\
		using underlying_type = std::tuple_element_t<TypeIndex,std::tuple<Rest...>>;					\
	};

#define Construct1LayerEligibleTypeWithPosition(Name,StartLayer,TypeIndex1,...)							\
	template<typename T>																				\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingTypeWithPosition(StartLayer,T,TypeIndex1),##__VA_ARGS__>;

#define Construct2LayerEligibleTypeWithPosition(Name,StartLayer,TypeIndex1,TypeIndex2,...)				\
	template<typename T>																				\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingTypeWithPosition(StartLayer-1,		\
	_GetUnderlyingTypeWithPosition(StartLayer,T,TypeIndex1),TypeIndex2),##__VA_ARGS__>;

#define Construct3LayerEligibleTypeWithPosition(Name,StartLayer,TypeIndex1,TypeIndex2,TypeIndex3,...)	\
	template<typename T>																				\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingTypeWithPosition(StartLayer-2,		\
	_GetUnderlyingTypeWithPosition(StartLayer-1,_GetUnderlyingTypeWithPosition(StartLayer,T,TypeIndex1),TypeIndex2),TypeIndex3),##__VA_ARGS__>;
}