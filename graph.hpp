#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <vector> 



class TNode {
    public:
        TNode(){}
        TNode(short vertex) {
            vertex_ = vertex;
            color_  = -1;
            rank_   = -1;
        }

        void rank(short rank) { rank_ = rank; }
        void color(short color) { color_ = color; }
        short color() { return color_; }
        short rank() { return rank_; }

    private:
        short vertex_, rank_, color_; 

};

typedef struct {
    TNode first, second; 
} TEdge; 



class Graph {
    public:
        Graph();
        Graph(short nVertex);
        short nVertex() { return nVertex_; }
        std::vector< short > getAdjacents(short vertex);

    private:
        short nVertex_; 
        std::vector<TNode> nodes_; 
        std::vector< std::vector< TEdge > > adjList_; 
};
#endif

