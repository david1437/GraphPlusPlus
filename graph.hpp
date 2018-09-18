#ifndef _GRAPH_HPP
#define _GRAPH_HPP

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <set>
#include <type_traits>
#include <utility>
#include <exception>

// TODO
// remove_node and remove_edge functions
// Add directed_graph and undirected_graph classes that inherit from graph_base
// Allow all graph types to be weighted
// Add type trait checks where necessary
// Add static_asserts
// Update exception class

class GraphError {
	private:	
		const std::string error;
	public:
		GraphError(const std::string& _error) : error(_error) {}
		const char* what() const noexcept {
			return error.c_str();
		}
};

template <class Type,
	  class Mapped = std::set<Type>,
	  class Container = std::map<Type, Mapped>>
struct graph_base
{
public:
	// std::map types
	using container_type  = Container;
	using key_type        = typename container_type::key_type;
	using mapped_type     = typename container_type::mapped_type;

	// std::set types
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

	container_type graph_map;
	size_type num_nodes;
	size_type num_edges;

	// iterator
	const_iterator cbegin() { return graph_map.cbegin(); }
	const_iterator cend() { return graph_map.cend(); }
	const_reverse_iterator crbegin() { return graph_map.crbegin(); }
	const_reverse_iterator crend() { return graph_map.crend(); }

	// helper functions
	bool check_key_exist(const key_type& key) {
		return graph_map.find(key) != graph_map.end();
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
		for(auto iter = l.begin(); iter != l.end(); ++iter) {
			insert_node(*iter);
		}
	}

	// functions
	size_type node_count() const noexcept { return num_nodes; }
	size_type edge_count() const noexcept { return num_edges; }
	bool insert_node(const key_type& key) {
		bool check = check_key_exist(key);
		if(check) {
			throw GraphError("Node already exists!");
		}
		graph_map[key];
		++num_nodes;
		return true;
	}
	bool insert_node(key_type&& key) {
		bool check = check_key_exist(key);
		if(check) {
			throw GraphError("Node already exists!");
		}
		graph_map[std::move(key)];
		++num_nodes;
		return true;
	}
	bool insert_edge(const key_type& key1, const key_type& key2) {
		bool check_key1 = check_key_exist(key1);
		bool check_key2 = check_key_exist(key2);
		if(!check_key1 || !check_key2) {
			throw GraphError("Node being used to build edge does not exist!");
		}
		if(!graph_map[key1].insert(key2).second || !graph_map[key2].insert(key1).second) {
			throw GraphError("Edge already exists between these nodes!");
		}
		num_edges += 2;
		return true;
	}
	std::pair<mapped_const_iterator, mapped_const_iterator> out_degrees(const key_type& key) {
		bool check = check_key_exist(key);
		if(!check) {
			throw GraphError("Node does not exist!");
		}
		const auto& neighbors = graph_map[key];
		return {neighbors.cbegin(), neighbors.cend()};
	}
	bool empty() const noexcept {
		return graph_map.empty();
	}
	void clear() noexcept {
		graph_map.clear();
	}
	size_type size() const noexcept {
		return graph_map.size();
	}
};

#endif
