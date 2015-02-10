#ifndef __CHROMATIC_HPP__
#define __CHROMATIC_HPP__
#include <sstream>
#include <vector>
#include <tuple>
#include <array>
#include <bitset>

#define MAX_NVERTEX 10000

extern std::bitset<MAX_NVERTEX> usable[MAX_NVERTEX];
extern std::bitset<MAX_NVERTEX> labels; 
extern std::vector< short > coloring_rank;
extern short bestSolution; 
extern short lastNColor[MAX_NVERTEX];
extern short cliqueSize;
extern short vertex_rank[MAX_NVERTEX];

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
    // Restores colors to -1 
    void clear(); 
    // Adds an edge to the graph
    void addEdge(short first, short second);
    // Get the adjacents list of vertex to vertex
    std::vector< short > getAdjacents(short vertex);

    // Functions for branch and bound
    // Vertex coloring for Branch and bound algorithm
    bool colorVertex(short vertex, short color); 
    // Get blockings and preventions for look-ahead
    std::pair<short,short> getBlockingsAndPreventions(short vertex, short color); 
    // Determine Usables (U(xk)) for vertex coloring_rank[current]
    void determineUsables(short current); 
    // Removes the current color of the vertex from usables for that vertex
    void removeOwnColor(short vertex); 
    // Gets the color with the less of blockings and preventions
    std::pair<short, bool> getBestColor(short vertex); 
    // Remove the labels from the first bestSolution-colored vertex and return said vertex
    short findBestSolutionAndRemoveLabels(); 
    // Labels the coloring_rank[current] according to the paper's constraints
    void label(short current); 
    // Determine the right-est vertex among the labeled vertices
    short determineDeepestLabeled(); 
    short ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm(); 
    

    // Functions for DSATUR
    // Colors the graph according to DSATUR algorithm
    short dsatur();
    // Vertex coloring for DSATUR algorithm
    bool colorVertexDSATUR(short vertex, short color, std::vector< TNode >& nodes);
    // Determine clique size from DSATUR coloring results
    void determineCliqueSize(); 


    std::array< std::vector< short >, MAX_NVERTEX > adjacentsList;
    short nVertex_;
    std::vector< short > colored_;
};

std::istringstream *filterComments(std::istream& input);
void buildGraph(std::istringstream& input, Graph& graph);
void clear(); 
#endif
