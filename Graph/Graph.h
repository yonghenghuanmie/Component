#pragma once

template<typename Vertex, typename Weight = int>
	requires requires {Weight{} != Weight{}; }
class Graph
{
public:
	using PathType = std::unordered_map<Vertex, Weight>::value_type;
	using ResultsType = std::vector<std::reference_wrapper<const PathType>>;
	using BestPathType = std::variant<std::string, ResultsType>(const Graph<Vertex, Weight>*, const Vertex&, const Vertex&, std::function<bool(const Weight&, const Weight&)>&&);

private:
	template<typename T>
	struct Allocator :public std::allocator<T>
	{
		void construct(std::same_as<PathType> auto* p, auto&& tag, auto&& key_tuple, auto&& value_tuple)
		{
			assert(std::tuple_size_v<std::remove_reference_t<decltype(value_tuple)>> == 0);
			::new(p) PathType(std::forward<decltype(tag)>(tag),
				std::forward<decltype(key_tuple)>(key_tuple),
				std::forward_as_tuple(Graph<Vertex, Weight>::INFINITE));
		}

		template<typename U>
		void destroy(U* p)
		{
			p->~U();
		}

		template<typename U>
		struct rebind
		{
			using type = Allocator<U>;
		};
	};

	using AdjacencyMatrix = std::unordered_map<Vertex, std::unordered_map<Vertex, Weight, std::hash<Vertex>, std::equal_to<Vertex>, Allocator<PathType>>>;
	AdjacencyMatrix matrix;
	mutable std::unordered_map<Vertex, Weight> cache;
	std::function<BestPathType> best_path;

public:
	static Weight INFINITE;

	Graph(const Weight& Infinite) noexcept { INFINITE = Infinite; }

	Graph(const Weight& Infinite, auto&&... rests) noexcept
	{
		INFINITE = Infinite;
		Construct(std::forward<decltype(rests)>(rests)...);
	}

	Graph(const Weight& Infinite, std::initializer_list<std::tuple<Vertex, Weight, Vertex>> list) noexcept
	{
		INFINITE = Infinite;
		for (auto&& element : list)
			AddEdge(std::get<0>(element), std::get<2>(element)) = std::get<1>(element);
	}

	Weight& AddEdge(const Vertex& begin, const Vertex& end) noexcept
	{
		return matrix[begin][end];
	}

	Weight& AddEdge(Vertex&& begin, Vertex&& end) noexcept
	{
		return matrix[std::move(begin)][std::move(end)];
	}

	void RemoveEdge(const Vertex& begin, const Vertex& end) noexcept
	{
		matrix[begin][end] = INFINITE;
	}

	ResultsType GetOutPath(const Vertex& vertex) const noexcept
	{
		ResultsType results;
		if (matrix.contains(vertex))
			for (auto&& element : matrix.at(vertex))
				if (element.second != INFINITE)
					results.emplace_back(element);
		return results;
	}

	void GetOutPath(const Vertex& vertex, std::invocable<const PathType&> auto&& callback) const noexcept
		requires std::is_same_v<std::invoke_result_t<decltype(callback), const PathType&>, bool>
	{
		if (matrix.contains(vertex))
			for (auto&& element : matrix.at(vertex))
				if (element.second != INFINITE)
					if (!callback(element))
						break;
	}

	ResultsType GetInPath(const Vertex& vertex) const noexcept
	{
		ResultsType results;
		for (auto iterator = matrix.begin(); iterator != matrix.end(); ++iterator)
			if (iterator->second.contains(vertex) && iterator->second.at(vertex) != INFINITE)
			{
				cache[iterator->first] = iterator->second.at(vertex);
				results.emplace_back(*cache.find(iterator->first));
			}
		return results;
	}

	void GetInPath(const Vertex& vertex, std::invocable<const PathType&> auto&& callback) const noexcept
		requires std::is_same_v<std::invoke_result_t<decltype(callback), const PathType&>, bool>
	{
		for (auto iterator = matrix.begin(); iterator != matrix.end(); ++iterator)
			if (iterator->second.contains(vertex) && iterator->second.at(vertex) != INFINITE)
				if (!callback(PathType{ iterator->first, iterator->second.at(vertex) }))
					break;
	}

	void SetBestPathAlgorithm(std::invocable<const Graph<Vertex, Weight>*, const Vertex&, const Vertex&, std::function<bool(const Weight&, const Weight&)>&&> auto&& function) noexcept
	{
		best_path = std::forward<decltype(function)>(function);
	}

