#include "Graph.h"
#include <cmath>

#define PI 3.14159265359f // Define PI as a float constant

Graph::Graph(int size) : size(size) {
    vertices.resize(size);
    for (int i = 0; i < size; i++) {
        vertices[i].data = i;
        vertices[i].edges = nullptr;
    }
}

Graph::~Graph() {
    for (int i = 0; i < size; i++) {
        Edge* current = vertices[i].edges;
        while (current) {
            Edge* temp = current;
            current = current->next;
            delete temp;
        }
    }
}

void Graph::Initialize() {
    AddEdge(0, 1, 4);
    AddEdge(1, 2, 5);
    AddEdge(2, 3, 6);
    AddEdge(3, 4, 7);
}

void Graph::AddEdge(int from, int to, int weight) {
    Edge* newEdge = new Edge{to, weight, nullptr};
    if (!vertices[from].edges) vertices[from].edges = newEdge;
    else {
        Edge* current = vertices[from].edges;
        while (current->next) current = current->next;
        current->next = newEdge;
    }
}

void Graph::DeleteEdge(int from, int to) {
    Edge* current = vertices[from].edges;
    if (!current) return;
    if (current->to == to) {
        vertices[from].edges = current->next;
        delete current;
        return;
    }
    while (current->next && current->next->to != to) current = current->next;
    if (current->next) {
        Edge* temp = current->next;
        current->next = temp->next;
        delete temp;
    }
}

void Graph::UpdateEdge(int from, int to, int newWeight) {
    Edge* current = vertices[from].edges;
    while (current) {
        if (current->to == to) current->weight = newWeight;
        current = current->next;
    }
}

bool Graph::SearchEdge(int from, int to) {
    Edge* current = vertices[from].edges;
    while (current) {
        if (current->to == to) return true;
        current = current->next;
    }
    return false;
}

void Graph::Draw(Font font, int x, int y) {
    const float radius = 200.0f;
    for (int i = 0; i < size; i++) {
        float angle = 2.0f * PI * i / size;
        int vx = static_cast<int>(x + radius * cos(angle));
        int vy = static_cast<int>(y + radius * sin(angle));
        DrawCircle(vx, vy, 25, LIGHTGRAY);
        DrawTextEx(font, TextFormat("%d", vertices[i].data), {static_cast<float>(vx - 10), static_cast<float>(vy - 10)}, 30, 1, DARKBLUE);

        Edge* edge = vertices[i].edges;
        while (edge) {
            float toAngle = 2.0f * PI * edge->to / size;
            int tx = static_cast<int>(x + radius * cos(toAngle));
            int ty = static_cast<int>(y + radius * sin(toAngle));
            DrawLine(vx, vy, tx, ty, BLACK);
            DrawTextEx(font, TextFormat("%d", edge->weight), {static_cast<float>((vx + tx) / 2), static_cast<float>((vy + ty) / 2)}, 20, 1, RED);
            edge = edge->next;
        }
    }
}