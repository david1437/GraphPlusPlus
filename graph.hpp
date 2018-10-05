#ifndef GRAPH_BASE_HPP
#define GRAPH_BASE_HPP

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
		container_type graph_map;
		std::unordered_map<Type, std::unordered_map<Type, double>> edge_map;
		size_type num_nodes;
		size_type num_edges;

		// helper functions
		const_iterator key_search(const key_type& node) const {
			if constexpr (std::is_same_v<container_type, std::map<Type,Mapped>>) {
				auto iter = std::lower_bound(std::begin(graph_map), std::end(graph_map), node, [](const std::pair<value_type,mapped_type>& v1, const value_type& v2) { return v1.first < v2; });
				return iter != std::end(graph_map) ? ++iter : iter;
			} else {
				return std::find_if(std::begin(graph_map), std::end(graph_map), [&](const std::pair<value_type, mapped_type>& m) { return m.first == node; });
			}
		};
		[[nodiscard]] bool key_exists(const key_type& node) const {
			return key_search(node) != std::end(graph_map);
		};
	public:
		// constructors
		template<class Iter>
		graph_base(const Iter& beg, const Iter& end) : num_nodes(0), num_edges(0) {
			for(auto iter = beg; iter != end; ++iter) {
				graph_map[*iter];
				++num_nodes;
			}
		};
		template<class T>
		graph_base(const std::initializer_list<T>& init_list) : num_nodes(0), num_edges(0) {
			for(auto iter = std::begin(init_list); iter != std::end(init_list); ++iter) {
				graph_map[*iter];
				++num_nodes;
			}
		};
		graph_base() : num_nodes(0), num_edges(0) {};

		// non modifying functions
		size_type node_count() const noexcept {
			return num_nodes;
		};
		size_type edge_count() const noexcept {
			return num_edges;
		};
		size_type size() const noexcept {
			graph_map.size();
		};

		// modifying functions
		void clear() {
			graph_map.clear();
		};
		void add_node(const key_type& node) {
			if(key_exists(node)) {
				// throw exception
				std::cout << "Key already exists!\n";
			} else {
				graph_map[node];
				++num_nodes;
			}
		};
		void add_node(key_type&& node) {
			if(key_exists(node)) {
				// throw exception
				std::cout << "Key already exists!\n";
			} else {
				graph_map[std::move(node)];
				++num_nodes;
			}
		};
};

#endif
