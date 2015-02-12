#include "graph.hpp"
#include <algorithm> // sort, min, max
#include <iostream>  // cout, endl

std::bitset<MAX_NVERTEX> usable[MAX_NVERTEX];
short bestSolution; 

std::ostream& operator<<(std::ostream& out, const TVertex& v) {
    out << "<" << v.vertex() << ", "
        << v.dsat() << ", "
        << v.degree() << ">"; 
    return out; 
}

inline bool compareRanks(TVertex* lhs, TVertex* rhs) {
    return lhs->rank() < rhs->rank(); 
}

inline bool lessThanVertex(TVertex* lhs, TVertex* rhs) {

    if (lhs->dsat() != rhs->dsat()) {
        return lhs->dsat() < rhs->dsat();
    } else {
        return lhs->degree() < rhs->degree();
    }
}

TGraph::TGraph() {
    nVertex_ = 0; 
}

TGraph::~TGraph() {

    for (int i = 0; i < nVertex_; ++i) {
        delete vertices_[i];
    }
}

TGraph::TGraph(short nVertex) {
    nVertex_ = nVertex; 
    vertices_.reserve(nVertex); 
    adjList_.reserve(nVertex); 

    for (int i = 0; i < nVertex; ++i) {
        vertices_.push_back(new TVertex(i)); 
        adjList_.push_back(std::vector< TVertex* >()); 
    }
}

void TGraph::clear() {

    for (auto v : vertices_) {
        v->clear(); 
    }
}
void TGraph::addEdge(short first, short second) {

    if (first == second) { return; } 

    TVertex *fvertex = vertices_[first]; 
    TVertex *svertex = vertices_[second]; 

    adjList_[first].push_back(svertex); 
    adjList_[second].push_back(fvertex); 
}

std::vector< TVertex* > TGraph::adjacents(short vertex) {

    return adjList_[vertex]; 
}

std::vector< short >  TGraph::coloringRank() {
    std::vector< TVertex* > tmp(vertices_); 
    std::sort(tmp.begin(), tmp.end(), compareRanks); 

    std::vector< short > result; 

    for (auto v : tmp) {
       result.push_back(v->vertex()); 
    }

    return result; 
}

std::vector< short >  TGraph::coloringOrder() {
    std::vector< TVertex* > tmp(vertices_); 
    std::sort(tmp.begin(), tmp.end(), compareRanks); 

    std::vector< short > result; 

    for (auto v : tmp) {
       result.push_back(v->color()); 
    }

    return result; 
}


TVertex *TGraph::getVertexWithMaxDegree() {

    TVertex *current = vertices_[0]; 

    for (int i = 1; i < nVertex_; ++i) {
        if (vertices_[i]->color() == -1 && lessThanVertex(current,vertices_[i])) {

            current = vertices_[i]; 
        }     
    }

    if (current->color() != -1) {
        return NULL; 
    }

    return current; 
}

void TGraph::updateDsat(short vertex) {

    short color = vertices_[vertex]->color(); 
    for (auto adj : this->adjacents(vertex)) {
        if (adj->color() == -1 && !usable[adj->vertex()].test(color)) {

            usable[adj->vertex()].set(color);

            short tmpDsat = adj->dsat(); 
            ++tmpDsat;
            adj->dsat(tmpDsat); 
        }
    }
}

void TGraph::dsatur() {

    for (int i = 0; i < nVertex_; ++i) {
        short nEdges = this->adjacents(i).size(); 
        TVertex *node = vertices_[i];
        node->degree(nEdges); 

    }


    TVertex *current = this->getVertexWithMaxDegree(); 


    // Coloring vertex
    current->color(0); 
    this->updateDsat(current->vertex()); 

    current->rank(0); 

    short coloring_rank = 1; 
    short nColor = 1; 

    while(current != NULL) {

        current = this->getVertexWithMaxDegree(); 

        if (current == NULL) { break; }

        short use = 0;
        while(usable[current->vertex()].test(use)) { ++use; }

        nColor = std::max(nColor, (short) (use + 1));

        current->color(use); 
        this->updateDsat(current->vertex()); 
        current->rank(coloring_rank); 
        ++coloring_rank; 
    }

    bestSolution = nColor;
}


void TGraph::determineUsables(short vertex) {

    short nColor = std::min((short)(bestSolution - 2), vertices_[vertex]->lastNColors());
    usable[vertex].reset(); 
    // TODO: Can be improved to bitwise operations
    for (int i = 0; i < nColor; ++i) {
        usable[vertex].set(i); 
    }

    short currentRank = vertices_[vertex]->rank(); 
    for (auto adj: this->adjacents(vertex)) {
        short color = adj->color(); 
        if (adj->rank() < currentRank &&  usable[vertex].test(color)) {
            usable[vertex].reset(color); 
        }
    }
}

