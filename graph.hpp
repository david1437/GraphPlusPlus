#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <deque>

template <class T, class W>
struct Edge;

template <class T, class W>
struct Node;

template <class T, class W>
struct Edge {
	private:
		W weight {1};
	public:
		Edge() = default;
		T snode {};
		T enode {};
		void setFirstNode(const T& t_n) { snode = t_n; }
		void setSecondNode(const T& t_n) { enode = t_n; }
		virtual void setWeight(const W& m_weight) { weight = m_weight; }
		virtual const W& getWeight() const { return weight; }
		virtual ~Edge() = default;
};

template <class T, class W>
struct Node {
	Node() = default;
	T value {};
	std::vector<std::unique_ptr<Edge<T,W>>> neighbors{};
	bool visited{false};
	void setValue(const T& v) { value = v; }
};

template <class T, class W=double, template<class, class> class E=Edge, template<class, class> class N=Node>
class Graph {
	private:
		std::map<T, std::unique_ptr<N<T,W>>> nodes;
		bool doesKeyExist(const T& key) {
			auto search = nodes.find(key);
			return search != nodes.end();
		};
		bool doesEdgeExist(const T& key1, const T& key2) {
			for(const auto& edge : nodes[key1]->neighbors) {
				if((edge->snode == key1 && edge->enode == key2) || (edge->snode == key2 && edge->enode == key1)) {
					return true;
				}
			}
			return false;
		}
		std::unique_ptr<N<T,W>> createAndInitializeNode(const T& t_n) {
			auto n = std::unique_ptr<N<T,W>>(new N<T,W>());
			n->setValue(t_n);
			return n;
		}

		std::unique_ptr<E<T,W>> createAndInitializeEdge(const T& t_n1, const T& t_n2) {
			auto e = std::unique_ptr<E<T,W>>(new E<T,W>());
			e->setFirstNode(t_n1);
			e->setSecondNode(t_n2);
			return e;
		}
		template <class WeightObject=double>
		std::unique_ptr<E<T,W>> createAndInitializeWeightedEdge(const T& t_n1, const T& t_n2, const WeightObject& t_wo) {
			auto e = std::unique_ptr<E<T,W>>(new E<T,W>());
			e->setFirstNode(t_n1);
			e->setSecondNode(t_n2);
			e->setWeight(t_wo);
			return e;
		}

	public:
		Graph() = default;
		template <class Iter>
		Graph(const Iter& iter) {
			for(const auto& value : iter) {
				addNode(value);
			}
		}
		void addNode(const T& t_n) {
			if(doesKeyExist(t_n)) {
				std::cerr << "WARNING: Attempted to add node that is already in graph!\n";
				return;
			} else {
				nodes[t_n] = createAndInitializeNode(t_n);
			}
		}
		void addEdge(const T& t_n1, const T& t_n2) {
			if(!doesKeyExist(t_n1) || !doesKeyExist(t_n2)) {
				std::cerr << "WARNING: Attempted to build edge with a node not yet in graph!\n";
				return;
			} else if (doesEdgeExist(t_n1, t_n2)) {
				std::cerr << "WARNING: Attempted to build edge that already exists in the graph!\n";
			} else {
				nodes[t_n1]->neighbors.push_back(std::move(createAndInitializeEdge(t_n1, t_n2)));
				nodes[t_n2]->neighbors.push_back(std::move(createAndInitializeEdge(t_n1, t_n2)));
			}
		}
		template <class WeightObject=double>
		void addEdge(const T& t_n1, const T& t_n2, const WeightObject& t_wo) {
			if(!doesKeyExist(t_n1) || !doesKeyExist(t_n2)) {
				std::cerr << "WARNING: Attempted to build edge with a node not yet in graph!\n";
				return;
			} else if (doesEdgeExist(t_n1, t_n2)) {
				std::cerr << "WARNING: Attempted to build edge that already exists in the graph!\n";
			} else {
				nodes[t_n1]->neighbors.push_back(std::move(createAndInitializeWeightedEdge(t_n1, t_n2, t_wo)));
				nodes[t_n2]->neighbors.push_back(std::move(createAndInitializeWeightedEdge(t_n1, t_n2, t_wo)));
			}
		}
};

#endif
