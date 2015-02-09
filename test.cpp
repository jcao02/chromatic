#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
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

    static Graph graph;
};

Graph GraphTest::graph(9);

TEST(Graph, IgnoreComment) {
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
    Graph dummy;
    buildGraph(input, dummy);
    EXPECT_EQ(graph.adjacentsList, dummy.adjacentsList);
}

TEST_F(GraphTest, LoadGraph) {
    const string input_str = "c comment1\nc comment2\n"
        "p edge 9 14\ne 1 2\ne 1 3\ne 2 3\n"
        "e 2 4\ne 2 8\ne 2 9\ne 3 4\ne 4 6\n"
        "e 4 8\ne 4 9\ne 5 7\ne 6 7\ne 6 8\ne 7 9\n";
    istringstream input(input_str);

    Graph dummy; 
    buildGraph(*filterComments(input), dummy); 
    EXPECT_EQ(graph.adjacentsList, dummy.adjacentsList);

}

TEST_F(GraphTest, ColorVertex) {
    graph.colorVertex(0,0); 
    graph.colorVertex(7, 1); 

    EXPECT_FALSE(usable[1].test(0));
    EXPECT_FALSE(usable[1].test(1));
    EXPECT_FALSE(usable[2].test(0));
    EXPECT_FALSE(usable[3].test(1));
    EXPECT_FALSE(usable[5].test(1));

    EXPECT_EQ(0,graph.colored_[0]);
    EXPECT_EQ(1,graph.colored_[7]);
}

TEST_F(GraphTest, BlockingsAndPrevention) {
    // 2 prevention (for vertex 1 and vertex 0)
    // 1 blocking (for vertex 0)
    clear(); 

    usable[0].set(0); 
    usable[1].set(0); 
    usable[1].set(1); 
    usable[3].set(1); 

    const pair<short, short>& bp = graph.getBlockingsAndPreventions(2,0); 

    EXPECT_EQ(bp.first, 2);
    EXPECT_EQ(bp.second, 1);
}

TEST_F(GraphTest, DetermineUsable) {
    // Determine usables of vertex 3
    // Surrounded by:
    // 1 with color 0
    // 2 with color 1
    // 5 with color 0
    // 7 with color 1
    // 8 with color 1
    // lastNColor = 2
    // bestSolution = 6
    clear(); 
    graph.clear(); 

    graph.colored_[1] = 0; 
    graph.colored_[2] = 1; 
    graph.colored_[5] = 0; 
    graph.colored_[7] = 1; 
    graph.colored_[8] = 1; 

    lastNColor = 2;
    bestSolution = 6; 

    graph.determineUsables(3); 

    bitset<3> result(string("100")); 

    EXPECT_EQ(result.to_ulong(), usable[3].to_ulong()); 
}

TEST_F(GraphTest, DetermineUsable_All) {
    // Determine usables of vertex 3
    // lastNColor = 2
    // bestSolution = 6
    clear(); 
    graph.clear(); 

    lastNColor = 2;
    bestSolution = 6; 

    graph.determineUsables(3); 

    bitset<3> result(string("111")); 

    EXPECT_EQ(result.to_ulong(), usable[3].to_ulong()); 
}

TEST_F(GraphTest, DetermineUsable_None) {
    // Determine usables of vertex 3
    // Surrounded by:
    // 1 with color 0
    // 2 with color 0
    // 5 with color 0
    // 7 with color 0
    // 8 with color 0
    // lastNColor = 2
    // bestSolution = 2
    clear(); 
    graph.clear(); 

    graph.colored_[1] = 0; 
    graph.colored_[2] = 0; 
    graph.colored_[5] = 0; 
    graph.colored_[7] = 0; 
    graph.colored_[8] = 0; 

    lastNColor = 2;
    bestSolution = 2; 

    graph.determineUsables(3); 

    bitset<3> result(string("000")); 

    EXPECT_EQ(result.to_ulong(), usable[3].to_ulong()); 
}


