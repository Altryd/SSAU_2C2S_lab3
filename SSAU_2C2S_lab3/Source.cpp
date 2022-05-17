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
	Town() { name = ""; people = 0; }
	Town(const std::string& name, const int people) : name(name), people(people) {}
	Town(const Town& rhs)
	{
		name = rhs.name;
		people = rhs.people;
	}
	Town& operator=(const Town& rhs)
	{
		if (this == &rhs) return *this;
		name = rhs.name;
		people = rhs.people;
		return *this;
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

template<>
struct std::equal_to<Town>
{
	size_t operator()(const Town& lhs, const Town& rhs) const
	{
		return ((lhs.GetName() == rhs.GetName()) && (rhs.GetPeopleCount() == rhs.GetPeopleCount()));
	}
};

class Road
{
	double length;
	std::string name;
public:
	Road() { length = 0; }
	Road(const double length, const std::string& name) : length(length), name(name) {}
	double GetLength() const { return length; }
	std::string GetName() const { return name; }
	explicit operator double() const { return length; }  // explicit чтобы не было неявных преобразований
};

enum RoadTypes
{
	Default = 0,
	Taxes = 1,
	HighCost = 2
};
class RoadWithCoefficient
{
	double length;
	std::string name;
	RoadTypes type;
public:
	RoadWithCoefficient() { length = 0; type = Default; }
	RoadWithCoefficient(const double length, const std::string& name, const RoadTypes type) : length(length), name(name), type(type) {}
	double GetLength() const { return length; }
	std::string GetName() const { return name; }
	RoadTypes GetType() const { return type; }
};

std::ostream& operator<<(std::ostream& out, const Road& road)
{
	out << road.GetName() << "  " << road.GetLength() << " ";
	return out;
}


template<typename TVertex, typename TEdge, typename KeyEqual = std::equal_to<TVertex>>
struct Connection {
	const TVertex src;
	const TVertex dest;
	const TEdge edge;

	Connection(const TVertex& src, const TVertex& dest, const TEdge& edge) : src(src), dest(dest), edge(edge) {}
	Connection(const TVertex& src, const TVertex& dest) : src(src), dest(dest) {}
	bool operator==(const Connection& rhs) const
	{
		KeyEqual equality;
		return (equality(src, rhs.src) && equality(dest, rhs.dest));
	}
	bool operator!=(const Connection& rhs) const
	{
		return !(*this == rhs);
	}
};

template<typename TEdge>
class DefaultWeightSelector
{
public:
	double operator()(const TEdge& edge) const
	{
		double res = static_cast<double>(edge);
		return res;
	}
};


class RoadWithCoefficientSelector
{
public:
	double operator()(const RoadWithCoefficient& edge) const
	{
		double length = edge.GetLength();
		switch (edge.GetType())
		{
		case Taxes:
			length *= 10;
			break;

		case HighCost:
			length *= 500;
			break;
		default:
			break;
		}
		return length;
	}
};
std::ostream& operator<<(std::ostream& out, const RoadWithCoefficient& road)
{
	RoadTypes type = road.GetType();
	std::string additional_info;
	switch (type)
	{
	case Taxes:
		additional_info = std::string("С налогами");
		break;
	case HighCost:
		additional_info = std::string("Очень высокая цена");
		break;
	default:
		additional_info = std::string("Стандартная");
		break;
	}
	out << road.GetName() << "  " << road.GetLength() << " " + additional_info;
	return out;
}


template<typename TVertex, typename TEdge, typename Hasher = std::hash<TVertex>, typename KeyEqual=std::equal_to<TVertex>>
class Graph
{
private:
	std::vector<TVertex> vert;
	std::unordered_map<TVertex, std::list<Connection<TVertex, TEdge>>, Hasher, KeyEqual> edges;
	void SearchInDepth(const TVertex& start, std::unordered_map<TVertex, bool>& colors, std::unordered_map<TVertex, TVertex>& pred, void (*func)(const TVertex&)) const
	{
		const auto needed_list = edges.find(start);
		for (const auto it : needed_list->second)
		{
			if (colors[it.dest] == false)
			{
				pred[it.dest] = start;
				colors[it.dest] = true;
				func(it.dest);
				SearchInDepth(it.dest, colors, pred, func);
			}
		}
	}
	std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<TVertex>>> FindInVertex(const TVertex& vertex, const std::vector<TVertex>& vector) const
	{
		KeyEqual equality;
		auto it = vector.begin();
		for (; it != vector.end(); it++)
		{
			if (equality(vertex, *it)) return it;
		}
		return it;
	}
public:
	void AddVertex(const TVertex& vertex)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		if (FindInVertex(vertex, vert) != end)
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
		if (FindInVertex(connection.src, vert) == end || FindInVertex(connection.dest, vert) == end)
		{
			throw "one of vertexes is unknown";
		}
		edges[connection.src].push_front(connection);
	}

	void AddEdge(const TVertex& src, const TVertex& dest, const TEdge& edge)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		if (FindInVertex(src, vert) == end || FindInVertex(dest, vert) == end)
		{
			throw "one of vertexes is unknown";
		}
		edges[src].push_front(Connection<TVertex, TEdge>(src, dest, edge));
	}

	void RemoveEdge(const Connection<TVertex, TEdge>& connection)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		if (FindInVertex(connection.src, vert) == end || FindInVertex(connection.dest, vert) == end)
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
	void RemoveEdge(const TVertex& src, const TVertex& dst)
	{
		auto begin = vert.begin();
		auto end = vert.end();
		if (FindInVertex(src, vert) == end || FindInVertex(dst, vert) == end)
		{
			throw "one of vertexes is unknown";
		}
		auto begin_list = edges[src].begin();
		auto end_list = edges[src].end();
		Connection<TVertex, TEdge> to_delete(src, dst);
		auto needed_to_erase = std::find(begin_list, end_list, to_delete);
		if (needed_to_erase != end_list)
		{
			edges[src].erase(needed_to_erase);
		}
	}

	void RemoveVertex(const TVertex& vertex)
	{
		KeyEqual equality;
		auto begin = vert.begin();
		auto end = vert.end();
		auto needed_to_delete = FindInVertex(vertex, vert);
		if (needed_to_delete != end)
		{
			vert.erase(needed_to_delete);
			auto unordered_map_erase_pair = edges.find(vertex);
			if (unordered_map_erase_pair != edges.end()) edges.erase(unordered_map_erase_pair);
			for (auto& pair : edges)
			{
				auto it_list = pair.second.begin();
				for (; it_list != pair.second.end(); )
				{
					if (equality(it_list->dest, vertex))
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

	void SearchInDepth(const TVertex& start, void (*func)(const TVertex&)) const
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
		func(start);
		const auto needed_list = edges.find(start);
		for (const auto it : needed_list->second)
		{
			if (colors[it.dest] == false)
			{
				pred[it.dest] = start;
				colors[it.dest] = true;
				func(it.dest);
				SearchInDepth(it.dest, colors, pred, func);
			}
		}
	}
	
	template<typename WeightSelector = DefaultWeightSelector<TEdge>>
	std::pair<std::list<TVertex>, double> Dijkstra(const TVertex& source, const TVertex& dest) const
	{
		//Инициализация: для каждой вершины расстояние = беск, пред = нуль, d(source) = 0
		//S = список пройденных вершин, Q - очередь с приоритетами, изначально содержит все вершины (здесь не используется)
		//Пока Q не пуста: извлекаем минимальную вершину, добавляем ее в S
		// Для каждой смежной вершины с минимальной:
		//		Если (d(смежной) > d(min)+w(мин, смежной)): d(смежной) = d(min) + w(мин, смежной) , пред(смежной) = мин
		WeightSelector selector;
		std::unordered_map<TVertex, double> distance;
		std::unordered_map<TVertex, TVertex> pred;
		std::vector<TVertex> visited_vertexes;
		KeyEqual equality = KeyEqual();
		double res_weight = 0;
		for (const auto vertex : vert)
		{
			distance[vertex] = std::numeric_limits<double>::max();
		}
		distance[source] = 0;
		while (!distance.empty())
		{
			auto min = std::min_element(distance.begin(), distance.end(), [](const std::pair<TVertex, double>& a, const std::pair<TVertex, double> b)->bool {return a.second < b.second; });
			if (FindInVertex(min->first, visited_vertexes) == visited_vertexes.end())
			{
				visited_vertexes.push_back(min->first);
				auto pair_with_list = edges.find(min->first);
				for (const auto it : pair_with_list->second)
				{
					double w = selector(it.edge);
					if (w < 0) throw "Algorithm cannot be executed if there are negative weights";
					if (distance[it.dest] > distance[min->first] + w)
					{
						distance[it.dest] = distance[min->first] + w;
						pred[it.dest] = min->first;
					}
				}
				if (equality(min->first, dest)) {
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
		std::cout << "Вершина: (Вершина-приемник, Ребро)" << std::endl;
		auto begin = vert.begin();
		auto end = vert.end();
		for (; begin != end; begin++)
		{
			std::cout << *begin << " : ";
			auto pair_with_list = edges.find(*begin);
			for (const auto it : pair_with_list->second)
			{
				std::cout << "( " << it.dest << ",  " << it.edge << " ) ";
			}
			std::cout << std::endl;
		}
	}
};

void printTown(const Town& town)
{
	std::cout << town.GetName() << std::endl;
}


//#define easiest
//#define medium
//#define IRL
#define HARD_IRL
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
	Town moscow("Москва", 5100000);
	Town minsk("Безенчук", 20000);
	Town samara("Магадан", 90000);
	Town bezenchuk("Иркутск", 100000);
	Town ufa("Владивосток", 500000);
	Town surgut("Вашингтон", 9800000);
	Town irkutsk("Минск", 7855000);

	g1.AddVertex(moscow);
	g1.AddVertex(Town("Самара", 1000000));
	g1.AddVertex(minsk);
	g1.AddVertex(samara);
	g1.AddVertex(bezenchuk);
	g1.AddVertex(ufa);
	g1.AddVertex(surgut);
	g1.AddVertex(irkutsk);
	g1.AddEdge(Connection<Town, Road>(moscow, Town("Самара", 1000000), Road(10.1, "Ми-6")));
	g1.AddEdge(Connection<Town, Road>(Town("Самара", 1000000), moscow, Road(10.3, "Ми-6")));
	g1.AddEdge(Connection<Town, Road>(Town("Самара", 1000000), minsk, Road(2.3, "Дорога")));
	g1.AddEdge(Connection<Town, Road>(minsk, samara, Road(7.3, "Еду в Магадан")));
	g1.AddEdge(Connection<Town, Road>(moscow, bezenchuk, Road(15.01, "Байкал-Москва")));
	g1.AddEdge(Connection<Town, Road>(samara, bezenchuk, Road(5.15, "Экскурсия на Байкал")));
	g1.AddEdge(Connection<Town, Road>(bezenchuk, ufa, Road(5.15, "Иркутск-Владивосток")));
	g1.AddEdge(Connection<Town, Road>(moscow, ufa, Road(28.15, "Москва-Владивосток")));
	g1.AddEdge(Connection<Town, Road>(ufa, moscow, Road(28.15, "Владивосток-Москва")));
	g1.AddEdge(Connection<Town, Road>(moscow, irkutsk, Road(30.33, "Москва-Минск")));
	g1.AddEdge(Connection<Town, Road>(irkutsk, moscow, Road(30.33, "Минск-Москва")));
	g1.SearchInDepth(Town("Самара", 1000000), printTown);
	const auto source = samara;
	const auto dest = irkutsk;
	const auto pair = g1.Dijkstra(source, dest);
	const auto list = pair.first;
	const double weight = pair.second;
	if (list.size() == 1)
	{
		std::cout << "Пути в данную вершину нет";
	}
	else
	{
		std::equal_to<Town> equality;
		for (const auto it : list)
		{
			if (!equality(it, dest)) std::cout << it << " -> ";
			else std::cout << it;
		}
		std::cout << std::endl << "Вес пути: " << weight << std::endl;
	}
	std::cout << std::endl << std::endl;
	g1.Print();
#endif
#ifdef IRL
	Graph<Town, Road> graph_real;
	Town moscow("Москва", 12635466);
	Town saint_petersburg("Санкт-Петербург", 5377503);
	Town minsk("Минск", 1996553);
	Town samara("Самара", 1144759);
	Town bezenchuk("Безенчук", 21540);
	Town ufa("Уфа", 1125933);
	Town surgut("Сургут", 395900);
	Town irkutsk("Иркутск", 617315);
	Town khabarovsk("Хабаровск", 613480);
	Town washington("Вашингтон", 689545);
	Town new_york("Нью-Йорк", 8804190);
	
	

	graph_real.AddVertex(moscow);
	graph_real.AddVertex(saint_petersburg);
	graph_real.AddVertex(minsk);
	graph_real.AddVertex(samara);
	graph_real.AddVertex(bezenchuk);
	graph_real.AddVertex(ufa);
	graph_real.AddVertex(surgut);
	graph_real.AddVertex(irkutsk);
	graph_real.AddVertex(khabarovsk);
	graph_real.AddVertex(washington);
	graph_real.AddVertex(new_york);
	graph_real.AddEdge(moscow, minsk, Road(716.1, "М-1"));  //Москва
	graph_real.AddEdge(moscow, samara, Road(1057.3, "М-2"));
	graph_real.AddEdge(moscow, ufa, Road(1355.2, "М-3"));
	graph_real.AddEdge(moscow, surgut, Road(2876.4, "М-4"));
	graph_real.AddEdge(moscow, saint_petersburg, Road(710.5, "М-11"));

	graph_real.AddEdge(samara, bezenchuk, Road(96.1, "Р-229")); //Самара
	graph_real.AddEdge(samara, moscow, Road(1057.3, "М-2"));
	graph_real.AddEdge(samara, ufa, Road(473.1, "М-5"));

	graph_real.AddEdge(bezenchuk, minsk, Road(1939.2, "Р-120"));  //Безенчук
	graph_real.AddEdge(bezenchuk, samara, Road(96.1, "Р-229"));
	graph_real.AddEdge(bezenchuk, irkutsk, Road(4367.1, "Р-229"));

	graph_real.AddEdge(ufa, samara, Road(473.1, "М-5"));  //Уфа
	graph_real.AddEdge(ufa, surgut, Road(1678.3, "Р-404"));
	graph_real.AddEdge(ufa, irkutsk, Road(3826.9, "Р-255"));

	graph_real.AddEdge(surgut, irkutsk, Road(2763.8, "Р-256"));  //Сургут
	graph_real.AddEdge(surgut, khabarovsk, Road(6941.5, "Р-257"));

	graph_real.AddEdge(irkutsk, ufa, Road(3826.9, "Р-255"));  //Иркутск
	graph_real.AddEdge(irkutsk, khabarovsk, Road(3207.9, "Р-457"));

	graph_real.AddEdge(khabarovsk, irkutsk, Road(3207.9, "Р-457"));  //Хабаровск

	graph_real.AddEdge(saint_petersburg, moscow, Road(710.5, "М-11"));  //SPB

	graph_real.AddEdge(minsk, moscow, Road(716.1, "М-1"));  //Минск

	graph_real.AddEdge(washington, new_york, Road(380.0, "DC-NY"));
	graph_real.AddEdge(new_york, washington, Road(380.0, "DC-NY"));
	graph_real.RemoveVertex(new_york);
	

	graph_real.SearchInDepth(moscow, printTown);
	const auto source = minsk;
	const auto dest = khabarovsk;
	const auto pair = graph_real.Dijkstra(source, dest);
	const auto list = pair.first;
	const double weight = pair.second;
	if (list.size() == 1)
	{
		std::cout << "Пути в вершину " << dest << " нет";
	}
	else
	{
		for (const auto it : list)
		{
			if (it.GetName() != dest.GetName()) std::cout << it << " -> ";
			else std::cout << it;
		}
		std::cout << std::endl << "Вес пути: " << weight << std::endl;
	}
	std::cout << std::endl << std::endl;
	graph_real.Print();
#endif
#ifdef HARD_IRL
	Graph<Town, RoadWithCoefficient> graph_real;
	Town moscow("Москва", 12635466);
	Town saint_petersburg("Санкт-Петербург", 5377503);
	Town minsk("Минск", 1996553);
	Town samara("Самара", 1144759);
	Town bezenchuk("Безенчук", 21540);
	Town ufa("Уфа", 1125933);
	Town surgut("Сургут", 395900);
	Town irkutsk("Иркутск", 617315);
	Town khabarovsk("Хабаровск", 613480);
	Town washington("Вашингтон", 689545);
	Town new_york("Нью-Йорк", 8804190);


	graph_real.AddVertex(moscow);
	graph_real.AddVertex(saint_petersburg);
	graph_real.AddVertex(minsk);
	graph_real.AddVertex(samara);
	graph_real.AddVertex(bezenchuk);
	graph_real.AddVertex(ufa);
	graph_real.AddVertex(surgut);
	graph_real.AddVertex(irkutsk);
	graph_real.AddVertex(khabarovsk);
	graph_real.AddVertex(washington);
	graph_real.AddVertex(new_york);
	graph_real.AddEdge(moscow, minsk, RoadWithCoefficient(716.1, "М-1", Default));  //Москва
	graph_real.AddEdge(moscow, samara, RoadWithCoefficient(1057.3, "М-2", Default));
	graph_real.AddEdge(moscow, ufa, RoadWithCoefficient(1355.2, "М-3", Default));
	graph_real.AddEdge(moscow, surgut, RoadWithCoefficient(2876.4, "М-4", Default));
	graph_real.AddEdge(moscow, saint_petersburg, RoadWithCoefficient(710.5, "М-11", Default));

	graph_real.AddEdge(samara, bezenchuk, RoadWithCoefficient(96.1, "Р-229", Default)); //Самара
	graph_real.AddEdge(samara, moscow, RoadWithCoefficient(1057.3, "М-2", Default));
	graph_real.AddEdge(samara, ufa, RoadWithCoefficient(473.1, "М-5", Default));

	graph_real.AddEdge(bezenchuk, minsk, RoadWithCoefficient(1939.2, "Р-120", Default));  //Безенчук
	graph_real.AddEdge(bezenchuk, samara, RoadWithCoefficient(96.1, "Р-229", Default));
	graph_real.AddEdge(bezenchuk, irkutsk, RoadWithCoefficient(4367.1, "Р-229", Default));

	graph_real.AddEdge(ufa, samara, RoadWithCoefficient(473.1, "М-5", Default));  //Уфа
	graph_real.AddEdge(ufa, surgut, RoadWithCoefficient(1678.3, "Р-404", Default));
	graph_real.AddEdge(ufa, irkutsk, RoadWithCoefficient(3826.9, "Р-255", Default));

	graph_real.AddEdge(surgut, irkutsk, RoadWithCoefficient(2763.8, "Р-256", Default));  //Сургут
	graph_real.AddEdge(surgut, khabarovsk, RoadWithCoefficient(6941.5, "Р-257", Default));

	graph_real.AddEdge(irkutsk, ufa, RoadWithCoefficient(3826.9, "Р-255", Default));  //Иркутск
	graph_real.AddEdge(irkutsk, khabarovsk, RoadWithCoefficient(3207.9, "Р-457", Default));

	graph_real.AddEdge(khabarovsk, irkutsk, RoadWithCoefficient(3207.9, "Р-457", Default));  //Хабаровск

	graph_real.AddEdge(saint_petersburg, moscow, RoadWithCoefficient(710.5, "М-11", Default));  //SPB

	graph_real.AddEdge(minsk, moscow, RoadWithCoefficient(716.1, "М-1", HighCost));  //Минск

	graph_real.AddEdge(washington, new_york, RoadWithCoefficient(380.0, "DC-NY", Default));
	graph_real.AddEdge(new_york, washington, RoadWithCoefficient(380.0, "DC-NY", Default));
	graph_real.RemoveVertex(new_york);


	graph_real.SearchInDepth(moscow, printTown);
	const auto source = minsk;
	const auto dest = khabarovsk;
	const auto pair = graph_real.Dijkstra<RoadWithCoefficientSelector>(source, dest);
	const auto list = pair.first;
	const double weight = pair.second;
	if (list.size() == 1)
	{
		std::cout << "Пути в вершину " << dest << " нет";
	}
	else
	{
		for (const auto it : list)
		{
			if (it.GetName() != dest.GetName()) std::cout << it << " -> ";
			else std::cout << it;
		}
		std::cout << std::endl << "Вес пути: " << weight << std::endl;
	}
	std::cout << std::endl << std::endl;
	graph_real.Print();
#endif
}