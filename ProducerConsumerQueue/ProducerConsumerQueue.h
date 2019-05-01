#pragma once
template<class Node>
class ProducerConsumerQueue final
{
public:
	template<class T, class Enable = std::enable_if_t<std::is_same_v<std::decay_t<T>, Node>>>
	void Push(T &&t)
	{
		std::unique_lock lock(mutex);
		queue.push_back(std::move(t));
		if (queue.size() == 1)
			condition_variable.notify_one();
	}

	Node Pop()
	{
		std::unique_lock lock(mutex);
		if (queue.empty())
			condition_variable.wait(lock,[this] {return !queue.empty(); });
		auto temp = std::move(queue.front());
		queue.pop_front();
		return temp;
	}

	std::optional<Node> TryPop()
	{
		std::unique_lock lock(mutex);
		if (queue.empty())
			return {};
		auto temp = std::move(queue.front());
		queue.pop_front();
		return temp;
	}
private:
	std::mutex mutex;
	std::deque<Node> queue;
	std::condition_variable condition_variable;
};