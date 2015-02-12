#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include "graph.hpp"
#include "chromatic.hpp"

using namespace std;

class GraphTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        graph.addEdge(0,1);
        graph.addEdge(0,2);
        graph.addEdge(1,2);
        graph.addEdge(1,3);
        graph.addEdge(1,7);
        graph.addEdge(1,8);
        graph.addEdge(2,3);
        graph.addEdge(3,5);
        graph.addEdge(3,7);
        graph.addEdge(3,8);
        graph.addEdge(4,6);
        graph.addEdge(5,6);
        graph.addEdge(5,7);
        graph.addEdge(6,8);
    }

    static void TearDownTestCase() {}

    virtual void SetUp() {
        
        for (auto& u : usable) {
            u.reset(); 
        
        }

        for (int i = 0; i < graph.nVertex(); ++i) {
            graph.vertex(i)->color(-1); 
            graph.vertex(i)->rank(-1); 
        }
        graph.clear(); 
    
    }

    static TGraph graph;
};

TGraph GraphTest::graph(9);

TEST_F(GraphTest, DSATUR) {
    graph.dsatur(); 


    const vector< short > result         = {1, 3, 2, 7, 5, 8, 6, 0, 4};
    const vector< short > coloring_rank  = graph.coloringRank(); 
    const vector< short > coloring_order = graph.coloringOrder(); 
    const vector< short > colored        = {0, 1, 2, 2, 0, 2, 1, 1, 0};

    EXPECT_EQ(3, bestSolution);
    EXPECT_EQ(result, coloring_rank);
    EXPECT_EQ(colored, coloring_order);
}

TEST_F(GraphTest, TheSmallCorrection) {
    TGraph g(3);

    g.addEdge(0,1);
    g.addEdge(0,2);

    short dummy = g.ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm(); 

    EXPECT_EQ(2, dummy);
}

TEST_F(GraphTest, TheCorrection) {

    short dummy = graph.ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm(); 

    EXPECT_EQ(3, dummy);
}



TEST_F(GraphTest, IgnoreComment) {
    const string input_str = "c comment1\nc comment2\n"
        "p edge 9 14\ne 1 2\ne 1 3\ne 2 3\n"
        "e 2 4\ne 2 8\n e 2 9\ne 3 4\ne 4 6\n"
        "e 4 8\ne 4 9\ne 5 7\ne 6 7\ne 6 8\ne 7 9\n";
    const string result_str = "p edge 9 14\ne 1 2\ne 1 3\ne 2 3\n"
        "e 2 4\ne 2 8\n e 2 9\ne 3 4\ne 4 6\n"
        "e 4 8\ne 4 9\ne 5 7\ne 6 7\ne 6 8\ne 7 9\n";
    istringstream input(input_str);
    EXPECT_EQ((*filterComments(input)).str(),result_str);
}

TEST_F(GraphTest, BuildGraph) {
    const string input_str = "p edge 9 14\n" 
        "e 1 2\ne 1 3\ne 2 3\ne 2 4\ne 2 8\n" 
        "e 2 9\ne 3 4\ne 4 6\ne 4 8\ne 4 9\n" 
        "e 5 7\ne 6 7\ne 6 8\ne 7 9\n";
    istringstream input(input_str);

    TGraph dummy = buildGraph(input); 

    const vector< vector < TVertex* > > adjG = graph.adjacents(); 
    const vector< vector < TVertex* > > adjD = dummy.adjacents(); 
    
    for (int i = 0; i < 9; ++i) {
        TVertex *v1 = graph.vertex(i); 
        TVertex *v2 = dummy.vertex(i); 
        EXPECT_EQ(v1->vertex(), v2->vertex()); 

        ASSERT_EQ(adjG[v1->vertex()].size(), adjG[v2->vertex()].size()); 

        const vector< TVertex* >& adjV1 = adjG[v1->vertex()]; 
        const vector< TVertex* >& adjV2 = adjG[v2->vertex()]; 
        int N = adjV1.size(); 
        for (int j = 0; j < N; ++j) {
            EXPECT_EQ(adjV1[j]->vertex(), adjV2[j]->vertex()); 
        }
    }
}

TEST_F(GraphTest, determineUsables) {
    bestSolution = 5; 

    // Graph:
    // 0 -> 1 (colored 0)
    // 0 -> 2 (colored 1) 
    // 0 -> 3 (colored 0)
    // Expected result in usable[0] = 001
    
    TVertex* v = graph.vertex(1); 
    v->color(0); 
    v = graph.vertex(2); 
    v->color(1); 
    v = graph.vertex(3); 
    v->color(0); 

    v = graph.vertex(0); 
    v->lastNColors(3); 

    graph.determineUsables(0); 

    EXPECT_EQ(4, usable[0].to_ulong()); 
}

TEST_F(GraphTest, determineUsablesBig) {
    bestSolution = 7; 

    // Graph:
    // 1 -> 0 (colored 0)
    // 1 -> 2 (colored 1) 
    // 1 -> 3 (colored 1)
    // 1 -> 7 (colored 0)
    // 1 -> 8 (colored 2)
    // Expected result in usable[1] = 1000
    
    TVertex* v = graph.vertex(0); 
    v->color(0); 
    v = graph.vertex(2); 
    v->color(1); 
    v = graph.vertex(3); 
    v->color(1); 
    v = graph.vertex(7); 
    v->color(0); 
    v = graph.vertex(8); 
    v->color(2); 

    v = graph.vertex(1); 
    v->lastNColors(4); 

    graph.determineUsables(1); 

    EXPECT_EQ(8, usable[1].to_ulong()); 
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); 
}