TEST(Graph, Adjacents) {
    Graph graph(3);
    graph.addEdge(0,1);
    graph.addEdge(0,2);
    vector< short > result = {1,2};
    vector< short > dummy = graph.getAdjacents(0);

    EXPECT_EQ(result, dummy);
}

TEST(Graph, ColorVertexDSATUR_3V2E) {
    Graph graph(3);
    graph.addEdge(0,1);
    graph.addEdge(0,2);

    vector< TNode > nodes;
    vector< short > colored(graph.nVertex_, -1);

    for (int i = 0; i < graph.nVertex_; ++i) {
        TNode n;
        n.degree = graph.adjacentsList[i].size();
        n.dsat = 0;
        n.vertex = i;
        nodes.push_back(n);
    }

    graph.colorVertexDSATUR(0, 0, nodes);

    vector< TNode > result;
    result.push_back(createTNode(0,0,2));
    result.push_back(createTNode(1,1,1));
    result.push_back(createTNode(2,1,1));
    EXPECT_EQ(result, nodes);
}

TEST(Graph, ColorVertexDSATUR_4V5E) {
    Graph graph(4);
    graph.addEdge(0,1);
    graph.addEdge(0,2);
    graph.addEdge(1,2);
    graph.addEdge(1,3);
    graph.addEdge(2,3);

    clear(); 
    vector< TNode > nodes;
    vector< short > colored(graph.nVertex_, -1);

    for (int i = 0; i < graph.nVertex_; ++i) {
        TNode n;
        n.degree = graph.adjacentsList[i].size();
        n.dsat = 0;
        n.vertex = i;
        nodes.push_back(n);
    }

    graph.colorVertexDSATUR(1, 0, nodes);

    vector< TNode > result;

    result.push_back(createTNode(0,1,2));
    result.push_back(createTNode(1,0,3));
    result.push_back(createTNode(2,1,3));
    result.push_back(createTNode(3,1,2));
    EXPECT_EQ(result, nodes);
}



TEST(Graph, DSATUR_2V1E) {
    Graph graph(2);
    graph.addEdge(0,1);

    clear(); 

    vector< short > result = {0, 1};
    short dummy = graph.dsatur();

    EXPECT_EQ(2, dummy);
    EXPECT_EQ(result, coloring_rank);
}

TEST(Graph, DSATUR_3V2E) {
    Graph graph(3);
    graph.addEdge(0,1);
    graph.addEdge(0,2);

    clear(); 

    vector< short > result = {0, 1, 2};
    short dummy = graph.dsatur();

    EXPECT_EQ(2, dummy);
    EXPECT_EQ(result, coloring_rank);
}

TEST(Graph, DSATUR_2V0E) {
    Graph graph(2);

    clear(); 

    vector< short > result = {0, 1};
    short dummy = graph.dsatur();

    EXPECT_EQ(1, dummy);
    EXPECT_EQ(result, coloring_rank);
}

TEST(Graph, DSATUR_4V5E) {
    Graph graph(4);
    graph.addEdge(0,1);
    graph.addEdge(0,2);
    graph.addEdge(1,2);
    graph.addEdge(1,3);
    graph.addEdge(2,3);

    clear(); 

    vector< short > result = {1, 2, 0, 3};
    vector< short > colored = {2, 0, 1, 2};
    short dummy = graph.dsatur();

    EXPECT_EQ(3, dummy);
    EXPECT_EQ(result, coloring_rank);
    EXPECT_EQ(colored, graph.colored_);
}

TEST_F(GraphTest, DSATUR_test) {
    graph.clear(); 
    short dummy = graph.dsatur(); 


    const vector< short > result  = {1, 3, 2, 7, 5, 8, 6, 0, 4};
    const vector< short > colored = {1, 0, 2, 1, 0, 0, 1, 2, 2};

    EXPECT_EQ(3, dummy);
    EXPECT_EQ(result, coloring_rank);
    EXPECT_EQ(colored, graph.colored_);

}

TEST_F(GraphTest, DetermineCliqueSize) {
    graph.clear(); 
    graph.dsatur(); 

    graph.determineCliqueSize(); 
    EXPECT_EQ(3, cliqueSize); 
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); 
}
