#ifndef __CHROMATIC_HPP__
#define __CHROMATIC_HPP__
#include <sstream>
#include <vector>
#include <tuple>
#include <array>
#include <bitset>

extern std::bitset<10000> usable[10000];        // Usable color, given a node
extern std::vector< short > coloring_rank;               // Colored coloring_ranks
extern short bestSolution;                      // Upper bound to branch and bound
typedef struct {
    short degree, dsat, vertex;
} TNode;

TNode createTNode(short,short,short); 
std::ostream& operator<<(std::ostream& out, const TNode& node);

inline bool operator==(const TNode& lhs,const TNode& rhs) {
    return lhs.vertex == rhs.vertex && lhs.degree == rhs.degree 
                                    && lhs.dsat == rhs.dsat;
}

class Graph {
public:
    Graph(short nVertex);
    Graph(){}
    void addEdge(short first, short second);
    std::vector< short > getAdjacents(short vertex);
    short dsatur();
    bool colorVertexDSATUR(short vertex, short color, std::vector< TNode >& nodes);
    bool colorVertex(short vertex, short color); 
    std::pair<short,short> getBlockingsAndPreventions(short vertex, short color); 


    std::array< std::vector< short >, 10000 > adjacentsList;
    short nVertex_;
    std::vector< short > colored_;
};

std::istringstream *filterComments(std::istringstream& input);
void buildGraph(std::istringstream& input, Graph& graph);
void clear(); 
#endif
