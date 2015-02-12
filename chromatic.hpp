#ifndef __CHROMATIC_HPP__
#define __CHROMATIC_HPP__
#include <sstream>
#include "graph.hpp"

std::istringstream *filterComments(std::istream& input);
TGraph buildGraph(std::istringstream& input);
void clear(); 
#endif
