#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <bitset>

#define MAX_NVERTEX 10000
extern std::bitset<MAX_NVERTEX> usable[MAX_NVERTEX];
extern short bestSolution;

#include <vector> 

class TVertex {
    public:
        TVertex(){}
        TVertex(short vertex) {
            vertex_      = vertex;
            color_       = -1;
            rank_        = -1;
            labeled_     = false;
            lastNColors_ = 0;
            dsat_        = 0;
            degree_      = 0;
        }

        void clear() {
            //color_       = -1; 
            //rank_        = -1;
            degree_      = -1;
            dsat_        = -1;
            labeled_     = false; 
            lastNColors_ = 0;
        }

        void color(short color) { color_ = color; }
        void rank(short rank) { rank_ = rank; }
        void degree(short degree) { degree_ = degree; }
        void dsat(short dsat) { dsat_ = dsat; }
        void labeled(bool labeled) { labeled_ = labeled; }
        void lastNColors(short last) { lastNColors_ = last; }

        short vertex() const { return vertex_; }
        short color() const { return color_; }
        short rank() const { return rank_; }
        short degree() const { return degree_; }
        short dsat() const { return dsat_; }
        bool labeled() { return labeled_; }
        short lastNColors() { return lastNColors_; }

        

       std::vector< short > prohibited_colors; 

    private:
        short vertex_, rank_, color_;
        short dsat_, degree_, lastNColors_;
        bool  labeled_; 


};



class TGraph {
    public:
        TGraph();
        TGraph(short nVertex);
        ~TGraph(); 
        
        // Getters 
        short nVertex() { return nVertex_; }
        short clique() { return clique_; }
        std::vector< TVertex* > adjacents(short vertex);
        std::vector< std::vector< TVertex* > > adjacents() { return adjList_; };
        TVertex* vertex(short pos) { return vertices_[pos]; }

        void addEdge(short first, short second); 
        void clear(); 
        std::vector <short> coloringRank(); 
        std::vector <short> coloringOrder(); 

        // DSATUR member functions
        void dsatur();
        void updateDsat(short vertex); 
        TVertex *getVertexWithMaxDegree(); 

        // Chromatic color member functions
        void determineUsables(short vertex); 
        void determineCliqueSize(const std::vector< short >& colors);
        short getMinimalColor(short vertex); 
        short getMinimalQcolored(const std::vector< short >& coloring_rank); 
        void procedureLabel(short vertex); 
        short getMaxRankAmongLabeled(const std::vector < short >& coloring_ranks);
        short ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm(); 

    private:
        short nVertex_; 
        short clique_; 
        std::vector<TVertex*> vertices_; 
        std::vector< std::vector< TVertex* > > adjList_; 
};
#endif
