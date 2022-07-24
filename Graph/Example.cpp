#include <cassert>
#include <limits>
#include <variant>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <execution>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include "Graph.h"

int main()
{
	Graph<std::string> graph(std::numeric_limits<int>::max(), {
		{ "A", 9, "B" }, { "A", 5, "C" }, { "C", 2, "B" },
		{ "A", 3, "D" }, { "B", 5, "D" }, { "D", 2, "C" },
		{ "D", 2, "E" }, { "D", 5, "B" }, { "C", 4, "E" },
		{ "B", 2, "F" }, { "F", 1, "E" }, { "C", 5, "F" },
		});

	graph.AddEdge("C", "D") = 3;
	auto out_path = graph.GetOutPath("C");
	graph.RemoveEdge("C", "D");
	bool not_exist = graph.AddEdge("C", "D") == graph.INFINITE;
	assert(not_exist);
	graph.GetOutPath("C", [](auto&& new_path) {std::cout << "C->" << new_path.first << '\n'; return true; });

	auto in_path = graph.GetInPath("B");
	std::vector<decltype(graph)::PathType> copy_results;
	graph.GetInPath("B", [&copy_results](auto&& new_path) {copy_results.push_back(new_path); return true; });

	auto weight = graph.AddEdge("C", "E");
	not_exist = graph.AddEdge("C", "T") == graph.INFINITE;
	assert(not_exist);

	auto result_handle = [](auto&& value) {
		if constexpr (std::is_same_v<std::remove_reference_t<decltype(value)>, decltype(graph)::ResultsType>)
			for (auto&& path : value)
				std::cout << path.get().first << " " << path.get().second << '\n';
		else
			std::cout << value << '\n';
		std::cout << std::endl;
	};
	// OK
	auto error_or_result = graph.GetBestPath("A", "E");
	std::visit(result_handle, error_or_result);
	// empty vector
	error_or_result = graph.GetBestPath("B", "T");
	std::visit(result_handle, error_or_result);
	// error message
	error_or_result = graph.GetBestPath("F", "F");
	std::visit(result_handle, error_or_result);

	if (graph.CacheSize() > 0)
		graph.ClearCache();

	return 0;
}