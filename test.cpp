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
    istringstream input("c comentario\np hola\nc comentario\n");
    EXPECT_EQ(filterComments(input).str(),"p hola\n");
}

TEST_F(GraphTest, BuildGraph) {
    istringstream input("p edge 9 14\ne 1 2\ne 1 3\ne 2 3\ne 2 4\ne 2 8\ne 2 9\ne 3 4\ne 4 6\ne 4 8\ne 4 9\ne 5 7\ne 6 7\ne 6 8\ne 7 9\n");
    Graph dummy(9);
    buildGraph(input, dummy);
    EXPECT_EQ(graph.adjacentsList, dummy.adjacentsList);
}

TEST(Graph, Adjacents) {
    Graph graph(3);
    graph.addEdge(0,1);
    graph.addEdge(0,2);
    vector< short > result = {1,2};
    vector< short > dummy = graph.getAdjacents(0);

    EXPECT_EQ(result, dummy);
}

TEST(Graph, ColorVertex) {
    Graph graph(3);
    graph.addEdge(0,1);
    graph.addEdge(0,2);

    vector< TNode > nodes;
    vector< short > colored(graph.nVertex, -1);

    for (int i = 0; i < graph.nVertex; ++i) {
        TNode n;
        n.degree = graph.adjacentsList[i].size();
        n.dsat = 0;
        n.vertex = i;
        nodes.push_back(n);
    }

    graph.colorVertex(0, 0, nodes, colored);

    vector< TNode > result;
    TNode n;
    n.vertex = 0;
    n.degree = 2;
    n.dsat = 0;
    result.push_back(n);
    n.vertex = 1;
    n.degree = 1;
    n.dsat = 1;
    result.push_back(n);
    n.vertex = 2;
    n.degree = 1;
    n.dsat = 1;
    result.push_back(n);

    EXPECT_EQ(result, nodes);
}

TEST(Graph, DSATUR_2V1E) {
    Graph graph(2);
    graph.addEdge(0,1);
    vector< short > rank = {1, 0};
    const tuple< short, vector<short>, vector<short> >& dummy = graph.dsatur();

    EXPECT_EQ(2, get<0>(dummy));
    EXPECT_EQ(rank, get<1>(dummy));
}

TEST(Graph, DSATUR_3V3E) {
    Graph graph(3);
    graph.addEdge(0,1);
    graph.addEdge(0,2);
    vector< short > rank = {0, 2, 1};
    const tuple< short, vector<short>, vector<short> >& dummy = graph.dsatur();

    EXPECT_EQ(2, get<0>(dummy));
    EXPECT_EQ(rank, get<1>(dummy));
}

TEST(Graph, DSATUR_2V0E) {
    Graph graph(2);
    vector< short > rank = {1, 0};
    const tuple< short, vector<short>, vector<short> >& dummy = graph.dsatur();

    EXPECT_EQ(1, get<0>(dummy));
    EXPECT_EQ(rank, get<1>(dummy));
}

TEST(Graph, DSATUR_4V5E) {
    Graph graph(4);
    graph.addEdge(0,1);
    graph.addEdge(0,2);
    graph.addEdge(1,2);
    graph.addEdge(1,3);
    graph.addEdge(2,3);
    vector< short > rank = {2, 1, 0, 3};
    vector< short > colored = {2, 1, 0, 2};
    const tuple< short, vector<short>, vector<short> >& dummy = graph.dsatur();

    EXPECT_EQ(3, get<0>(dummy));
    EXPECT_EQ(rank, get<1>(dummy));
    EXPECT_EQ(colored, get<2>(dummy));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); 
}
