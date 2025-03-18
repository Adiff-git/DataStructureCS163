#ifndef GRAPH_H
#define GRAPH_H

#include "DataStructureLogic.h"
#include "raylib.h"
#include "Screen.h" // Include Screen.h
#include <vector>
#include <string>

class Graph : public DataStructureLogic {
private:
    struct Edge {
        int to;
        int weight;
        Edge* next;
        Edge(int t, int w) : to(t), weight(w), next(nullptr) {}
    };
    struct Vertex {
        int data;
        Edge* edges;
        Vertex() : data(-1), edges(nullptr) {}
        Vertex(int d) : data(d), edges(nullptr) {}
    };
    std::vector<Vertex> vertices;
    int size;
    Edge* CopyEdges(Edge* source);
    void* CopyState() override;
    void Draw(Font font, void* state, int x, int y) override;
    void ClearStates() override;

public:
    Graph(int size);
    ~Graph() override;
    void Initialize(int param) override;
    void Add(int value) override;
    void AddEdge(int from, int to, int weight);
    void Delete(int value) override;
    void Update(int oldValue, int newValue) override;
    bool Search(int value) override;
    void DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
};

#endif