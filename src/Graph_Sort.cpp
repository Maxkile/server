#include <iostream>
#include <vector>
#include "Graph_Sort.h"
using namespace std;

// Graph Constructor
Graph::Graph(vector<Edge> const &edges, int N)
    {
        // resize the vector to N elements of type vector<int>
        adjList.resize(N);

        // initialize indegree
        vector<int> temp(N, 0);
        indegree = temp;

        // add edges to the directed graph
        for (auto &edge: edges)
        {
            // add an edge from source to destination
            adjList[edge.src].push_back(edge.dest);
            // increment in-degree of destination vertex by 1
            indegree[edge.dest]++;
        }
    }

bool doTopologicalSort(Graph const &graph, vector<int> &L, int N)
{
    vector<int> indegree = graph.indegree;

    // Set of all nodes with no incoming edges
    vector<int> S;
    for (int i = 0; i < N; i++) {
        if (!indegree[i]) {
            S.push_back(i);
        }
    }

    while (!S.empty())
    {
        // remove a node n from S
        int n = S.back();
        S.pop_back();

        // add n to tail of L
        L.push_back(n);

        for (int m : graph.adjList[n])
        {
            // remove edge from n to m from the graph
            indegree[m] -= 1;

            // if m has no other incoming edges then
            // insert m into S
            if (!indegree[m]) {
                S.push_back(m);
            }
        }
    }

    // if graph has edges then graph has at least one cycle
    for (int i = 0; i < N; i++) {
        if (indegree[i]) {
            return false;
        }
    }

    return true;
}