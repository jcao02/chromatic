#include <algorithm>                // sort, max
#include <sstream>                  // istringstream
#include <iostream>                 // cout
#include <string>                   // string
#include <new>                      // bad_alloc
#include <queue>                    // queue
#include "chromatic.hpp"

using namespace std;

bitset<10000> usable[10000];        // Usable color, given a node
vector< short > coloring_rank;      // Colored coloring_ranks
short cliqueSize;
bitset<10000> labels;               // Labels on each node

short partialSolution[10000];
short currentNColors;
short bestSolution = 10000;
vector< short > lastNColor(10000, 0); 

TNode popMax(vector< TNode >& nodes);
void clearUsable();

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
    nVertex_ = nVertex;
    colored_ = vector< short >(nVertex, -1); 
}

void Graph::clear() {
    fill(colored_.begin(), colored_.end(), -1);
}

void Graph::addEdge(short first, short second) {
    if (first < nVertex_ && second < nVertex_) {
        adjacentsList[first].push_back(second);
        adjacentsList[second].push_back(first);
    }
}

vector< short > Graph::getAdjacents(short vertex) {
    if (vertex < nVertex_) {
        return adjacentsList[vertex];
    } else {
        return vector< short >();
    }
}

short Graph::dsatur() {
    vector< TNode > nodes;
    short nColor = 1;


    coloring_rank.reserve(nVertex_);
    nodes.reserve(nVertex_);

    for (int i = 0; i < nVertex_; ++i) {
        TNode n;
        n.degree = adjacentsList[i].size();
        n.dsat = 0;
        n.vertex = i;
        nodes.push_back(n);
    }


    const TNode& p = popMax(nodes); 

    colorVertexDSATUR(p.vertex, 0, nodes);
    coloring_rank.push_back(p.vertex);

    while(! nodes.empty()) {

        const TNode& current = popMax(nodes); 


        short use = 0;
        while(usable[current.vertex].test(use)) { ++use; }

        nColor = max(nColor, (short) (use + 1));

        colorVertexDSATUR(current.vertex, use, nodes);

        coloring_rank.push_back(current.vertex);
    }

    return nColor;
}

void Graph::determineCliqueSize() {
    cliqueSize = 1; 
    while(cliqueSize == colored_[coloring_rank[cliqueSize]]) { ++cliqueSize; }
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


bool Graph::colorVertexDSATUR(short vertex, short color, vector< TNode >& nodes) {
    colored_[vertex] = color;

    for (auto adj : this->getAdjacents(vertex)) {
        if (colored_[adj] == -1 && !usable[adj].test(color)) {

            usable[adj].set(color);

            short adjPos = 0; 
            while (nodes[adjPos].vertex != adj) { ++adjPos; }
            ++nodes[adjPos].dsat;
        }
    }

    return true;
}

bool Graph::colorVertex(short vertex, short color) {
    colored_[vertex] = color; 
    for (auto adj : this->getAdjacents(vertex)) {
        if (usable[adj].test(color)) {
            usable[adj].set(color, 0);
        }
    }

    return true;
}

// TODO: need to return blocked currents (try to make it k-indexed)
pair<short,short> Graph::getBlockingsAndPreventions(short vertex, short color) {
    short preventions = 0, blockings = 0; 
    for (auto adj : this->getAdjacents(vertex)) {

        if (colored_[adj] == -1 && usable[adj].test(color)) {
            ++preventions;
            if (usable[adj].count() == 1) {
                ++blockings; 
            }
        }
    }
    return make_pair(preventions, blockings); 
}

void Graph::determineUsables(short current) {
    short vertex = coloring_rank[current];
    short lastVertex = coloring_rank[current - 1]; 
    short nColor = min(bestSolution - 1, lastNColor[lastVertex] + 1);

    // TODO: Can be improved to bitwise operations
    for (int i = 0; i < nColor; ++i) {
        usable[vertex].set(i); 
    }

    for (auto adj: this->getAdjacents(vertex)) {
        short color = colored_[adj]; 
        if (color != -1) {
            usable[vertex].reset(color); 
        }
    }
}


// TODO : -> 
// Removes the current color of the vertex from usables for that vertex
void Graph::removeOwnColor(short vertex) {
    short color = colored_[vertex]; 
    usable[vertex].reset(color); 
}
// Gets the color with the less of blockings and preventions
short Graph::getBestColor(short vertex) {
    pair<short, short> min(10000, 10000); 
    short color = -1; 

    for (int i = 0; i < bestSolution - 1; ++i) {
        const pair<short, short>& pb =  this->getBlockingsAndPreventions(vertex, i); 
        if ((pb.second < min.second) || 
                (pb.second == min.second && pb.first < min.first))  {
            min   = pb; 
            color = i; 
        }
    }

    return color; 
}
// Labels the coloring_rank[current] according to the paper's constraints
void Graph::label(short current) {

    short vertex = coloring_rank[current]; 
    const vector< short >& adjs = this->getAdjacents(vertex); 

    for (auto adj : adjs) {

        auto it = find(coloring_rank.begin(), coloring_rank.begin() + current, adj);
        if (it != coloring_rank.begin() + current) {
            auto pos = distance(coloring_rank.begin(), it); 

            short color = colored_[adj]; 
            bool wasFirst = true; 
            for (; wasFirst && pos >= 0; --pos) {
                short tmpVertex = coloring_rank[pos]; 
                if (tmpVertex != adj && colored_[tmpVertex] == color && 
                        find(adjs.begin(), adjs.end(), tmpVertex) != adjs.end()) {
                    wasFirst = false; 
                }
            }

            if (wasFirst) {
                labels.set(adj); 
            }
        }
    }
}
short Graph::findBestSolutionAndRemoveLabels() {

    short pos = 0; 

    while (pos < nVertex_) {
        short vertex = coloring_rank[pos]; 
        if (colored_[vertex] == bestSolution - 1) {
            break;
        }
        ++pos; 
    }

    // Unlabel following vertices in ranks
    for (int i = pos; i < nVertex_; ++i) {
        short vertex = coloring_rank[i]; 
        if (labels.test(vertex)) {
            labels.reset(vertex); 
        }
    }

    return pos; 
}
// Determine the right-est vertex among the labeled vertices
short Graph::determineDeepestLabeled() {
    short pos = nVertex_ - 1; 

    while (pos >= 0) {
        short vertex = coloring_rank[pos]; 
        if (labels.test(vertex)) {
            break; 
        }
        --pos; 
    }

    return pos; 
}


void Graph::ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm() {

}


void clearUsable() {
    for (auto& u : usable) { u.reset(); }
}

void clear() {
    clearUsable(); 
    coloring_rank.clear(); 
    labels.reset(); 
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
