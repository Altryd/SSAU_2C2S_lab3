#include <queue>
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <queue>

class Town
{
	std::string name;
	int people;
public:
	Town() { name = ""; people = -1; }
	Town(const std::string& name, const int people) : name(name), people(people) {}
	Town(const Town& rhs)
	{
		name = rhs.name;
		people = rhs.people;
	}
	Town& operator=(const Town& rhs)
	{
		if (*this == rhs) return *this;
		name = rhs.name;
		people = rhs.people;
		return *this;
	}
	bool operator==(const Town& rhs) const
	{
		return ((name == rhs.name) && (people == rhs.people));
	}
	bool operator!=(const Town& rhs) const
	{
		return !(*this == rhs);
	}
	std::string GetName() const
	{
		return name;
	}
	int GetPeopleCount() const { return people; }
};
std::ostream& operator<<(std::ostream& out, const Town& town)
{
	out << town.GetName() << " "; //<< town.GetPeopleCount() << " ";
	return out;
}
template<>
struct std::hash<Town>
{
	size_t operator()(const Town& town) const
	{
		return std::hash<std::string>()(town.GetName());
	}
};

class Road
{
	double length;
	std::string name;
public:
	Road(const double length, const std::string& name) : length(length), name(name) {}
	double GetLength() const { return length; }
	std::string GetName() const { return name; }
	operator double() const { return length; }
};
std::ostream& operator<<(std::ostream& out, const Road& road)
{
	out << road.GetName() << "  " << road.GetLength() << " ";
	return out;
}


template<typename TVertex, typename TEdge>
struct Connection {
	const TVertex src;
	const TVertex dest;
	const TEdge edge;

	Connection(const TVertex& src, const TVertex& dest, const TEdge& edge) : src(src), dest(dest), edge(edge) {}
	bool operator==(const Connection& rhs) const
	{
		return ((src == rhs.src) && (dest == rhs.dest) && (edge == rhs.edge));
	}
	bool operator!=(const Connection& rhs) const
	{
		return !(*this == rhs);
	}
};


