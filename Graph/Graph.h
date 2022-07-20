#pragma once

template<typename Vertex, typename Weight = int>
class Graph
{
	using PathType = std::unordered_map<Vertex, Weight>::value_type;

	template<typename T>
	struct Allocator :public std::allocator<T>
	{
		void construct(std::same_as<PathType> auto* p, auto&& tag, auto&& key_tuple, auto&& value_tuple)
		{
			assert(std::tuple_size_v<std::remove_reference_t<decltype(value_tuple)>> == 0);
			::new((void*)p) PathType(std::forward<decltype(tag)>(tag),
				std::forward<decltype(key_tuple)>(key_tuple),
				std::forward_as_tuple(Graph<Vertex, Weight>::INFINITE));
		}

		template<class U>
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

	using Compare = bool(*)(const Weight&, const Weight&);
	std::function<std::vector<PathType>(const Vertex&, const Vertex&, Compare)> best_path;

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

	auto GetOutPath(const Vertex& vertex)
	{
		std::vector<PathType> results;
		for (auto&& element : matrix[vertex])
			if (element.second != INFINITE)
				results.push_back(element);
		return results;
	}

	void GetOutPath(const Vertex& vertex, std::invocable<PathType> auto&& callback)
		requires std::is_same_v<std::invoke_result_t<decltype(callback), PathType>, bool>
	{
		for (auto&& element : matrix[vertex])
			if (element.second != INFINITE)
				if (!callback(element))
					break;
	}

	auto GetInPath(const Vertex& vertex)
	{
		std::vector<PathType> results;
		for (auto iterator = matrix.begin(); iterator != matrix.end(); ++iterator)
			if (iterator->second.contains(vertex) && iterator->second[vertex] != INFINITE)
				results.push_back({ iterator->first, iterator->second[vertex] });
		return results;
	}

	void GetInPath(const Vertex& vertex, std::invocable<PathType> auto&& callback)
		requires std::is_same_v<std::invoke_result_t<decltype(callback), PathType>, bool>
	{
		for (auto iterator = matrix.begin(); iterator != matrix.end(); ++iterator)
			if (iterator->second.contains(vertex) && iterator->second[vertex] != INFINITE)
				if (!callback(PathType{ iterator->first, iterator->second[vertex] }))
					break;
	}

	void SetBestPathAlgorithm(std::invocable<const Vertex&, const Vertex&, Compare> auto&& function)
	{
		best_path = std::forward<decltype(function)>(function);
	}

	std::vector<PathType> GetBestPath(const Vertex& begin, const Vertex& end, Compare comparator = [](const Weight& first, const Weight& second) {return first < second; })
	{
		if (best_path)
			return best_path(begin, end, comparator);
		else
		{
			std::vector<std::vector<PathType>> results;
			IterateGraph(results, end, {}, { begin, Weight{} }, {});
			if (!results.empty())
			{
				std::vector<std::pair<Weight, std::size_t>> sort_weight;
				for (size_t i = 0; i < results.size(); ++i)
					sort_weight.emplace_back(
						std::accumulate(results[i].begin(), results[i].end(), Weight{}, [](auto&& acc, auto&& pair) {return acc + pair.second; }),
						i);
				std::sort(std::execution::par_unseq, sort_weight.begin(), sort_weight.end(), [comparator](auto&& first, auto&& second) {return comparator(first.first, second.first); });
				auto& result = results[sort_weight[0].second];
				result[0].second = sort_weight[0].first;
				return result;
			}
			return {};
		}
	}

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

	void IterateGraph(std::vector<std::vector<PathType>>& results, const Vertex& target, std::vector<PathType> current_path, PathType new_path, std::unordered_set<Vertex> set)
	{
		set.insert(new_path.first);
		current_path.push_back(new_path);

		if (new_path.first == target)
		{
			results.push_back(std::move(current_path));
			return;
		}

		for (auto [vertex, weight] : matrix[new_path.first])
		{
			if (set.contains(vertex) || weight == INFINITE)
				continue;
			IterateGraph(results, target, current_path, { vertex, weight }, set);
		}
	}
};

template<typename Vertex, typename Weight>
inline Weight Graph<Vertex, Weight>::INFINITE;