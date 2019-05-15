#include <cmath>
#include <cassert>
#include <list>
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>
#define DEBUG
#include "Astar.h"

using namespace Astar;

int map[10][10] =
{
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 1, 1, 1, 0, 1, 2, 2, 2,
	2, 2, 0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 2, 0, 0,
	0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
};

using Distance=double;
Distance heuristic = 0;

class Test :public Point<Distance>
{
public:
	Test(int l, int c) :line(l), column(c)
	{
		points[line][column].reset(this);
	}

	std::vector<Point_ptr<Distance>> GetSurroundPoint()override
	{
		std::vector<Point_ptr<Distance>> results;
		if (line + 1 < 10 && map[line + 1][column] == 0)
			results.emplace_back(points[line + 1][column]);
		if (column + 1 < 10 && map[line][column + 1] == 0)
			results.emplace_back(points[line][column + 1]);
		if (line - 1 >= 0 && map[line - 1][column] == 0)
			results.emplace_back(points[line - 1][column]);
		if (column - 1 >= 0 && map[line][column - 1] == 0)
			results.emplace_back(points[line][column - 1]);
		return results;
	}

	Distance GetEstimatedDistance(const Point_ptr<Distance> & other)override
	{
		Test* t = dynamic_cast<Test*>(other.get());
		return std::abs(line - t->line) + std::abs(column - t->column);
		//return 0;
	}

	Distance GetDistanceWithWeight()override
	{
		//heuristic += 0.01;
		return (map[line][column] == 0 ? 1 : 2) + heuristic;
	}

	static Point_ptr<Distance> points[10][10];
	int line, column;
};
Point_ptr<Distance> Test::points[10][10];

int main()
{
	int start_line = 0, start_column = 0;
	int end_line = 9, end_column = 9;

	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			new Test(i, j);

	auto && path = FindPath(Test::points[start_line][start_column], Test::points[end_line][end_column]);
	for (auto&& i : path)
	{
		Test* t = dynamic_cast<Test*>(i.get());
		map[t->line][t->column] = 3;
	}
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
			std::cout << map[i][j] << " ";
		std::cout << std::endl;
	}
	return 0;
}