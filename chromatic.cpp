#include <algorithm>                // sort, max
#include <sstream>                  // istringstream
#include <iostream>                 // cin, cout
#include <string>                   // string
#include <new>                      // bad_alloc
#include <queue>                    // queue
#include <string.h>
#include <fstream>
#include "chromatic.hpp"

using namespace std;

int main(int argc, char **argv) {

    std::istringstream *noComments; 
    if (argc < 2) {
        std::istream& is = cin; 
        noComments = filterComments(is); 
    } else {
        std::ifstream ifs(argv[1]); 
        std::istream& is = ifs; 
        noComments = filterComments(is); 
    }

    TGraph graph = buildGraph(*noComments); 

    cout << graph.ACorrectionToBrelazsModificationOfBrownsColoringAlgorithm(); 
    cout << endl;

    delete noComments; 
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

TGraph buildGraph(std::istringstream& input) {
    int nVertex, nEdges;
    int first, second;
    string line;
    char name[256];

    getline(input, line);
    sscanf(line.c_str(), "p %s %d %d", name, &nVertex, &nEdges);

    TGraph graph(nVertex); 

    while(getline(input, line)) {
        sscanf(line.c_str(), "e %d %d", &first, &second);
        first--;
        second--;

        graph.addEdge(first, second);
    }

    return graph; 
}