template<typename TVertex, typename TEdge>
class Graph
{
private:
	std::vector<TVertex> vert;
	std::unordered_map<TVertex, std::list<Connection<TVertex, TEdge>>> edges;
	void SearchInDepth(const TVertex& start, std::unordered_map<TVertex, bool>& colors, std::unordered_map<TVertex, TVertex>& pred) const
	{
		const auto needed_list = edges.find(start);
		for (const auto it : needed_list->second)
		{
			if (colors[it.dest] == false)
			{
				pred[it.dest] = start;
				colors[it.dest] = true;
				SearchInDepth(it.dest, colors, pred);
			}
		}
	}
	struct PQcompare
	{
		size_t operator()(const std::pair<TVertex, double>& lhs, const std::pair<TVertex, double>& rhs) const
		{
			return lhs.second < rhs.second;
		}
	};
public:
	void AddVertex(const TVertex& vertex)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		if (std::find(begin, end, vertex) != end)
		{
			throw "Vertex is already inserted";
		}
		vert.push_back(vertex);
		std::list<Connection<TVertex, TEdge>> blank_list;
		edges.insert(std::pair<TVertex, std::list<Connection<TVertex, TEdge>>>(vertex, blank_list));
	}

	void AddEdge(const Connection<TVertex, TEdge>& connection)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		if (std::find(begin, end, connection.src) == end || std::find(begin, end, connection.dest) == end)
		{
			throw "one of vertexes is unknown";
		}
		edges[connection.src].push_front(connection);
	}

	void RemoveEdge(const Connection<TVertex, TEdge>& connection)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		if (std::find(begin, end, connection.src) == end || std::find(begin, end, connection.dest) == end)
		{
			throw "one of vertexes is unknown";
		}
		auto begin_list = edges[connection.src].begin();
		auto end_list = edges[connection.src].end();
		auto needed_to_erase = std::find(begin_list, end_list, connection);
		if (needed_to_erase != end_list)
		{
			edges[connection.src].erase(needed_to_erase);
		}
	}
	void RemoveVertex(const TVertex& vertex)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		auto needed_to_delete = std::find(begin, end, vertex);
		if (needed_to_delete != end)
		{
			vert.erase(needed_to_delete);
			for (auto pair : edges)
			{
				auto it_list = pair.second.begin();
				for (; it_list != pair.second.end(); )
				{
					if (it_list->dest == vertex)
					{
						it_list = pair.second.erase(it_list);
					}
					else
					{
						++it_list;
					}
				}
			}
		}
	}

	
	std::list<TVertex> SearchInWidth(const TVertex& source, const TVertex& dest) const
	{
		std::unordered_map<TVertex, bool> colors;  //0 = непосещенная
		std::unordered_map<TVertex, TVertex> pred;
		std::unordered_map<TVertex, int> distance;
		for (auto it : vert)
		{
			colors.insert(std::pair<TVertex, bool>(it, false));
			distance.insert(std::pair<TVertex, int>(it, std::numeric_limits<int>::max()));
		}
		std::queue<TVertex> q;
		q.push(source);
		colors[source] = true;
		distance[source] = 0;
		while (!q.empty())
		{
			const TVertex u = q.front();
			const auto needed_list = edges.find(u);
			for (const auto it : needed_list->second)
			{
				if (colors[it.dest] == false)
				{
					colors[it.dest] = true;
					distance[it.dest] = distance[u] + 1;
					pred[it.dest] = u;
					q.push(it.dest);
				}
			}
			colors[u] = true;
			q.pop();
		}
		std::list<TVertex> path_to_dest;
		path_to_dest.push_back(dest);
		auto prev_u = pred.find(dest);
		while (prev_u != pred.end())
		{
			path_to_dest.push_front(prev_u->second);
			prev_u = pred.find(prev_u->second);
		}
		return path_to_dest;
	}



	void SearchInDepth(const TVertex& start) const
	{
		//Инициализация- для каждой вершины цвет = белый, пред = нуль
		//После - цвет(старт) = черный, для каждой смежной с старт:
		//		если цвет белый, то пред = старт, поиск_в_глубину(смежной)
		//цвет(старт) = черный
		// + после окончания алгоритма: для каждой вершины которая осталась белой - поиск в глубину
		std::unordered_map<TVertex, bool> colors; // 1 = visited, 0 = not visited
		std::unordered_map<TVertex, TVertex> pred;
		for (const auto vertex : vert)
		{
			colors[vertex] = false;
		}
		colors[start] = true;
		const auto needed_list = edges.find(start);
		for (const auto it : needed_list->second)
		{
			if (colors[it.dest] == false)
			{
				pred[it.dest] = start;
				colors[it.dest] = true;
				SearchInDepth(it.dest, colors, pred);
			}
		}
	}

	std::pair<std::list<TVertex>, double> Djkstra(const TVertex& source, const TVertex& dest) const
	{
		//Инициализация: для каждой вершины расстояние = беск, пред = нуль, d(source) = 0
		//S = список пройденных вершин, Q - очередь с приоритетами, изначально содержит все вершины
		//Пока Q не пуста: извлекаем минимальную вершину, добавляем ее в S
		// Для каждой смежной вершины с минимальной:
		//		Если (d(смежной) > d(min)+w(мин, смежной)): d(смежной) = d(min) + w(мин, смежной) , пред(смежной) = мин
		std::unordered_map<TVertex, double> distance;
		std::unordered_map<TVertex, TVertex> pred;
		std::list<TVertex> S;
		double res_weight = 0;
		//std::priority_queue<std::pair<TVertex, double>&, std::vector<std::pair<TVertex, double>&>, PQcompare> q;
		for (const auto vertex : vert)
		{
			distance[vertex] = std::numeric_limits<double>::max();
		}
		distance[source] = 0;
		while (!distance.empty())
		{
			auto min = std::min_element(distance.begin(), distance.end(), [](const std::pair<TVertex, double>& a, const std::pair<TVertex, double> b)->bool {return a.second < b.second; });
			if (std::find(S.begin(), S.end(), min->first) == S.end())
			{
				S.push_front(min->first);
				auto pair_with_list = edges.find(min->first);
				for (const auto it : pair_with_list->second)
				{
					if (distance[it.dest] > distance[min->first] + static_cast<double>(it.edge))
					{
						distance[it.dest] = distance[min->first] + static_cast<double>(it.edge);
						pred[it.dest] = min->first;
					}
				}
				if (min->first == dest) {
					res_weight = distance[dest];
				}
			}
			distance.erase(min->first);
		}
		std::list<TVertex> path_to_dest;
		path_to_dest.push_back(dest);
		auto prev_u = pred.find(dest);
		while (prev_u != pred.end())
		{
			path_to_dest.push_front(prev_u->second);
			prev_u = pred.find(prev_u->second);
		}
		return std::pair<std::list<TVertex>, double>(path_to_dest, res_weight);
	}

	void Print() const
	{
		std::cout << "Вершина: (Вершина-источник, Вершина-приемник, Ребро)" << std::endl;
		auto begin = vert.begin();
		auto end = vert.end();
		for (; begin != end; begin++)
		{
			std::cout << *begin << " : ";
			auto pair_with_list = edges.find(*begin);
			for (const auto it : pair_with_list->second)
			{
				std::cout << "( " << it.src << ",  " << it.dest << ",  " << it.edge << " ) ";
			}
			std::cout << std::endl;
		}

	}
};


