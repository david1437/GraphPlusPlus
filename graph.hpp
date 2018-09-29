#ifndef GRAPH_BASE_HPP
#define GRAPH_BASE_HPP

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
#include "graph_exception.hpp"
#include "graph_types.hpp"

template <class Type,
	  class Mapped = std::vector<std::pair<Type,double>>,
	  class Container = std::map<Type, Mapped>,
	  class Directed = std::false_type>
struct graph_base
{
	// Container types
	using container_type  = Container;
	using key_type        = typename container_type::key_type;
	using mapped_type     = typename container_type::mapped_type;

	// Mapped types
	using value_type      = typename std::pair<Type,double>;
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
	container_type graph_map;
	size_type num_nodes;
	size_type num_edges;

	// iterator
	const_iterator cbegin() { return graph_map.cbegin(); }
	const_iterator cend() { return graph_map.cend(); }
	const_reverse_iterator crbegin() { return graph_map.crbegin(); }
	const_reverse_iterator crend() { return graph_map.crend(); }

	// helper functions
	bool check_key_exist(const key_type& key) const {
		return graph_map.find(key) != graph_map.end();
	}
	void invalidateEdges(const key_type& key) {
		for(auto iter = graph_map.begin(); iter != graph_map.end(); ++iter) {
			const auto& search = std::find_if(iter->second.begin(), iter->second.end(), [&](const value_type& element) { return element.first == key; });
			if(search != iter->second.end()) {
				iter->second.erase(search);
				if constexpr(std::is_same_v<Directed, std::true_type>) {
					num_edges -= 2;
				} else {
					--num_edges;
				}
			}
		}
	}
	bool checkEdgeExists(const key_type& key1, const key_type& key2) const {
		const auto& neighbors_key1 = graph_map.at(key1);
		const auto& search_key2_in_key1 = std::find_if(neighbors_key1.begin(), neighbors_key1.end(), [&](const value_type& element) { return element.first == key2; });
		if constexpr(std::is_same_v<Directed, std::true_type>) {
			return search_key2_in_key1 != neighbors_key1.end();
		} else {
			const auto& neighbors_key2 = graph_map.at(key2);
			const auto& search_key1_in_key2 = std::find_if(neighbors_key2.begin(), neighbors_key2.end(), [&](const value_type& element) { return element.first == key1; });
			return search_key2_in_key1 != neighbors_key1.end() && search_key1_in_key2 != neighbors_key2.end();
		}
		return false;
	}

	// constructors
	graph_base() : num_nodes(0), num_edges(0) {};
	template <class Iterator>
	graph_base(const Iterator& start, const Iterator& stop) : num_nodes(0), num_edges(0) {
		for(Iterator iter = start ; iter != stop; ++iter) {
			insert_node(*iter);
		}
	}
	template <class T>
	graph_base(std::initializer_list<T> l) : num_nodes(0), num_edges(0) {
		static_assert(std::is_copy_constructible_v<T>, "Initializer list elements must be copy constructible!\n");
		for(auto iter = l.begin(); iter != l.end(); ++iter) {
			insert_node(*iter);
		}
	}

