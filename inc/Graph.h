#ifndef GRAPH_H
#define GRAPH_H
#include "raylib.h"
#include <vector>

struct Edge {
    int to;
    int weight;
    Edge* next;
};

struct Vertex {
    int data;
    Edge* edges;
};

class Graph {
private:
    std::vector<Vertex> vertices;
    int size;
public:
    Graph(int size);
    ~Graph();
    void Initialize();
    void AddEdge(int from, int to, int weight);
    void DeleteEdge(int from, int to);
    void UpdateEdge(int from, int to, int newWeight);
    bool SearchEdge(int from, int to);
    void Draw(Font font, int x, int y);
};

#endif