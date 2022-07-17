#pragma once

using EdgeWeight = int;

template<typename Vertex>
using AdjacencyMatrix = std::unordered_map<Vertex, std::unordered_map<Vertex, EdgeWeight>>;

template<typename Vertex, typename Edge = AdjacencyMatrix<Vertex>>
class Graph
{
public:
	Graph() = default;

	template<typename... Rests>
	Graph(Rests...&& rests) { Construct(std::forward<Rests>(rests)...); }

	void AddEdge(Vertex&& first, EdgeWeight weight, Vertex&& second)
	{
		edges[first][second] = weight;
	}

	void RemoveEdge(Vertex&& first, Vertex&& second)
	{
		edges[first][second] = INFINITE;
	}

private:
	template<typename First, typename Second, typename... Rests>
	Construct(First&& first, EdgeWeight weight, Second&& second, Rests...&& rests)
	{
		AddEdge(std::forward<First>(first), weight, std::forward<Second>(second));
		Construct(rests...);
	}

	template<typename First, typename Second>
	Construct(First&& first, EdgeWeight weight, Second&& second)
	{
		AddEdge(std::forward<First>(first), weight, std::forward<Second>(second));
	}

	using INFINITE = std::numeric_limits<EdgeWeight>::max();
	Edge edges;
};