	// functions
	size_type node_count() const noexcept { return num_nodes; }
	size_type edge_count() const noexcept { return num_edges; }
	bool insert_node(const key_type& key) {
		const auto check = check_key_exist(key);
		if(check) {
			throw NodeExists("Try removing node first!\n");
		}
		graph_map[key];
		++num_nodes;
		return true;
	}
	bool insert_node(key_type&& key) {
		const auto check = check_key_exist(key);
		if(check) {
			throw NodeExists("Try removing node first!\n");
		}
		graph_map.insert(std::move(key));
		++num_nodes;
		return true;
	}
	bool insert_edge(const key_type& key1, const key_type& key2, const double w = 1.0) {
		const auto check1 = check_key_exist(key1);
		const auto check2 = check_key_exist(key2);
		if(!check1 || !check2) {
			throw NodeNotFound("Check node was added to the container!\n");
		}
		if constexpr(!std::is_same_v<Directed, std::true_type>) {
			return insert_edge_undirected(key1, key2, w);
		} else {
			return insert_edge_directed(key1, key2, w);
		}
		return false;
	}
	bool remove_node(const key_type& key) {
		const auto check = check_key_exist(key);
		if(!check) {
			throw NodeNotFound("Check node was added to the container!\n");
		}
		graph_map.erase(key);
		--num_nodes;
		invalidateEdges(key);
		return true;
	}
	bool remove_edge(const key_type& key1, const key_type& key2) {
		const auto check1 = check_key_exist(key1);
		const auto check2 = check_key_exist(key2);
		if(!check1 || !check2) {
			throw NodeNotFound("Check node was added to the container!\n");
		}
		if constexpr(std::is_same_v<Directed, std::true_type>) {
			return remove_edge_directed(key1, key2);
		} else {
			return remove_edge_undirected(key1, key2);
		}	
		return false;
	}
	std::pair<mapped_const_iterator, mapped_const_iterator> neighbors(const key_type& key) const {
		const auto check = check_key_exist(key);
		if(!check) {
			throw NodeNotFound("Check node was added to the container!\n");
		}
		const auto& neighborList = graph_map.at(key);
		return {neighborList.cbegin(), neighborList.cend()};
	}
	template <class F>
	std::pair<mapped_const_iterator, mapped_const_iterator> neighbors(const key_type& key, const F& f) {
		static_assert(std::is_function_v<F>, "Custom comparision function must be a function type!\n");
		const auto check = check_key_exist(key);
		if(!check) {
			throw NodeNotFound("Check node was added to the container!\n");
		}
		auto& neighborList = graph_map.at(key);
		std::sort(neighborList.begin(), neighborList.end(), f);
		return {neighborList.cbegin(), neighborList.cend()};
	}
	std::pair<mapped_const_iterator, mapped_const_iterator> neighbors(const key_type& key, int policy) {
		const auto check = check_key_exist(key);
		if(!check) {
			throw NodeNotFound("Check node was added to the container!\n");
		}
		auto& neighborList = graph_map.at(key);
		if (policy == sort_policy::asc) {
			std::sort(neighborList.begin(), neighborList.end(), [](const auto& e1, const auto& e2) { return e1.first < e2.first; });
		} else if (policy == (sort_policy::asc | sort_policy::weight)) {
			std::sort(neighborList.begin(), neighborList.end(), [](const auto& e1, const auto& e2) { return e1.second < e2.second; });
		} else if (policy == sort_policy::desc) {
			std::sort(neighborList.begin(), neighborList.end(), [](const auto& e1, const auto& e2) { return e1.first > e2.first; });
		} else if (policy == (sort_policy::desc | sort_policy::weight)) {
			std::sort(neighborList.begin(), neighborList.end(), [](const auto& e1, const auto& e2) { return e1.second > e2.second; });
		}
		return {neighborList.cbegin(), neighborList.cend()};
	}
	bool remove_edge_directed(const key_type& key1, const key_type& key2) {
		const auto checkEdge = checkEdgeExists(key1, key2);
		if(!checkEdge) {
			throw EdgeNotFound("Edge not found, try adding edge first!\n");
		}
		auto& neighbors_key1 = graph_map.at(key1);
		const auto& search_key2_in_key1 = std::find_if(neighbors_key1.begin(), neighbors_key1.end(), [&](const value_type& element) { return element.first == key2; });
		if(search_key2_in_key1 != neighbors_key1.end()) {
			neighbors_key1.erase(search_key2_in_key1);
			--num_edges;
		}
		return true;
	}
	bool remove_edge_undirected(const key_type& key1, const key_type& key2) {
		const auto checkEdge = checkEdgeExists(key1, key2);
		if(!checkEdge) {
			throw EdgeNotFound("Edge not found, try adding edge first!\n");
		}
		auto& neighbors_key1 = graph_map[key1];
		auto& neighbors_key2 = graph_map[key2];
		const auto& search_key2_in_key1 = std::find_if(neighbors_key1.begin(), neighbors_key1.end(), [&](const value_type& element) { return element.first == key2; });
		const auto& search_key1_in_key2 = std::find_if(neighbors_key2.begin(), neighbors_key2.end(), [&](const value_type& element) { return element.first == key1; });
		if(search_key2_in_key1 != neighbors_key1.end() && search_key1_in_key2 != neighbors_key2.end()) {
			neighbors_key1.erase(search_key2_in_key1);
			neighbors_key2.erase(search_key1_in_key2);
			num_edges -= 2;
		}
		return true;
	}
	bool insert_edge_directed(const key_type& key1, const key_type& key2, const double w = 1.0) {
		const auto checkEdge = checkEdgeExists(key1, key2);
		if(checkEdge) {
			throw EdgeExists("Edge already exists, try removing first!\n");
		}
		graph_map.at(key1).push_back({key2, w});
		++num_edges;
		return true;
	}
	bool insert_edge_undirected(const key_type& key1, const key_type& key2, const double w = 1.0) {
		const auto checkEdge = checkEdgeExists(key1, key2);
		if(checkEdge) {
			throw EdgeExists("Edge already exists, try removing first!\n");
		}
		graph_map.at(key1).push_back({key2, w});
		graph_map.at(key2).push_back({key1, w});
		num_edges += 2;
		return true;
	}
	bool empty() const noexcept {
		return graph_map.empty();
	}
	void clear() noexcept {
		graph_map.clear();
	}
	size_type size() const noexcept{
		graph_map.size();
	}
};

#endif
