#pragma once

template<class StateType, class Operation, class Derive>
class StateMachine :public Graph<StateType, Operation>
{
public:
	StateType GetState() noexcept
	{
		return state;
	}

	bool PerformOperation(const Operation& operation)
	{
		if (derive == nullptr && (derive = dynamic_cast<Derive*>(this)) == nullptr)
			return false;

		auto paths = this->GetOutPath(state);
		for (auto&& [new_state, op] : paths)
			if (op == operation)
			{
				if (callback_map[new_state](derive, state, new_state))
				{
					state = new_state;
					return true;
				}
				else
					return false;
			}
		return false;
	}

protected:
	using Callable = std::function<bool(Derive*, StateType, StateType)>;

	StateMachine(const StateType& initial_state, const Operation& invalid, auto&&... rests)
		:Graph<StateType, Operation>(invalid, std::forward<decltype(rests)>(rests)...), state(initial_state) {}

	virtual ~StateMachine() {}

	void RegisterCallback(const StateType& state, Callable&& function, auto&&... rests)
	{
		callback_map[state] = std::move(function);
		RegisterCallback(rests...);
	}

private:
	void RegisterCallback(const StateType& state, Callable&& function)
	{
		callback_map[state] = std::move(function);
	}

	StateType state;
	Derive* derive = nullptr;
	std::unordered_map<StateType, Callable> callback_map;
};