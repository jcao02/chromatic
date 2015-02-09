#ifndef __CHROMATIC_HPP__
#define __CHROMATIC_HPP__
#include <sstream>
#include <vector>
#include <tuple>
#include <array>
#include <bitset>

extern std::bitset<10000> usable[10000];
extern std::vector< short > coloring_rank;
extern short bestSolution; 
extern short lastNColor;
extern short cliqueSize;

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
    void determineUsables(short vertex); 
    void determineCliqueSize(); 
    void clear(); 


    std::array< std::vector< short >, 10000 > adjacentsList;
    short nVertex_;
    std::vector< short > colored_;
};

std::istringstream *filterComments(std::istringstream& input);
void buildGraph(std::istringstream& input, Graph& graph);
void clear(); 
#endif
