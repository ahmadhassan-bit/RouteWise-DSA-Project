#include "Graph.h"
#include "Queue.h"
#include "Stack.h"
#include <iomanip>
#include <iostream>
using namespace std;

namespace {
const int INF = 1000000000;   // "no distance yet"
}

Graph::Graph()
    : names(new string[8]), adjList(new LinkedList<Edge>*[8]), matrix(new int[64]()),
      vertexCount(0), capacity(8), edgeCount(0) {}

Graph::~Graph() {
    for (int i = 0; i < vertexCount; i++) delete adjList[i];
    delete[] adjList;
    delete[] names;
    delete[] matrix;
}

// Doubles the capacity of the name array, the list array and the matrix.
void Graph::grow() {
    int newCapacity = capacity * 2;
    string* newNames = new string[newCapacity];
    LinkedList<Edge>** newLists = new LinkedList<Edge>*[newCapacity];
    int* newMatrix = new int[newCapacity * newCapacity]();

    for (int i = 0; i < vertexCount; i++) {
        newNames[i] = names[i];
        newLists[i] = adjList[i];
        for (int j = 0; j < vertexCount; j++) {
            newMatrix[i * newCapacity + j] = matrix[i * capacity + j];
        }
    }

    delete[] names;
    delete[] adjList;
    delete[] matrix;
    names = newNames;
    adjList = newLists;
    matrix = newMatrix;
    capacity = newCapacity;
}

int Graph::findLocation(const string& name) const {
    for (int i = 0; i < vertexCount; i++) {
        if (names[i] == name) return i;
    }
    return -1;
}

int Graph::addLocation(const string& name) {
    if (findLocation(name) != -1) return -1;
    if (vertexCount == capacity) grow();

    names[vertexCount] = name;
    adjList[vertexCount] = new LinkedList<Edge>();
    return vertexCount++;
}

void Graph::updateWeight(int from, int to, int weight) {
    for (ListNode<Edge>* node = adjList[from]->getHead(); node != nullptr; node = node->next) {
        if (node->data.to == to) node->data.weight = weight;
    }
}

bool Graph::addRoad(int a, int b, int weight) {
    if (a < 0 || b < 0 || a >= vertexCount || b >= vertexCount) return false;
    if (a == b || weight <= 0) return false;

    if (cell(a, b) == 0) {
        adjList[a]->pushBack(Edge{b, weight});
        adjList[b]->pushBack(Edge{a, weight});
        edgeCount++;
    } else {
        updateWeight(a, b, weight);
        updateWeight(b, a, weight);
    }
    cell(a, b) = weight;
    cell(b, a) = weight;
    return true;
}

void Graph::displayLocations() const {
    for (int i = 0; i < vertexCount; i++) {
        cout << "  " << i + 1 << ". " << names[i] << "\n";
    }
}

void Graph::display() const {
    if (vertexCount == 0) {
        cout << "The network has no locations yet.\n";
        return;
    }

    cout << "\nLocations: " << vertexCount << ", roads: " << edgeCount << "\n";
    displayLocations();

    cout << "\nAdjacency List:\n";
    for (int i = 0; i < vertexCount; i++) {
        cout << "  " << names[i] << ":";
        for (ListNode<Edge>* e = adjList[i]->getHead(); e != nullptr; e = e->next) {
            cout << "  -> " << names[e->data.to] << " (" << e->data.weight << " km)";
        }
        cout << "\n";
    }

    cout << "\nAdjacency Matrix (km, 0 = no road, numbers as listed above):\n";
    cout << setw(5) << " ";
    for (int j = 0; j < vertexCount; j++) cout << setw(5) << j + 1;
    cout << "\n";
    for (int i = 0; i < vertexCount; i++) {
        cout << setw(5) << i + 1;
        for (int j = 0; j < vertexCount; j++) cout << setw(5) << weightBetween(i, j);
        cout << "\n";
    }
}

// Breadth-first search: visits locations level by level using a queue.
int* Graph::bfs(int start, int& count) const {
    int* order = new int[vertexCount];
    bool* visited = new bool[vertexCount]();
    Queue<int> queue;

    count = 0;
    visited[start] = true;
    queue.enqueue(start);

    while (!queue.isEmpty()) {
        int current = queue.dequeue();
        order[count++] = current;
        for (ListNode<Edge>* e = adjList[current]->getHead(); e != nullptr; e = e->next) {
            if (!visited[e->data.to]) {
                visited[e->data.to] = true;
                queue.enqueue(e->data.to);
            }
        }
    }

    delete[] visited;
    return order;
}

