#pragma once

class elapse
{
public:
	elapse()
	{
		begin = std::chrono::system_clock().now();
	}

	elapse(std::function<void(std::chrono::system_clock::duration)> function) :elapse()
	{
		this->function = std::move(function);
	}

	void restart() noexcept
	{
		begin = std::chrono::system_clock().now();
	}

	~elapse()
	{
		if (function)
			function(std::chrono::system_clock().now() - begin);
		else
			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock().now() - begin).count() << " milliseconds has elapsed." << std::endl;
	}

private:
	std::chrono::system_clock::time_point begin;
	std::function<void(std::chrono::system_clock::duration)> function;
};
