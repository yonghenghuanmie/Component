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
	template<typename T, Layer layer>
	struct _IsEligibleType :std::false_type {};

#define AddTypeLayer(LayerNumber,Type)																\
	template<typename T>																			\
	struct _IsEligibleType<Type<T>,LayerNumber> :std::true_type										\
	{																								\
		using underlying_type = T;																	\
	};

#define _GetUnderlyingType(LayerNumber,CurrentType)													\
	std::enable_if_t<_IsEligibleType<CurrentType,LayerNumber>::value, typename _IsEligibleType<CurrentType,LayerNumber>::underlying_type>

#define ConstructBasicEligibleType(Name,...)														\
	template<typename T>																			\
	concept Name = _EligibleUnderlyingType<T,##__VA_ARGS__>;

#define Construct1LayerEligibleType(Name,...)														\
	template<typename T>																			\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingType(1,T),##__VA_ARGS__>;

#define Construct2LayerEligibleType(Name,...)														\
	template<typename T>																			\
	concept Name = _EligibleUnderlyingType<_GetUnderlyingType(1,_GetUnderlyingType(2,T)),##__VA_ARGS__>;
}