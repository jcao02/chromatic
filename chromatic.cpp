#include <bitset>                   // bitset
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

inline bool operator<(const TNode& lhs, 
                      const TNode& rhs) {
    if (lhs.dsat != rhs.dsat) {
        return lhs.dsat < rhs.dsat;
    } else {
        return lhs.degree < rhs.degree;
    }
}

tuple<short, vector<short>, vector<short> > Graph::dsatur() {
    vector< TNode > nodes;
    vector< short > colored(nVertex, -1);
    vector< short > rank;
    short nColor = 1;

    for (auto& u : usable) { u.reset(); }

    rank.reserve(nVertex);
    nodes.reserve(nVertex);

    for (int i = 0; i < nVertex; ++i) {
        TNode n;
        n.degree = adjacentsList[i].size();
        n.dsat = 0;
        n.vertex = i;
        nodes.push_back(n);
    }

    sort(nodes.begin(), nodes.end());

    const TNode& p = nodes.back();
    nodes.pop_back();

    colorVertexDSATUR(p.vertex, 0, nodes, colored);
    rank.push_back(p.vertex);

    while(! nodes.empty()) {
        const TNode& current = nodes.back();
        nodes.pop_back();

        short use = 0;
        while(usable[current.vertex].test(use)) { ++use; }

        nColor = max(nColor, (short) (use + 1));

        colorVertexDSATUR(current.vertex, use, nodes, colored);
        rank.push_back(current.vertex);

        sort(nodes.begin(), nodes.end());
    }

    return make_tuple(nColor, rank, colored);
}

bool Graph::colorVertexDSATUR(short vertex, short color, vector< TNode >& nodes
                       , vector< short >& colored) {
    colored[vertex] = color;

    for (auto& adj : this->getAdjacents(vertex)) {
        if (! usable[adj].test(color)) {
            ++nodes[adj].dsat;
            usable[adj].set(color);
        }
    }

    return true;
}
