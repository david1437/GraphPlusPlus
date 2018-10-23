#ifndef GRAPH_BASE_HPP
#define GRAPH_BASE_HPP

#include <cmath>
#include <set>
#include <deque>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <exception>
#include <variant>
#include <any>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <iostream>

#include "graph_types.hpp"

template <class Type,
	  class Mapped = std::set<Type>,
	  class Container = std::map<Type, Mapped>,
	  class Directed = std::false_type>
class graph_base
{
	private:
		// container types
		using container_type  = Container;
		using key_type        = typename container_type::key_type;
		using mapped_type     = typename container_type::mapped_type;

		// mapped types
		using value_type      = Type;
		using allocator_type  = typename Mapped::allocator_type;
		using reference       = typename allocator_type::reference;
		using const_reference = typename allocator_type::const_reference;
		using size_type       = typename allocator_type::size_type;
		using difference_type = typename allocator_type::difference_type;
		using pointer         = typename allocator_type::pointer;
		using const_pointer   = typename allocator_type::const_pointer;

		using const_iterator          = typename container_type::const_iterator;
		using const_reverse_iterator  = typename container_type::const_reverse_iterator;
		using mapped_const_iterator   = typename mapped_type::const_iterator;

		// members
		container_type m_graph;
		std::map<std::pair<value_type, value_type>, float> m_edgeWeight;
		size_type m_nodeCount {0};
		size_type m_edgeCount {0};

		// helper functions
		const_iterator keySearch(const key_type& t_node) const {
			if constexpr (std::is_same_v<container_type, std::map<Type,Mapped>>) {
				auto iter = std::lower_bound(std::begin(m_graph), 
						             std::end(m_graph), 
						             t_node, 
							     [](const std::pair<value_type,mapped_type>& t_value1, const value_type& t_value2) 
							     	{ return t_value1.first < t_value2; }
							    );
				return iter != std::end(m_graph) ? (iter->first != t_node) ? ++iter : iter : iter;
			} else {
				return std::find_if(std::begin(m_graph), 
						    std::end(m_graph), 
						    [&](const std::pair<value_type, mapped_type>& t_value) 
						    	{ return t_value.first == t_node; }
						   );
			}
		}
		[[nodiscard]] bool keyExists(const key_type& t_node) const {
			return keySearch(t_node) != std::end(m_graph);
		}
		[[nodiscard]] bool edgeExists(const key_type& t_node_first, const key_type& t_node_second) const {
			try {
				if constexpr(std::is_same_v<Directed, std::false_type>) {
					auto b1 = std::isnan(m_edgeWeight.at({t_node_first, t_node_second}));
					auto b2 = std::isnan(m_edgeWeight.at({t_node_second, t_node_first}));
					return (!b1 && !b2);
				} else {
					auto b1 = std::isnan(m_edgeWeight.at({t_node_first, t_node_second}));
					return !b1;
				}
			} catch (const std::out_of_range& e) {
				return false;
			}
			return true;
		}
		void addEdgeUndirected(const key_type& t_node_first, const key_type& t_node_second, const float t_weight = 1.0) {
			if(edgeExists(t_node_first, t_node_second)) {
				// throw edge exists exception
				std::cout << "Edge already exists!\n";
			} else {
				m_graph.at(t_node_first).insert(t_node_second);
				m_graph.at(t_node_second).insert(t_node_first);
				m_edgeWeight[std::pair(t_node_first,t_node_second)] = t_weight;
				m_edgeWeight[std::pair(t_node_second,t_node_first)] = t_weight;
				m_edgeCount += 2;
			}
		}
		void addEdgeDirected(const key_type& t_node_first, const key_type& t_node_second, const float t_weight = 1.0) {
			if(edgeExists(t_node_first, t_node_second)) {
				// throw edge exists exception
				std::cout << "Edge already exists!\n";
			} else {
				m_graph.at(t_node_first).insert(t_node_second);
				m_edgeWeight[std::pair(t_node_first,t_node_second)] = t_weight;
				++m_edgeCount;
			}
		}
		void removeEdgeUndirected(const key_type& t_node_first, const key_type& t_node_second) {
			if(!edgeExists(t_node_first, t_node_second)) {
				// throw edge not exists exception
				std::cout << "Edge does not exist!\n";
			} else {
				m_graph.at(t_node_first).erase(t_node_second);
				m_graph.at(t_node_second).erase(t_node_first);
				m_edgeWeight[std::pair(t_node_first,t_node_second)] = NAN_TYPE;
				m_edgeWeight[std::pair(t_node_second,t_node_first)] = NAN_TYPE;
				m_edgeCount -= 2;
			}
		}
		void removeEdgeDirected(const key_type& t_node_first, const key_type& t_node_second) {
			if(!edgeExists(t_node_first, t_node_second)) {
				// throw edge not exists exception
				std::cout << "Edge does not exist!\n";
			} else {
				m_graph.at(t_node_first).erase(t_node_second);
				m_edgeWeight[std::pair(t_node_first,t_node_second)] = NAN_TYPE;
				--m_edgeCount;
			}
		}
		void invalidateEdges(const key_type& t_node, const mapped_type& t_neighbors) {
			for(auto& [key, value] : m_edgeWeight) {
				if(key.first == t_node || key.second == t_node) {
					value = NAN_TYPE;
					--m_edgeCount;
				}
			}
			if constexpr(std::is_same_v<Directed, std::false_type>) {
				for(auto& [key, value] : m_graph) {
					if(t_neighbors.find(key) != std::end(t_neighbors)) {
						value.erase(t_node);
					}
				}
			}
		}

