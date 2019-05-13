#pragma once
namespace Astar
{
	template<class Distance>
	class Point;

	template<class Distance>
	using Point_ptr=std::shared_ptr<Point<Distance>>;

	template<class Distance>
	class Point
	{
	public:
		virtual ~Point() {}
		virtual std::vector<Point_ptr<Distance>> GetSurroundPoint() = 0;
		virtual Distance GetEstimatedDistance(const Point_ptr<Distance>& other) = 0;
		virtual Distance GetWeight(const Point_ptr<Distance>& other) = 0;

		std::weak_ptr< Point<Distance>> parent;
		Distance path_weight;
	};

	template<class Distance>
	std::list<Point_ptr<Distance>> FindPath(Point_ptr<Distance> start, Point_ptr<Distance> end)
	{
		std::unordered_set<Point_ptr<Distance>> open, close;
		start->path_weight = start->GetWeight(start);
		open.emplace(start);
		do
		{
			if (open.empty())
				return{};
			auto iterator = std::min_element(open.begin(), open.end());
			assert(iterator != open.end());
			auto current = *iterator;
			open.erase(iterator);
			if (current == end)
				break;

			auto && points_list = current->GetSurroundPoint();
			for (auto&& point : points_list)
			{
				if (close.find(point) != close.end())
					continue;
				if (open.find(point) != open.end())
				{
					auto new_weight = current->path_weight + current->GetWeight(point) + point->GetEstimatedDistance(end);
					if (new_weight < point->path_weight)
					{
						point->path_weight = new_weight;
						point->parent = current;
					}
				}
				else
				{
					point->path_weight += current->path_weight + current->GetWeight(point) + point->GetEstimatedDistance(end);
					point->parent = current;
					open.emplace(point);
				}
			}
			close.emplace(current);
		} while (true);

		std::list<Point_ptr<Distance>> list;
		list.emplace_front(end);
		for (auto ptr = list.front()->parent.lock(); ptr; ptr = ptr->parent.lock())
			list.emplace_front(ptr);
		if (list.front() != start)
			return{};
		return list;
	}

}