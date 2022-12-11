#pragma once

namespace TypeCast
{
	namespace Detail
	{
		template<typename Ret, typename This>
		void InstanceOf(Ret&& result, This&& value) {}

		template<typename First, typename... Rest, typename Ret, typename This>
		void InstanceOf(Ret&& result, This&& value)
		{
			auto pointer = dynamic_cast<First>(value);
			if (pointer)
				result.template emplace<First>(pointer);
			else
				InstanceOf<Rest...>(result, value);
		}

		template<typename... Rest>
		struct AddTupleType {};

		template<typename... Rest, typename Type>
		struct AddTupleType<std::tuple<Rest...>, Type>
		{
			using type = std::tuple<Rest..., Type>;
		};

		template<int begin1, int begin2, typename... Rest>
		auto GetLeafLayer2(const std::tuple<Rest...>& tuple);

		template<int begin, typename... Rest, typename T>
		auto GetLeafLayer1(T&& result)
		{
			if constexpr (begin == sizeof...(Rest))
				return result;
			else if constexpr (std::is_same_v<decltype(GetLeafLayer2<begin, 0>(std::tuple<Rest...>{})), void > )
				return GetLeafLayer1<begin + 1, Rest...>(result);
			else
			{
				typename AddTupleType<std::remove_reference_t<decltype(result)>, std::tuple_element_t<begin, std::tuple<Rest...>>>::type new_result;
				return GetLeafLayer1<begin + 1, Rest...>(new_result);
			}
		}

		template<int begin1, int begin2, typename... Rest>
		auto GetLeafLayer2(const std::tuple<Rest...>& tuple)
		{
			if constexpr (begin2 == sizeof...(Rest))
				return std::tuple_element_t<begin1, std::tuple<Rest...>>{};
			else if constexpr (begin1 != begin2 && std::is_base_of_v<
				std::remove_pointer_t<std::tuple_element_t<begin1, std::tuple<Rest...>>>,
				std::remove_pointer_t<std::tuple_element_t<begin2, std::tuple<Rest...>>>
			>)
				return;
			else
				return GetLeafLayer2<begin1, begin2 + 1>(tuple);
		}
	}

	template<typename... Rest, typename This>
		requires std::is_pointer_v<This> && (std::is_pointer_v<Rest>&&...)
	std::variant<This, Rest...> InstanceOf(This&& value)
	{
		std::variant<This, Rest...> result(value);
		Detail::InstanceOf<Rest...>(result, value);
		return result;
	}

	template<typename... Rest>
		requires (std::is_pointer_v<Rest>&&...)
	auto GetLeaf()
	{
		return Detail::GetLeafLayer1<0, Rest...>(std::tuple<>());
	}

	template<typename... Rest, typename This>
	std::variant<This, Rest...> InstanceOf(This&& value, const std::tuple<Rest...>& leaf)
	{
		return InstanceOf<Rest...>(std::remove_reference_t<This>(value));
	}
}