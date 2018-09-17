#include "graph.hpp"
#include <math.h>

int main() {
	auto v = std::vector<std::string>{"Hello", "people", "how", "are", "you", "doing", "!"};
	Graph<std::string> g{v};
	for(const auto& s : v) {
		for(const auto& ss : v) {
			if(s != ss) {
				g.addEdge(s, ss, std::abs(static_cast<double>(s.size()-ss.size())));
			}
		}
	}
}