	// Do not pass std::function to comparator
	template<std::invocable<const Weight&, const Weight&> T = std::less<void>>
		requires (std::is_pointer_v<T> || (std::is_object_v<T> && std::is_empty_v<T>)) && std::is_same_v<std::invoke_result_t<T, const Weight&, const Weight&>, bool>
	std::variant<std::string, ResultsType> GetBestPath(const Vertex& begin, const Vertex& end, T comparator = T{}) const
	{
		if (best_path)
			return best_path(this, begin, end, std::function<bool(const Weight&, const Weight&)>{ comparator });
		else if constexpr (requires (Weight a, Weight b) { a < b; a -= b; })
		{
			if (Weight{} + Weight{} != Weight{})
				return "Weight{} + Weight{} must equal to Weight{}";
			if (begin == end)
				return "begin should not equivalent to end.";
			if (comparator(Weight{}, INFINITE))
			{
				cache[begin] = Weight{};
				std::vector<ResultsType> results;
				std::vector<std::unordered_set<Vertex>> sets;
				std::vector<Weight> left_weight;
				for (auto&& path : matrix.at(begin))
				{
					results.emplace_back(ResultsType{ *cache.find(begin), path });
					sets.emplace_back(std::unordered_set{ begin, path.first });
					left_weight.emplace_back(path.second);
				}
				auto best_result = IterateGraphBFS(results, end, sets, left_weight);
				if (best_result.size() > 1)
					cache[begin] = std::accumulate(best_result.begin(), best_result.end(), Weight{}, [](auto&& acc, auto&& pair) {return acc + pair.get().second; });
				return best_result;
			}
			else
			{
				std::vector<ResultsType> results;
				cache[begin] = Weight{};
				IterateGraphDFS(results, end, {}, *cache.find(begin), {});
				if (!results.empty())
				{
					std::vector<std::pair<Weight, std::size_t>> sort_weight;
					for (size_t i = 0; i < results.size(); ++i)
						sort_weight.emplace_back(std::accumulate(results[i].begin(), results[i].end(), Weight{}, [](auto&& acc, auto&& pair) {return acc + pair.get().second; }), i);
					std::sort(std::execution::par_unseq, sort_weight.begin(), sort_weight.end(), [comparator](auto&& first, auto&& second) {return comparator(first.first, second.first); });
					auto& best_result = results[sort_weight[0].second];
					cache[begin] = sort_weight[0].first;
					return best_result;
				}
				return ResultsType{};
			}
		}
		else return "Weight must have comparative and arithmetic functionality or you can use SetBestPathAlgorithm to set your own algorithm.";
	}

	auto CacheSize() noexcept { return cache.size(); }
	void ClearCache() noexcept { cache.clear(); }

private:
	void Construct(auto&& vertex1, auto&& weight, auto&& vertex2, auto&&... rests) noexcept
	{
		AddEdge(std::forward<decltype(vertex1)>(vertex1), std::forward<decltype(vertex2)>(vertex2)) = std::forward<decltype(weight)>(weight);
		Construct(std::forward<decltype(rests)>(rests)...);
	}

	void Construct(auto&& vertex1, auto&& weight, auto&& vertex2) noexcept
	{
		AddEdge(std::forward<decltype(vertex1)>(vertex1), std::forward<decltype(vertex2)>(vertex2)) = std::forward<decltype(weight)>(weight);
	}

	void IterateGraphDFS(std::vector<ResultsType>& results, const Vertex& target, ResultsType current_path, const PathType& new_path, std::unordered_set<Vertex> set) const
	{
		set.insert(new_path.first);
		current_path.push_back(new_path);

		if (new_path.first == target)
		{
			results.push_back(std::move(current_path));
			return;
		}

		for (auto&& [vertex, weight] : matrix.at(new_path.first))
		{
			if (set.contains(vertex) || weight == INFINITE)
				continue;
			IterateGraphDFS(results, target, current_path, *matrix.at(new_path.first).find(vertex), set);
		}
	}

	// All the weights must greater than Weight{}
	ResultsType IterateGraphBFS(std::vector<ResultsType>& results, const Vertex& target, std::vector<std::unordered_set<Vertex>>& sets, std::vector<Weight>& left_weight) const
	{
		assert(results.size() == sets.size());
		assert(results.size() == left_weight.size());
		Weight min = std::accumulate(results.begin(), results.end(), INFINITE,
			[](const Weight& acc, const ResultsType& path) {return acc < (path.back().get().second > Weight{} ? path.back().get().second : INFINITE) ? acc : path.back().get().second; });
		auto old_size = results.size();
		for (size_t i = 0; i < old_size; i++)
		{
			if (left_weight[i] > Weight{})
			{
				left_weight[i] -= min;
				auto old_path = results[i].back();
				if (left_weight[i] == Weight{} && matrix.contains(old_path.get().first))
				{
					bool first_time = true;
					auto index = i;
					auto result_copy = results[i];
					auto set_copy = sets[i];
					for (auto&& [vertex, weight] : matrix.at(old_path.get().first))
					{
						if (sets[i].contains(vertex) || weight == INFINITE)
							continue;
						switch (first_time)
						{
						case false:
							results.push_back(result_copy);
							sets.push_back(set_copy);
							left_weight.emplace_back();
							index = results.size() - 1;
						case true:
							first_time = false;
							results[index].push_back(*matrix.at(old_path.get().first).find(vertex));
							sets[index].insert(vertex);
							left_weight[index] = weight;
							if (vertex == target)
								return results[index];
							break;
						}
					}
				}
			}
		}
		if (auto iter = std::max_element(std::execution::par_unseq, left_weight.begin(), left_weight.end()); iter == left_weight.end() || *iter <= Weight{})
			return {};
		return IterateGraphBFS(results, target, sets, left_weight);
	}
};

template<typename Vertex, typename Weight>
	requires requires {Weight{} != Weight{}; }
inline Weight Graph<Vertex, Weight>::INFINITE;