short TGraph::getMinimalColor(short vertex) {
    // min (uk + 1, q - 1) is the same as bestSolution - 2 and uk due 0-indexed colors
    short nColor = std::min((short)(bestSolution - 2), vertices_[vertex]->lastNColors());
    for (int i = 0; i <= nColor; ++i) {
        if (usable[vertex].test(i))  {
            return i;
        }
    }
    return -1;
}

void TGraph::determineCliqueSize(const std::vector< short >& colors) {
    clique_ = 1; 
    while(clique_ == colors[clique_]) { ++clique_; }
}

short TGraph::getMinimalQcolored(const std::vector< short >& coloring_rank) {
    short pos = 0; 

    while (pos < nVertex_) {
        TVertex *vertex = vertices_[coloring_rank[pos]];
        if (vertex->color() == bestSolution - 1) {
            break;
        }
        ++pos; 
    }

    return pos; 
}

void TGraph::procedureLabel(short vertex) {
    TVertex *current = vertices_[vertex]; 
    const std::vector< TVertex* > adjacents = this->adjacents(vertex); 
    for (auto adj : adjacents) {
        // If the adj is lower rank
        if (adj->rank() < current->rank()) {
            bool first = true; 
            for (auto other : adjacents) {
                // If the adj is the lowest rank among its color
                if (other->vertex() != adj->vertex() 
                        && other->rank() < adj->rank() 
                        && other->color() == adj->color()) {
                    first = false; 
                
                }
                if (first) {
                    adj->labeled(true); 
                }
            }
        }
    }
}
short TGraph::getMaxRankAmongLabeled(const std::vector < short >& coloring_rank) {
    short pos = nVertex_ - 1; 

    while (pos >= 0) {
        TVertex* vertex = vertices_[coloring_rank[pos]]; 
        if (vertex->labeled()) {
            break; 
        }
        --pos; 
    }

    return pos; 
}

short TGraph::ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm() {

    // Get upper bound q
    this->dsatur(); 


    for (int i = 0; i < nVertex_; ++i) {
        usable[i].reset(); 
    }

    const std::vector< short > coloring_rank  = this->coloringRank(); 
    const std::vector< short > coloring_order = this->coloringOrder(); 

    this->clear(); 

    // To get the w
    this->determineCliqueSize(coloring_order); 



    // If q == w
    if (clique_ == bestSolution) { 
        return bestSolution; 
    }

    // Color clique vertices of their color
    for (int i = 0; i < clique_; ++i) {
        short vertex = coloring_rank[i]; 
        vertices_[vertex]->color(i); 
        vertices_[vertex]->labeled(true); 
        vertices_[vertex]->lastNColors(i); 
    }

    bool back = false; 
    short k = clique_; 
    short last = k - 1;  

    while (true) {
        TVertex *vertex = vertices_[coloring_rank[k]];

        if (!back) {
            // Determine uk
            last = k - 1; 
            TVertex *lvertex = vertices_[coloring_rank[last]];

            short lastSolution = (lvertex->color() < lvertex->lastNColors() ? 
                    lvertex->lastNColors() : lvertex->lastNColors() + 1);

            vertex->lastNColors(lastSolution); 

            // Determine U(xk) 
            this->determineUsables(vertex->vertex()); 

        } else {
            // U(xk) = U(xk) - c and remove label from xk
            short color = vertex->color(); 
            usable[vertex->vertex()].reset(color); 
            vertex->labeled(false); 
            vertex->color(-1); 
        }

        // If U(xk) != {}
        if (usable[vertex->vertex()].count() > 0) {
            // Get minimal color, color vertex and k = k + 1
            short color = this->getMinimalColor(vertex->vertex()); 
            vertex->color(color); 
            ++k; 
            
            if (k > nVertex_ - 1) {
                bestSolution = vertex->lastNColors(); 
                if (bestSolution == clique_) {
                    return bestSolution;
                }

                // Get las q colored and remove all labels from Xk to Xn
                k = this->getMinimalQcolored(coloring_rank); 
                for (int i = k; i < nVertex_; ++i) {
                    vertices_[coloring_rank[i]]->labeled(false); 
                }

                back = true; 
            } else  {
                back = false; 
            }
        } else {
            back = true; 
        }

        if (back) {
            this->procedureLabel(coloring_rank[k]); 
            k = this->getMaxRankAmongLabeled(coloring_rank); 
            if (k <= clique_) {
                return bestSolution; 
            }

        }
    }

    return bestSolution; 
}
