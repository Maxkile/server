#ifndef ASVK_GRAPH_H
#define ASVK_GRAPH_SORT_H

#include <iostream>
#include <vector>
using namespace std;

struct Edge
{
    int src, dest,weight;
};

struct Graph
{
    // construct a vector of vectors to represent an adjacency list
    vector<vector<int>> adjList;

    // stores indegree of a vertex
    vector<int> indegree;

    // Graph Constructor
    Graph(vector<Edge> const &edges, int N);
};

bool doTopologicalSort(Graph const &graph, vector<int> &L, int N);

#endif //ASVK_GRAPH_H
