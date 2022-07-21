#pragma once

template<typename Vertex, typename Weight = int>
class Graph
{
public:
	using PathType = std::unordered_map<Vertex, Weight>::value_type;
	using ResultsType = std::vector<std::reference_wrapper<const PathType>>;
	using ComparatorType = bool(*)(const Weight&, const Weight&);
	using BestPathType = ResultsType(const Graph<Vertex, Weight>*, const Vertex&, const Vertex&, ComparatorType);

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

	Graph(const Weight& Infinite) { INFINITE = Infinite; }

	Graph(const Weight& Infinite, auto&&... rests)
	{
		INFINITE = Infinite;
		Construct(std::forward<decltype(rests)>(rests)...);
	}

	Graph(const Weight& Infinite, std::initializer_list<std::tuple<Vertex, Weight, Vertex>> list)
	{
		INFINITE = Infinite;
		for (auto&& element : list)
			AddEdge(std::get<0>(element), std::get<2>(element)) = std::get<1>(element);
	}

	Weight& AddEdge(const Vertex& begin, const Vertex& end)
	{
		return matrix[begin][end];
	}

	Weight& AddEdge(Vertex&& begin, Vertex&& end)
	{
		return matrix[std::move(begin)][std::move(end)];
	}

	void RemoveEdge(const Vertex& begin, const Vertex& end)
	{
		matrix[begin][end] = INFINITE;
	}

	ResultsType GetOutPath(const Vertex& vertex) const
	{
		ResultsType results;
		if (matrix.contains(vertex))
			for (auto&& element : matrix.at(vertex))
				if (element.second != INFINITE)
					results.emplace_back(element);
		return results;
	}

	void GetOutPath(const Vertex& vertex, std::invocable<const PathType&> auto&& callback) const
		requires std::is_same_v<std::invoke_result_t<decltype(callback), const PathType&>, bool>
	{
		if (matrix.contains(vertex))
			for (auto&& element : matrix.at(vertex))
				if (element.second != INFINITE)
					if (!callback(element))
						break;
	}

	ResultsType GetInPath(const Vertex& vertex) const
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

	void GetInPath(const Vertex& vertex, std::invocable<const PathType&> auto&& callback) const
		requires std::is_same_v<std::invoke_result_t<decltype(callback), const PathType&>, bool>
	{
		for (auto iterator = matrix.begin(); iterator != matrix.end(); ++iterator)
			if (iterator->second.contains(vertex) && iterator->second.at(vertex) != INFINITE)
				if (!callback(PathType{ iterator->first, iterator->second.at(vertex) }))
					break;
	}

	void SetBestPathAlgorithm(std::invocable<const Graph<Vertex, Weight>*, const Vertex&, const Vertex&, ComparatorType> auto&& function)
	{
		best_path = std::forward<decltype(function)>(function);
	}

	ResultsType GetBestPath(const Vertex& begin, const Vertex& end, ComparatorType comparator = [](const Weight& first, const Weight& second) {return first < second; }) const
	{
		if (best_path)
			return best_path(this, begin, end, comparator);
		else
		{
			std::vector<ResultsType> results;
			cache[begin] = Weight{};
			IterateGraph(results, end, {}, *cache.find(begin), {});
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
			return {};
		}
	}

	auto CacheSize() { return cache.size(); }
	void ClearCache() { cache.clear(); }

private:
	void Construct(auto&& vertex1, auto&& weight, auto&& vertex2, auto&&... rests)
	{
		AddEdge(std::forward<decltype(vertex1)>(vertex1), std::forward<decltype(vertex2)>(vertex2)) = std::forward<decltype(weight)>(weight);
		Construct(std::forward<decltype(rests)>(rests)...);
	}

	void Construct(auto&& vertex1, auto&& weight, auto&& vertex2)
	{
		AddEdge(std::forward<decltype(vertex1)>(vertex1), std::forward<decltype(vertex2)>(vertex2)) = std::forward<decltype(weight)>(weight);
	}

	void IterateGraph(std::vector<ResultsType>& results, const Vertex& target, ResultsType current_path, const PathType& new_path, std::unordered_set<Vertex> set) const
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
			IterateGraph(results, target, current_path, *matrix.at(new_path.first).find(vertex), set);
		}
	}
};

template<typename Vertex, typename Weight>
inline Weight Graph<Vertex, Weight>::INFINITE;