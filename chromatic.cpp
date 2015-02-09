#include <algorithm>                // sort, max
#include <sstream>                  // istringstream
#include <iostream>                 // cout
#include <string>                   // string
#include <new>                      // bad_alloc
#include <queue>                    // queue
#include <tuple>                    // tuple
#include "chromatic.hpp"

using namespace std;

bitset<10000> usable[10000];        // Usable color, given a node
bitset<10000> clique;               // Clique returned by the algorithm ran before
short cliqueSize;
bitset<10000> labels[10000];        // Labels on each node

short partialSolution[10000];
short currentNColors;
short bestSolution;

TNode popMax(vector< TNode >& nodes);

istringstream *filterComments(istringstream& input) {
    string line, result_str;
    while(getline(input, line)) {
        if (line[0] != 'c') {
            result_str += line + "\n";
        }
    }
    istringstream *result = new istringstream(result_str);
    return result;
}

void buildGraph(istringstream& input, Graph& graph) {
    int nVertex, nEdges;
    int first, second;
    string line;
    char name[256];

    getline(input, line);
    sscanf(line.c_str(), "p %s %d %d", name, &nVertex, &nEdges);

    graph = Graph(nVertex); 

    while(getline(input, line)) {
        sscanf(line.c_str(), "e %d %d", &first, &second);
        first--;
        second--;

        graph.addEdge(first, second);
    }
}

Graph::Graph(short nVertex) {
    this->nVertex = nVertex;
}

void Graph::addEdge(short first, short second) {
    if (first < nVertex && second < nVertex) {
        adjacentsList[first].push_back(second);
        adjacentsList[second].push_back(first);
    }
}

vector< short > Graph::getAdjacents(short vertex) {
    if (vertex < nVertex) {
        return adjacentsList[vertex];
    } else {
        return vector< short >();
    }
}

tuple<short, vector<short>, vector<short> > Graph::dsatur() {
    vector< TNode > nodes;
    vector< short > colored(nVertex, -1);
    vector< short > rank;
    short nColor = 1;


    rank.reserve(nVertex);
    nodes.reserve(nVertex);

    for (int i = 0; i < nVertex; ++i) {
        TNode n;
        n.degree = adjacentsList[i].size();
        n.dsat = 0;
        n.vertex = i;
        nodes.push_back(n);
    }


    const TNode& p = popMax(nodes); 

    colorVertexDSATUR(p.vertex, 0, nodes, colored);
    rank.push_back(p.vertex);

    while(! nodes.empty()) {

        const TNode& current = popMax(nodes); 


        short use = 0;
        while(usable[current.vertex].test(use)) { ++use; }

        nColor = max(nColor, (short) (use + 1));

        colorVertexDSATUR(current.vertex, use, nodes, colored);

        rank.push_back(current.vertex);
    }

    return make_tuple(nColor, rank, colored);
}

ostream& operator<<(ostream& out, const TNode& node) {
    out << "<" << node.vertex << ", " << node.dsat << ", " << node.degree << ">"; 
    return out; 
}

inline bool operator<(const TNode& lhs, 
                      const TNode& rhs) {
    if (lhs.dsat != rhs.dsat) {
        return lhs.dsat < rhs.dsat;
    } else {
        return lhs.degree < rhs.degree;
    }
}


bool Graph::colorVertexDSATUR(short vertex, short color, vector< TNode >& nodes
                       , vector< short >& colored) {
    colored[vertex] = color;

    for (auto& adj : this->getAdjacents(vertex)) {
        if (colored[adj] == -1 && !usable[adj].test(color)) {

            usable[adj].set(color);

            short adjPos = 0; 
            while (nodes[adjPos].vertex != adj) { ++adjPos; }
            ++nodes[adjPos].dsat;
        }
    }

    return true;
}

void clear() {
    for (auto& u : usable) { u.reset(); }
}

TNode popMax(vector< TNode >& nodes) {

    short nVertex = nodes.size(); 
    short elemPos = 0; 
    TNode maxNode = nodes[0];

    for (int i = 1; i < nVertex; ++i) {

        if (maxNode < nodes[i]) {

            maxNode = nodes[i]; 
            elemPos = i; 
        }     
    }

    nodes.erase(nodes.begin() + elemPos); 

    return maxNode; 
}

TNode createTNode(short vertex,short dsat,short degree) {
    TNode n;

    n.vertex = vertex;
    n.dsat   = dsat;
    n.degree = degree;
    
    return n; 
}
