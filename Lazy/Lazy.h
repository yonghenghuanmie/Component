#pragma once

namespace lazy
{
	template<class T>
	class Value
	{
	public:
		Value(const T& v) :value(v) {}

		template<class Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
		/*explicit*/ operator T() const { return value; }

		template<class Enable = std::enable_if_t<std::is_invocable_v<T>>>
		auto operator()() const { return value(); }

	private:
		T value;
	};

	template<typename T>
	concept LazyRequired = requires(T v)
	{
		std::is_arithmetic_v<T> || std::is_invocable_v<T>;
	};

	// +
	template<LazyRequired T1, LazyRequired T2, bool IsInvocable1 = std::is_invocable_v<T1>, bool IsInvocable2 = std::is_invocable_v<T2>>
	auto operator+(const Value<T1>& value1, const T2& value2)
	{
		if constexpr (IsInvocable1)
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return value1() + value2(); });
			}
			else
			{
				return Value([&] {return value1() + static_cast<T2>(value2); });
			}
		}
		else
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return static_cast<T1>(value1) + value2(); });
			}
			else
			{
				return Value([&] {return static_cast<T1>(value1) + value2; });
			}
		}
	}

	// -
	template<LazyRequired T1, LazyRequired T2, bool IsInvocable1 = std::is_invocable_v<T1>, bool IsInvocable2 = std::is_invocable_v<T2>>
	auto operator-(const Value<T1>& value1, const T2& value2)
	{
		if constexpr (IsInvocable1)
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return value1() - value2(); });
			}
			else
			{
				return Value([&] {return value1() - static_cast<T2>(value2); });
			}
		}
		else
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return static_cast<T1>(value1) - value2(); });
			}
			else
			{
				return Value([&] {return static_cast<T1>(value1) - value2; });
			}
		}
	}

	// *
	template<LazyRequired T1, LazyRequired T2, bool IsInvocable1 = std::is_invocable_v<T1>, bool IsInvocable2 = std::is_invocable_v<T2>>
	auto operator*(const Value<T1>& value1, const T2& value2)
	{
		if constexpr (IsInvocable1)
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return value1() * value2(); });
			}
			else
			{
				return Value([&] {return value2 == 0 ? 0 : value1() * static_cast<T2>(value2); });
			}
		}
		else
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return value1 == 0 ? 0 : static_cast<T1>(value1) * value2(); });
			}
			else
			{
				return Value([&] {return static_cast<T1>(value1) * value2; });
			}
		}
	}

	// /
	template<LazyRequired T1, LazyRequired T2, bool IsInvocable1 = std::is_invocable_v<T1>, bool IsInvocable2 = std::is_invocable_v<T2>>
	auto operator/(const Value<T1>& value1, const T2& value2)
	{
		if constexpr (IsInvocable1)
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return value1() / value2(); });
			}
			else
			{
				return Value([&] {return value1() / static_cast<T2>(value2); });
			}
		}
		else
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return value1 == 0 ? 0 : static_cast<T1>(value1) / value2(); });
			}
			else
			{
				return Value([&] {return static_cast<T1>(value1) / value2; });
			}
		}
	}

	template<LazyRequired T1, LazyRequired T2, bool IsInvocable1 = std::is_invocable_v<T1>, bool IsInvocable2 = std::is_invocable_v<T2>>
	auto pow(const T1& base, const T2& index)
	{
		if constexpr (IsInvocable1)
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return std::pow(base(), index()); });
			}
			else
			{
				return Value([&] {return index == 0 ? 1 : std::pow(base(), index); });
			}
		}
		else
		{
			if constexpr (IsInvocable2)
			{
				return Value([&] {return base == 1 ? 1 : std::pow(base, index()); });
			}
			else
			{
				return Value([&] {return std::pow(base, index); });
			}
		}
	}

	template<LazyRequired T, bool IsArithmetic = std::is_arithmetic_v<T>>
	auto log(const T& value)
	{
		if constexpr (IsArithmetic)
		{
			return Value([&] {return value == 1 ? 0 : std::log(value); });
		}
		else
		{
			return Value([&] {return std::log(value()); });
		}
	}
}
