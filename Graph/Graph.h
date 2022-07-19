#pragma once

template<typename Vertex, std::integral Weight = int>
class Graph
{
	using AdjacencyMatrix = std::unordered_map<Vertex, std::unordered_map<Vertex, Weight>>;
	using PathType = AdjacencyMatrix::mapped_type::value_type;
	AdjacencyMatrix matrix;

	using Compare = bool(*)(const Weight&, const Weight&);
	std::function<std::vector<PathType>(const Vertex&, const Vertex&, Compare)> best_path;

	constexpr static Weight INFINITE = std::numeric_limits<Weight>::max();

public:
	Graph() = default;

	Graph(auto&&... rests) { Construct(std::forward<decltype(rests)>(rests)...); }

	Graph(std::initializer_list<std::tuple<Vertex, Weight, Vertex>> list)
	{
		for (auto&& element : list)
			AddEdge(std::get<0>(element), std::get<1>(element), std::get<2>(element));
	}

	void AddEdge(const Vertex& begin, const Weight& weight, const Vertex& end)
	{
		matrix[begin][end] = weight;
	}

	void AddEdge(Vertex&& begin, Weight&& weight, Vertex&& end)
	{
		matrix[std::move(begin)][std::move(end)] = std::move(weight);
	}

	void RemoveEdge(const Vertex& begin, const Vertex& end)
	{
		matrix[begin][end] = INFINITE;
	}

	Weight& GetWeight(const Vertex& begin, const Vertex& end)
	{
		return matrix[begin][end];
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
	{
		for (auto&& element : matrix[vertex])
			if (element.second != INFINITE)
				callback(element);
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
	{
		for (auto iterator = matrix.begin(); iterator != matrix.end(); ++iterator)
			if (iterator->second.contains(vertex) && iterator->second[vertex] != INFINITE)
				callback({ iterator->first, iterator->second[vertex] });
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
			IterateGraph(results, end, {}, { begin, Weight{ 0 } }, {});
			if (!results.empty())
			{
				std::vector<std::pair<Weight, std::size_t>> sort_weight;
				for (size_t i = 0; i < results.size(); ++i)
					sort_weight.emplace_back(
						std::accumulate(results[i].begin(), results[i].end(), Weight{ 0 }, [](auto&& acc, auto&& pair) {return acc + pair.second; }),
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
		AddEdge(std::forward<decltype(vertex1)>(vertex1), std::forward<decltype(weight)>(weight), std::forward<decltype(vertex2)>(vertex2));
		Construct(std::forward<decltype(rests)>(rests)...);
	}

	void Construct(auto&& vertex1, auto&& weight, auto&& vertex2)
	{
		AddEdge(std::forward<decltype(vertex1)>(vertex1), std::forward<decltype(weight)>(weight), std::forward<decltype(vertex2)>(vertex2));
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