	public:
		// interface
		struct graph_traversal {
			graph_traversal() = default;
			graph_traversal(graph_traversal&&) = default;
			graph_traversal(const graph_traversal&) = default;
			virtual ~graph_traversal() {};
		};

		// constructors
		template<class Iter>
		graph_base(const Iter& t_begin, const Iter& t_end) {
			for(auto iter = t_begin; iter != t_end; ++iter) {
				m_graph[*iter];
				++m_nodeCount;
			}
		}
		template<class T>
		graph_base(const std::initializer_list<T>& t_initializerList) {
			for(auto iter = std::begin(t_initializerList); 
			    iter != std::end(t_initializerList); 
			    ++iter) {
				m_graph[*iter];
				++m_nodeCount;;
			}
		}
		graph_base() = default;

		// non modifying functions
		size_type nodeCount() const noexcept {
			return m_nodeCount;
		}
		size_type edgeCount() const noexcept {
			return m_edgeCount;
		}
		size_type size() const noexcept {
			m_graph.size();
		}
		std::pair<mapped_const_iterator, mapped_const_iterator> neighbors(const key_type& t_node) const {
			if(!keyExists(t_node)) {
				// throw exception
				std::cout << "Key does not exist!\n";
			} else {
				const auto& return_neighbors = m_graph.at(t_node);
				return {std::cbegin(return_neighbors), std::cend(return_neighbors)};
			}
			return {};
		}

		// modifying functions
		void clear() {
			m_graph.clear();
		}
		void addNode(const key_type& t_node) {
			if(keyExists(t_node)) {
				// throw exception
				std::cout << "Key already exists!\n";
			} else {
				m_graph[t_node];
				++m_nodeCount;
			}
		}
		void addNode(key_type&& t_node) {
			if(keyExists(t_node)) {
				// throw exception
				std::cout << "Key already exists!\n";
			} else {
				m_graph[std::move(t_node)];
				++m_nodeCount;
			}
		}
		void removeNode(const key_type& t_node) {
			if(!keyExists(t_node)) {
				// throw exception
				std::cout << "Key does not exist!\n";
			} else {
				const auto& nbors = m_graph.at(t_node);
				m_graph.erase(t_node);
				--m_nodeCount;
				invalidateEdges(t_node, nbors);
			}
		}
		void addEdge(const key_type& t_node_first, const key_type& t_node_second, const float t_weight = 1.0) {
			if constexpr(std::is_same_v<Directed, std::false_type>) {
				addEdgeUndirected(t_node_first, t_node_second, t_weight);
			} else {
				addEdgeDirected(t_node_first, t_node_second, t_weight);
			}
		}
		void removeEdge(const key_type& t_node_first, const key_type& t_node_second) {
			if constexpr(std::is_same_v<Directed, std::false_type>) {
				removeEdgeUndirected(t_node_first, t_node_second);
			} else {
				removeEdgeDirected(t_node_first, t_node_second);
			}
		}
};

#endif
