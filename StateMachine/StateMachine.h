#pragma once

template<class StateType,class Derive>
class StateMachine
{
protected:
	using callable = std::function<void(Derive*)>;

	StateMachine() = default;
	virtual ~StateMachine() {}

	template<class... Rest>
	void Register_Callable(std::string name,callable function,Rest&&... rest)
	{
		callable_map[std::move(name)] = std::move(function);
		Register_Callable(rest...);
	}

	template<class... Rest>
	bool Register_Relation(const StateType &begin,const std::string &name,const StateType &end,Rest&&... rest)
	{
		auto iterator = callable_map.find(name);
		if (iterator == callable_map.end())
			return false;
		if (Register_Relation(rest...) == false)
			return false;
		relation[begin].emplace(iterator->first, end);
		return true;
	}

	void InitialState(const StateType &s)
	{
		state = s;
	}

	auto GetState() noexcept
	{
		return state;
	}

	bool PerformOperation(const std::string &operation)
	{
		if (state == false)
			return false;
		auto record = relation.find(state);
		if (record == relation.end())
			return false;
		auto pair = record->second.find(operation);
		if (pair == record->second.end())
			return false;
		Derive *derive = dynamic_cast<Derive>(this);
		if (derive == nullptr)
			return false;
		callable_set[operation](derive);
		state = pair->second;
		return true;
	}

private:
	void Register_Callable(){}
	bool Register_Relation() { return true; }

	std::optional<StateType> state;
	std::unordered_map<std::string, callable> callable_map;
	std::unordered_map<StateType, std::unordered_map<std::string_view, StateType>> relation;
};