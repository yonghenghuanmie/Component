#include <cassert>
#include <ranges>
#include <random>
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
#include "Elapse.h"

int main()
{
	Graph<char> graph(std::numeric_limits<int>::max());
	std::vector<char> set(13);
	std::iota(set.begin(), set.end(), 'A');
	std::default_random_engine engine;
	std::poisson_distribution distribution(5);
	for (size_t i = 0; i < set.size() * (set.size() - 1) / 2; i++)
	{
		std::shuffle(set.begin(), set.end(), engine);
		graph.AddEdge(set[0], set[1]) = distribution(engine);
	}

	std::vector<char> test_set;
	for (size_t i = 0; i < 100; i++)
	{
		std::shuffle(set.begin(), set.end(), engine);
		test_set.insert(test_set.end(), set.begin(), set.end());
	}

	{
		elapse e;
		for (size_t i = 0; i < test_set.size() / 2; i++)
			graph.GetBestPath(test_set[i * 2], test_set[i * 2 + 1]);
	}
	{
		elapse e;
		for (size_t i = 0; i < test_set.size() / 2; i++)
			graph.GetBestPath(test_set[i * 2], test_set[i * 2 + 1], std::greater<void>{});
	}
	return 0;
}