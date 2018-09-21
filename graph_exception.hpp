#ifndef GRAPH_EXCEPTION_HPP
#define GRAPH_EXCEPTION_HPP

struct NodeExists : public std::exception {
	std::string error;
	NodeExists(const std::string& s) : error(s) {}
	const char* what() const noexcept { return error.c_str(); }
};
struct EdgeExists : public std::exception {
	std::string error;
	EdgeExists(const std::string& s) : error(s) {}
	const char* what() const noexcept { return error.c_str(); }
};
struct EdgeNotFound : public std::exception {
	std::string error;
	EdgeNotFound(const std::string& s) : error(s) {}
	const char* what() const noexcept { return error.c_str(); }
};
struct NodeNotFound : public std::exception {
	std::string error;
	NodeNotFound(const std::string& s) : error(s) {}
	const char* what() const noexcept { return error.c_str(); }
};

#endif
