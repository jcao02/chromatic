#include <algorithm>                // sort, max
#include <sstream>                  // istringstream
#include <iostream>                 // cin, cout
#include <string>                   // string
#include <new>                      // bad_alloc
#include <queue>                    // queue
#include "chromatic.hpp"

using namespace std;

bitset<MAX_NVERTEX> usable[MAX_NVERTEX];        // Usable color, given a node
vector< short > coloring_rank;                  // Colored coloring_rank
short cliqueSize;
bitset<MAX_NVERTEX> labels;                     // Labels on each node

vector< short > blocked_adjacents;              // Adjacent vertices that would be blocked

short vertex_rank[MAX_NVERTEX];                 // Reference from vertex position to rank position

short lastNColor[MAX_NVERTEX];
short bestSolution = MAX_NVERTEX;

TNode popMax(vector< TNode >& nodes);
void clearUsable();

int main() {
    Graph graph;

    buildGraph(*filterComments(cin), graph);

    bestSolution = graph.dsatur();
    graph.determineCliqueSize(); 

    cout << "dsatur: " << bestSolution << endl;
    cout << "clique: " << cliqueSize << endl;
    cout << graph.ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm(); 
    cout << endl;

    return 0;
}


istringstream *filterComments(istream& input) {
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
    vertex_rank[p.vertex] = coloring_rank.size();
    coloring_rank.push_back(p.vertex);

    while(! nodes.empty()) {

        const TNode& current = popMax(nodes); 


        short use = 0;
        while(usable[current.vertex].test(use)) { ++use; }

        nColor = max(nColor, (short) (use + 1));

        colorVertexDSATUR(current.vertex, use, nodes);

        vertex_rank[current.vertex] = coloring_rank.size();
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

pair<short,short> Graph::getBlockingsAndPreventions(short vertex, short color) {
    short preventions = 0, blockings = 0; 
    for (auto adj : this->getAdjacents(vertex)) {

        if (colored_[adj] == -1 && usable[adj].test(color)) {
            ++preventions;
            if (usable[adj].count() == 1) {
                ++blockings; 
                blocked_adjacents.push_back(adj);
            }
        }
    }
    return make_pair(preventions, blockings); 
}

void Graph::determineUsables(short current) {
    short vertex = coloring_rank[current];
    short lastVertex = coloring_rank[current - 1]; 

    lastNColor[vertex] = lastNColor[lastVertex];

    if (colored_[lastVertex] >= lastNColor[lastVertex]) {
        ++lastNColor[vertex];
    }

    // cout << "color[lastVertex]: " << colored_[lastVertex] << " | lastVertex: " << lastVertex << " | vertex " << vertex << " | lastNColor: " << lastNColor[lastVertex] << endl;

    short nColor = min(bestSolution - 1, lastNColor[vertex] + 1);


    usable[vertex].reset(); 
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

void Graph::removeOwnColor(short vertex) {
    short color = colored_[vertex]; 
    // cout << "in-remove " << vertex << " | " << color << endl; fflush(stdout);
    if (color >= 0) {
        usable[vertex].reset(color); 
    }
}

pair<short, bool> Graph::getBestColor(short vertex) {
    pair<short, short> min(MAX_NVERTEX, MAX_NVERTEX); 
    short color = -1;

    for (int i = 0; i < bestSolution - 1; ++i) {
        const pair<short, short>& pb = this->getBlockingsAndPreventions(vertex, i); 
        // cout << "afuera " << i << " <" << pb.second << "," << pb.first << ">" << endl;
        if (usable[vertex].test(i) && (pb.second < min.second || 
                (pb.second == min.second && pb.first < min.first)))  {
            min   = pb; 
            color = i; 
        }
    }

    return make_pair(color, min.second > 0); 
}

short Graph::getMinColor(short vertex) {
    for (int i = 0; i < bestSolution - 1; ++i) {
        // cout << "afuera " << i << " <" << pb.second << "," << pb.first << ">" << endl;
        if (usable[vertex].test(i))  {
            return i;
        }
    }

    return -1;
}

void Graph::label(short current) {

    short vertex = coloring_rank[current]; 
    const vector< short >& adjs = this->getAdjacents(vertex); 

    for (auto adj : adjs) {

        short pos = vertex_rank[adj];

        if (pos < current) {
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


short Graph::ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm() {
    bool back = false;

    short current = cliqueSize;     // First vertex to check
    short vertex;

    if (cliqueSize == bestSolution) {
        cout << "best1: ";
        return bestSolution;
    }

    // Update all the clique vertices before starting
    for (short i = 0; i < cliqueSize; ++i) {
        vertex = coloring_rank[i];
        // colored_[vertex] = i;
        labels.set(vertex);
        lastNColor[vertex] = i;
    }

    // DO FOREVER
    while (true) {
        // cout << "while\n"; fflush(stdout);
        vertex = coloring_rank[current];
        // cout << "vertex\n"; fflush(stdout);
        blocked_adjacents.clear();

        if (current % 7 == 0) {
            cout << bestSolution << " | " << current << " | " << usable[vertex].to_ulong() << " | " << nVertex_ << endl;
        }

        // IF not back
        if (! back) {
            // cout << "first-back\n"; fflush(stdout);
            this->determineUsables(current);
        } else {
            // cout << "first-back else\n"; fflush(stdout);
            this->removeOwnColor(vertex);
            // cout << "removeOwn\n"; fflush(stdout);
            labels.reset(vertex);
        }

        // cout << "vertex " << vertex << endl; fflush(stdout);
        // IF U(Xk) ≠ ø
        if (usable[vertex].count() > 0) {
            // cout << "entré\n"; fflush(stdout);
            short color = this->getMinColor(vertex);
            this->colorVertex(vertex, color);
            ++current;

            // IF k > n
            if (current > nVertex_ - 1) {
                bestSolution = lastNColor[vertex];

                // EXIT IF q = w
                if (bestSolution == cliqueSize) {
                    cout << "best2: ";
                    return bestSolution;
                }

                current = this->findBestSolutionAndRemoveLabels();
                back = true;
            } else {
                back = false;
            }
        } else {
            back = true;
        }

        // IF back
        if (back) {
            // cout << "out-block\n"; fflush(stdout);
            this->label(current);
            // cout << "label-current\n"; fflush(stdout);
            current = this->determineDeepestLabeled();
            // cout << "determined\n"; fflush(stdout);
            // cout << "new current: " << current << endl;

            if (current <= cliqueSize - 1) {
                cout << "best3: ";
                return bestSolution;
            }
        }
    }
}


void clearUsable() {
    for (auto& u : usable) { u.reset(); }
}

void clear() {
    clearUsable(); 
    coloring_rank.clear(); 
    labels.reset(); 
    memset(vertex_rank, 0, sizeof(short) * MAX_NVERTEX);
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
