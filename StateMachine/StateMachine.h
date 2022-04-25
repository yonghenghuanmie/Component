#pragma once

template<class StateType, class Operation, class Derive>
class StateMachine
{
public:
	auto GetState() noexcept
	{
		return state;
	}

	bool PerformOperation(const Operation& operation)
	{
		if ((bool)state == false)
			return false;
		auto operation_list = relation.find(state.value());
		if (operation_list == relation.end())
			return false;
		auto pair = operation_list->second.find(operation);
		if (pair == operation_list->second.end())
			return false;

		Derive* derive = dynamic_cast<Derive*>(this);
		if (derive == nullptr)
			return false;
		next_state = pair->second;
		if (callable_map[*next_state](derive))
			state = next_state;
		next_state = std::nullopt;
		return true;
	}

	bool RepeatOperation()
	{
		if ((bool)state == false)
			return false;
		auto iterator = callable_map.find(state.value());
		if (iterator == callable_map.end())
			return false;
		Derive* derive = dynamic_cast<Derive*>(this);
		if (derive == nullptr)
			return false;

		next_state = state;
		callable_map[*next_state](derive);
		next_state = std::nullopt;
		return true;
	}

protected:
	using callable = std::function<bool(Derive*)>;

	StateMachine() = default;
	virtual ~StateMachine() {}

	template<class... Rest>
	void Register_Callable(const StateType& s, callable&& function, Rest&&... rest)
	{
		callable_map[s] = std::move(function);
		Register_Callable(rest...);
	}

	template<class... Rest>
	bool Register_Relation(const StateType& begin, const Operation& operation, const StateType& end, Rest&&... rest)
	{
		auto iterator = callable_map.find(begin);
		if (iterator == callable_map.end())
			return false;
		iterator = callable_map.find(end);
		if (iterator == callable_map.end())
			return false;

		if (Register_Relation(rest...) == false)
			return false;
		relation[begin].emplace(operation, end);
		return true;
	}

	bool InitialState(const StateType& s)
	{
		auto pair = callable_map.find(s);
		if (pair == callable_map.end())
			return false;
		Derive* derive = dynamic_cast<Derive*>(this);
		if (derive == nullptr)
			return false;
		state = s;
		callable_map[s](derive);
		return true;
	}

	auto GetNextState() noexcept
	{
		return next_state;
	}

private:
	void Register_Callable() {}
	bool Register_Relation() { return true; }

	std::optional<StateType> state;
	std::optional<StateType> next_state;
	std::unordered_map<StateType, callable> callable_map;
	std::unordered_map<StateType, std::unordered_map<Operation, StateType>> relation;
};