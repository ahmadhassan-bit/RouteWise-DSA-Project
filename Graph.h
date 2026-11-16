#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include "LinkedList.h"

struct Edge {
    int to;
    int weight;   // distance in km
};

// Undirected weighted graph of delivery locations.
// The roads are stored twice: as adjacency lists (used by BFS, DFS and shortest path)
// and as an adjacency matrix (used by the minimum spanning tree).
class Graph {
private:
    std::string* names;
    LinkedList<Edge>** adjList;   // one list of roads per location
    int* matrix;                  // capacity x capacity, 0 means "no road"
    int vertexCount;
    int capacity;
    int edgeCount;

    void grow();
    int& cell(int row, int col) { return matrix[row * capacity + col]; }
    int weightBetween(int a, int b) const { return matrix[a * capacity + b]; }
    void updateWeight(int from, int to, int weight);

public:
    Graph();
    ~Graph();

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    int addLocation(const std::string& name);       // returns the new index, or -1 if it already exists
    int findLocation(const std::string& name) const; // returns -1 if not found
    bool addRoad(int a, int b, int weight);          // adds the road, or updates its distance

    int getLocationCount() const { return vertexCount; }
    int getRoadCount() const { return edgeCount; }
    std::string getName(int index) const { return names[index]; }

    void displayLocations() const;   // numbered list, starting at 1
    void display() const;            // adjacency list and adjacency matrix

    // Both traversals return a new[] array of visited locations; the caller must delete[] it.
    int* bfs(int start, int& count) const;
    int* dfs(int start, int& count) const;
    bool isReachable(int source, int destination) const;

    bool printShortestRoute(int source, int destination) const;   // Dijkstra
    void printMinimumSpanningTree() const;                        // Prim
};

#endif
