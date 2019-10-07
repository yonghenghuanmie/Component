#include <cmath>
#include <typeinfo>
#include <iostream>
#include <functional>
#include <type_traits>
#include <Eigen/Dense>

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
				return Value([&] {return value1 == 0 ? 0 : static_cast<T1>(value1)* value2(); });
			}
			else
			{
				return Value([&] {return static_cast<T1>(value1)* value2; });
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
				return Value([&] {return base == 1 ? 1 : std::pow(base, index());; });
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
			return value == 1 ? 0 : [&] {return std::log(value); };
		}
		else
		{
			return [&] {return std::log(value()); };
		}
	}
}

int main()
{
	/*auto y00 = (lazy::Value(10) * 10)();
	auto y01 = (lazy::Value(10) * []() {return 10; })();
	auto y10 = (lazy::Value([]() {return 10; }) * 10)();
	auto y11 = (lazy::Value([]() {return 10; }) * []() {return 10; })();*/
	auto func_y1 = lazy::pow(1, lazy::pow(10, 2) * 5) * 10;
	auto func_y2 = (lazy::Value(1.0) + 10) * 20 / lazy::log(2) - []() {return 4 + 8; }-lazy::pow(4, 2);
	auto y1 = func_y1();
	auto y2 = func_y2();
	return 0;
}