//#define easiest
#define medium
int main()
{
	setlocale(LC_ALL, "Ru");
#ifdef easiest
	Graph<int, int> g;
	g.AddVertex(3);
	g.AddVertex(5);
	g.AddVertex(7);
	g.AddVertex(9);
	g.AddVertex(10);
	g.AddVertex(15);
	g.AddVertex(22);
	g.AddEdge(Connection<int, int>(5, 3, 1));
	g.AddEdge(Connection<int, int>(5, 7, 1));
	g.AddEdge(Connection<int, int>(3, 7, 1));
	g.AddEdge(Connection<int, int>(7, 9, 1));
	g.AddEdge(Connection<int, int>(9, 10, 1));
	g.AddEdge(Connection<int, int>(10, 15, 1));
	g.AddEdge(Connection<int, int>(3, 5, 1));
	g.AddEdge(Connection<int, int>(15, 22, 1));
	g.RemoveEdge(Connection<int, int>(15, 22, 1));
	g.AddEdge(Connection<int, int>(15, 22, 1));
	g.Print();
	int source = 3;
	int dest = 22;
	const auto list = g.SearchInWidth(source, dest);
	if (dest != source && list.size() == 1)
	{
		std::cout << "Пути в данную вершину нет";
	}
	else
	{
		for (const auto it : list)
		{
			if (it != dest) std::cout << it << " -> ";
			else std::cout << it;
		}
		std::cout << std::endl << "Длина пути: " << list.size() << std::endl;
	}
#endif
#ifdef medium
	Graph<Town, Road> g1;
	Town some_city("Москва", 5100000);
	Town some_city2("Безенчук", 20000);
	Town some_city3("Магадан", 90000);
	Town some_city4("Иркутск", 100000);
	Town some_city5("Владивосток", 500000);
	Town some_city6("Вашингтон", 9800000);
	Town some_city7("Минск", 7855000);

	g1.AddVertex(some_city);
	g1.AddVertex(Town("Самара", 1000000));
	g1.AddVertex(some_city2);
	g1.AddVertex(some_city3);
	g1.AddVertex(some_city4);
	g1.AddVertex(some_city5);
	g1.AddVertex(some_city6);
	g1.AddVertex(some_city7);
	g1.AddEdge(Connection<Town, Road>(some_city, Town("Самара", 1000000), Road(10.1, "Ми-6")));
	g1.AddEdge(Connection<Town, Road>(Town("Самара", 1000000), some_city, Road(10.3, "Ми-6")));
	g1.AddEdge(Connection<Town, Road>(Town("Самара", 1000000), some_city2, Road(2.3, "Дорога")));
	g1.AddEdge(Connection<Town, Road>(some_city2, some_city3, Road(7.3, "Еду в Магадан")));
	g1.AddEdge(Connection<Town, Road>(some_city, some_city4, Road(15.01, "Байкал-Москва")));
	g1.AddEdge(Connection<Town, Road>(some_city3, some_city4, Road(5.15, "Экскурсия на Байкал")));
	g1.AddEdge(Connection<Town, Road>(some_city4, some_city5, Road(5.15, "Иркутск-Владивосток")));
	g1.AddEdge(Connection<Town, Road>(some_city, some_city5, Road(28.15, "Москва-Владивосток")));
	g1.AddEdge(Connection<Town, Road>(some_city5, some_city, Road(28.15, "Владивосток-Москва")));
	g1.AddEdge(Connection<Town, Road>(some_city, some_city7, Road(30.33, "Москва-Минск")));
	g1.AddEdge(Connection<Town, Road>(some_city7, some_city, Road(30.33, "Минск-Москва")));
	g1.SearchInDepth(Town("Самара", 1000000));
	const auto source = some_city3;
	const auto dest = some_city7;
	const auto pair = g1.Djkstra(source, dest);
	const auto list = pair.first;
	const double weight = pair.second;
	if (list.size() == 1)
	{
		std::cout << "Пути в данную вершину нет";
	}
	else
	{
		for (const auto it : list)
		{
			if (it != dest) std::cout << it << " -> ";
			else std::cout << it;
		}
		std::cout << std::endl << "Вес пути: " << weight << std::endl;
	}
	std::cout << std::endl << std::endl;
	g1.Print();
#endif
}