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
				result.emplace<First>(pointer);
			else
				InstanceOf<Rest...>(result, value);
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
}