// Depth-first search: goes as deep as possible first using a stack.
// Neighbours are pushed from last to first so the first neighbour is explored first.
int* Graph::dfs(int start, int& count) const {
    int* order = new int[vertexCount];
    bool* visited = new bool[vertexCount]();
    Stack<int> stack;

    count = 0;
    stack.push(start);

    while (!stack.isEmpty()) {
        int current = stack.pop();
        if (visited[current]) continue;
        visited[current] = true;
        order[count++] = current;
        for (ListNode<Edge>* e = adjList[current]->getTail(); e != nullptr; e = e->prev) {
            if (!visited[e->data.to]) stack.push(e->data.to);
        }
    }

    delete[] visited;
    return order;
}

bool Graph::isReachable(int source, int destination) const {
    int count;
    int* order = bfs(source, count);
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (order[i] == destination) found = true;
    }
    delete[] order;
    return found;
}

// Dijkstra's algorithm: repeatedly settles the closest unsettled location.
bool Graph::printShortestRoute(int source, int destination) const {
    int* distance = new int[vertexCount];
    int* previous = new int[vertexCount];
    bool* settled = new bool[vertexCount]();

    for (int i = 0; i < vertexCount; i++) {
        distance[i] = INF;
        previous[i] = -1;
    }
    distance[source] = 0;

    for (int round = 0; round < vertexCount; round++) {
        int current = -1;
        for (int i = 0; i < vertexCount; i++) {
            if (!settled[i] && distance[i] < INF && (current == -1 || distance[i] < distance[current])) {
                current = i;
            }
        }
        if (current == -1 || current == destination) break;

        settled[current] = true;
        for (ListNode<Edge>* e = adjList[current]->getHead(); e != nullptr; e = e->next) {
            int newDistance = distance[current] + e->data.weight;
            if (newDistance < distance[e->data.to]) {
                distance[e->data.to] = newDistance;
                previous[e->data.to] = current;
            }
        }
    }

    cout << "\nSource: " << names[source] << "\n";
    cout << "Destination: " << names[destination] << "\n";

    bool found = distance[destination] < INF;
    if (found) {
        // Walk back from the destination to build the route, then print it forwards.
        int* route = new int[vertexCount];
        int length = 0;
        for (int v = destination; v != -1; v = previous[v]) route[length++] = v;

        cout << "Recommended Route:\n";
        for (int i = length - 1; i >= 0; i--) {
            cout << names[route[i]] << (i > 0 ? " -> " : "\n");
        }
        cout << "Total Distance: " << distance[destination] << " km\n";
        delete[] route;
    } else {
        cout << "No route exists between these locations.\n";
    }

    delete[] distance;
    delete[] previous;
    delete[] settled;
    return found;
}

// Prim's algorithm on the adjacency matrix: grows a tree from location 1,
// always adding the cheapest road that connects a new location to the tree.
void Graph::printMinimumSpanningTree() const {
    if (vertexCount == 0) {
        cout << "The network has no locations yet.\n";
        return;
    }

    int* cheapest = new int[vertexCount];   // cheapest known road into the tree
    int* parent = new int[vertexCount];
    bool* inTree = new bool[vertexCount]();

    for (int i = 0; i < vertexCount; i++) {
        cheapest[i] = INF;
        parent[i] = -1;
    }
    cheapest[0] = 0;

    int totalCost = 0;
    int connected = 0;

    cout << "\nSelected connections:\n";
    for (int round = 0; round < vertexCount; round++) {
        int current = -1;
        for (int i = 0; i < vertexCount; i++) {
            if (!inTree[i] && cheapest[i] < INF && (current == -1 || cheapest[i] < cheapest[current])) {
                current = i;
            }
        }
        if (current == -1) break;   // the remaining locations cannot be reached

        inTree[current] = true;
        connected++;
        if (parent[current] != -1) {
            cout << "  " << names[parent[current]] << " -- " << names[current]
                 << " (" << cheapest[current] << " km)\n";
            totalCost += cheapest[current];
        }

        for (int v = 0; v < vertexCount; v++) {
            int road = weightBetween(current, v);
            if (!inTree[v] && road != 0 && road < cheapest[v]) {
                cheapest[v] = road;
                parent[v] = current;
            }
        }
    }

    cout << "Total network cost: " << totalCost << " km\n";
    if (connected < vertexCount) {
        cout << "Warning: the network is not fully connected. Only " << connected << " of "
             << vertexCount << " locations are linked to " << names[0] << ".\n";
    }

    delete[] cheapest;
    delete[] parent;
    delete[] inTree;
}
