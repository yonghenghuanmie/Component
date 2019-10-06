#include <cmath>
#include <tuple>
#include <typeinfo>
#include <iostream>
#include <functional>
#include <type_traits>

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

	// +
	template<class T1, class T2, bool IsArithmetic1 = std::is_arithmetic_v<T1>, bool IsArithmetic2 = std::is_arithmetic_v<T2>,
		class Enable = std::enable_if_t<
		(std::is_arithmetic_v<T1> || std::is_invocable_v<T1>) &&
		(std::is_arithmetic_v<T2> || std::is_invocable_v<T2>) >>
		auto operator+(const Value<T1> & value1, const T2 & value2)
	{
		if constexpr (IsArithmetic1)
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return static_cast<T1>(value1) + value2; });
			}
			else
			{
				return Value([&]() {return static_cast<T1>(value1) + value2(); });
			}
		}
		else
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return value1() + static_cast<T2>(value2); });
			}
			else
			{
				return Value([&]() {return value1() + value2(); });
			}
		}
	}

	// -
	template<class T1, class T2, bool IsArithmetic1 = std::is_arithmetic_v<T1>, bool IsArithmetic2 = std::is_arithmetic_v<T2>,
		class Enable = std::enable_if_t<
		(std::is_arithmetic_v<T1> || std::is_invocable_v<T1>) &&
		(std::is_arithmetic_v<T2> || std::is_invocable_v<T2>) >>
		auto operator-(const Value<T1> & value1, const T2 & value2)
	{
		if constexpr (IsArithmetic1)
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return static_cast<T1>(value1) - value2; });
			}
			else
			{
				return Value([&]() {return static_cast<T1>(value1) - value2(); });
			}
		}
		else
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return value1() - static_cast<T2>(value2); });
			}
			else
			{
				return Value([&]() {return value1() - value2(); });
			}
		}
	}

	// *
	template<class T1, class T2, bool IsArithmetic1 = std::is_arithmetic_v<T1>, bool IsArithmetic2 = std::is_arithmetic_v<T2>,
		class Enable = std::enable_if_t<
		(std::is_arithmetic_v<T1> || std::is_invocable_v<T1>) &&
		(std::is_arithmetic_v<T2> || std::is_invocable_v<T2>) >>
		auto operator*(const Value<T1> & value1, const T2 & value2)
	{
		if constexpr (IsArithmetic1)
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return static_cast<T1>(value1) * value2; });
			}
			else
			{
				return Value([&]() {return value1 == 0 ? 0 : static_cast<T1>(value1) * value2(); });
			}
		}
		else
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return value2 == 0 ? 0 : value1() * static_cast<T2>(value2); });
			}
			else
			{
				return Value([&]() {return value1() * value2(); });
			}
		}
	}

	// /
	template<class T1, class T2, bool IsArithmetic1 = std::is_arithmetic_v<T1>, bool IsArithmetic2 = std::is_arithmetic_v<T2>,
		class Enable = std::enable_if_t<
		(std::is_arithmetic_v<T1> || std::is_invocable_v<T1>) &&
		(std::is_arithmetic_v<T2> || std::is_invocable_v<T2>) >>
		auto operator/(const Value<T1> & value1, const T2 & value2)
	{
		if constexpr (IsArithmetic1)
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return static_cast<T1>(value1) / value2; });
			}
			else
			{
				return Value([&]() {return value1 == 0 ? 0 : static_cast<T1>(value1) / value2(); });
			}
		}
		else
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return value1() / static_cast<T2>(value2); });
			}
			else
			{
				return Value([&]() {return value1() / value2(); });
			}
		}
	}

	template<class T1, class T2, bool IsArithmetic1 = std::is_arithmetic_v<T1>, bool IsArithmetic2 = std::is_arithmetic_v<T2>,
		class Enable = std::enable_if_t<
		(std::is_arithmetic_v<T1> || std::is_invocable_v<T1>) &&
		(std::is_arithmetic_v<T2> || std::is_invocable_v<T2>) >>
		auto pow(const T1 & base, const T2 & index)
	{
		if constexpr (IsArithmetic1)
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return std::pow(base, index); });
			}
			else
			{
				return Value([&]() {return base == 1 ? 1 : std::pow(base, index());; });
			}
		}
		else
		{
			if constexpr (IsArithmetic2)
			{
				return Value([&]() {return index == 0 ? 1 : std::pow(base(), index); });
			}
			else
			{
				return Value([&]() {return std::pow(base(), index()); });
			}
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
	auto func_y2 = (lazy::Value(1.0) + 10) * 20 / 2 - []() {return 4 + 8; }-lazy::pow(4, 2);
	auto y1 = func_y1();
	auto y2 = func_y2();
	return 0;
}