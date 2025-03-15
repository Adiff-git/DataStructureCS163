#include "Graph.h"
#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>

const float _PI = 3.14159265358979323846f;

Graph::Graph(int size) : size(size) {
    vertices.resize(size);
    for (int i = 0; i < size; i++) {
        vertices[i] = Vertex(i); // Sử dụng hàm tạo có tham số để gán giá trị
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
    ClearStates();
}

Graph::Edge* Graph::CopyEdges(Edge* source) {
    if (!source) return nullptr;
    Edge* newEdge = new Edge(source->to, source->weight);
    newEdge->next = CopyEdges(source->next);
    return newEdge;
}

void* Graph::CopyState() {
    std::vector<Edge*>* state = new std::vector<Edge*>;
    state->resize(size, nullptr);
    for (int i = 0; i < size; i++) {
        (*state)[i] = CopyEdges(vertices[i].edges);
    }
    return state;
}

void Graph::Draw(Font font, void* state, int x, int y) {
    std::vector<Edge*>* edgesState = static_cast<std::vector<Edge*>*>(state);
    const float radius = 200.0f;
    for (int i = 0; i < size; i++) {
        float angle = 2.0f * _PI * i / size;
        int vx = static_cast<int>(x + radius * cos(angle));
        int vy = static_cast<int>(y + radius * sin(angle));
        DrawCircle(vx, vy, 25, LIGHTGRAY);
        DrawTextEx(font, TextFormat("%d", vertices[i].data), {static_cast<float>(vx - 10), static_cast<float>(vy - 10)}, 30, 1, DARKBLUE);

        Edge* edge = (*edgesState)[i];
        while (edge) {
            float toAngle = 2.0f * _PI * edge->to / size;
            int tx = static_cast<int>(x + radius * cos(toAngle));
            int ty = static_cast<int>(y + radius * sin(toAngle));
            DrawLine(vx, vy, tx, ty, BLACK);
            DrawTextEx(font, TextFormat("%d", edge->weight), {static_cast<float>((vx + tx) / 2), static_cast<float>((vy + ty) / 2)}, 20, 1, RED);
            edge = edge->next;
        }
    }
}

void Graph::ClearStates() {
    for (auto state : stepStates) {
        std::vector<Edge*>* edgesState = static_cast<std::vector<Edge*>*>(state);
        for (auto edge : *edgesState) {
            while (edge) {
                Edge* temp = edge;
                edge = edge->next;
                delete temp;
            }
        }
        delete edgesState;
    }
}

void Graph::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    for (int i = 0; i < size; i++) {
        while (vertices[i].edges) {
            Edge* temp = vertices[i].edges;
            vertices[i].edges = temp->next;
            delete temp;
        }
    }
    SaveStep("Initial state (empty)", CopyState());
    AddEdge(0, 1, 4);
    AddEdge(1, 2, 5);
    AddEdge(2, 3, 6);
    AddEdge(3, 4, 7);
}

void Graph::Add(int value) {
    SaveStep("Add operation not supported for Graph", CopyState());
}

void Graph::AddEdge(int from, int to, int weight) {
    Edge* newEdge = new Edge(to, weight);
    if (!vertices[from].edges) vertices[from].edges = newEdge;
    else {
        Edge* current = vertices[from].edges;
        while (current->next) current = current->next;
        current->next = newEdge;
    }
    SaveStep("Added edge " + std::to_string(from) + " -> " + std::to_string(to), CopyState());
}

void Graph::Delete(int value) {
    for (int i = 0; i < size; i++) {
        Edge* current = vertices[i].edges;
        Edge* prev = nullptr;
        while (current) {
            if (current->to == value || i == value) {
                if (prev) {
                    prev->next = current->next;
                    delete current;
                    current = prev->next;
                } else {
                    vertices[i].edges = current->next;
                    delete current;
                    current = vertices[i].edges;
                }
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
    SaveStep("Deleted vertex " + std::to_string(value), CopyState());
}

void Graph::Update(int oldValue, int newValue) {
    for (int i = 0; i < size; i++) {
        Edge* current = vertices[i].edges;
        while (current) {
            if (current->to == oldValue) current->to = newValue;
            current = current->next;
        }
        if (i == oldValue) vertices[i].data = newValue;
    }
    SaveStep("Updated vertex " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
}

bool Graph::Search(int value) {
    ClearSteps();
    for (int i = 0; i < size; i++) {
        if (vertices[i].data == value) {
            SaveStep("Found vertex " + std::to_string(value), CopyState());
            return true;
        }
        Edge* current = vertices[i].edges;
        while (current) {
            if (current->to == value) {
                SaveStep("Found edge to " + std::to_string(value), CopyState());
                return true;
            }
            current = current->next;
        }
    }
    SaveStep("Vertex " + std::to_string(value) + " not found", CopyState());
    return false;
}