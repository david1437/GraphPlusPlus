#ifndef GRAPH_TYPES_HPP
#define GRAPH_TYPES_HPP
		
const float NAN_TYPE = std::nanf("1");

template <class Type,
	  class Mapped,
	  class Container,
	  class Directed>
struct graph_base;

template <class Type,
	  class Mapped = std::set<Type>,
	  class Container = std::map<Type, Mapped>,
	  class Directed = std::false_type>
using graph = graph_base<Type, Mapped, Container, std::false_type>;

template <class Type,
	  class Mapped = std::set<Type>,
	  class Container = std::map<Type, Mapped>,
	  class Directed = std::false_type>
using directed_graph = graph_base<Type, Mapped, Container, std::true_type>;

#endif
