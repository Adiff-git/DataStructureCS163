#include "Graph.h"
#include <cmath>

#define PI 3.14159265359f

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
    for (auto& state : stepStates) {
        for (auto edge : state) {
            while (edge) {
                Edge* temp = edge;
                edge = edge->next;
                delete temp;
            }
        }
    }
}

void Graph::ClearSteps() {
    steps.clear();
    for (auto& state : stepStates) {
        for (auto edge : state) {
            while (edge) {
                Edge* temp = edge;
                edge = edge->next;
                delete temp;
            }
        }
    }
    stepStates.clear();
}

Edge* Graph::CopyEdges(Edge* source) {
    if (!source) return nullptr;
    Edge* newEdge = new Edge{source->to, source->weight, nullptr};
    newEdge->next = CopyEdges(source->next);
    return newEdge;
}

void Graph::SaveStep(const std::string& description) {
    steps.push_back(description);
    std::vector<Edge*> state;
    for (int i = 0; i < size; i++) {
        state.push_back(CopyEdges(vertices[i].edges));
    }
    stepStates.push_back(state);
}

void Graph::Initialize() {
    ClearSteps();
    for (int i = 0; i < size; i++) {
        while (vertices[i].edges) {
            Edge* temp = vertices[i].edges;
            vertices[i].edges = temp->next;
            delete temp;
        }
    }
    SaveStep("Initial state (empty)");
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
    SaveStep("Added edge " + std::to_string(from) + " -> " + std::to_string(to));
}

void Graph::DeleteEdge(int from, int to) {
    Edge* current = vertices[from].edges;
    if (!current) return;
    if (current->to == to) {
        vertices[from].edges = current->next;
        delete current;
        SaveStep("Deleted edge " + std::to_string(from) + " -> " + std::to_string(to));
        return;
    }
    while (current->next && current->next->to != to) current = current->next;
    if (current->next) {
        Edge* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted edge " + std::to_string(from) + " -> " + std::to_string(to));
    }
}

void Graph::UpdateEdge(int from, int to, int newWeight) {
    Edge* current = vertices[from].edges;
    bool updated = false;
    while (current) {
        if (current->to == to) {
            current->weight = newWeight;
            updated = true;
        }
        current = current->next;
    }
    if (updated) SaveStep("Updated edge " + std::to_string(from) + " -> " + std::to_string(to) + " to weight " + std::to_string(newWeight));
}

bool Graph::SearchEdge(int from, int to) {
    ClearSteps();
    Edge* current = vertices[from].edges;
    while (current) {
        SaveStep("Checking edge to " + std::to_string(current->to));
        if (current->to == to) {
            SaveStep("Found edge " + std::to_string(from) + " -> " + std::to_string(to));
            return true;
        }
        current = current->next;
    }
    SaveStep("Edge " + std::to_string(from) + " -> " + std::to_string(to) + " not found");
    return false;
}

void Graph::Draw(Font font, int x, int y, int step) {
    if (step < 0 || step >= stepStates.size()) return;
    const float radius = 200.0f;
    for (int i = 0; i < size; i++) {
        float angle = 2.0f * PI * i / size;
        int vx = static_cast<int>(x + radius * cos(angle));
        int vy = static_cast<int>(y + radius * sin(angle));
        DrawCircle(vx, vy, 25, LIGHTGRAY);
        DrawTextEx(font, TextFormat("%d", vertices[i].data), {static_cast<float>(vx - 10), static_cast<float>(vy - 10)}, 30, 1, DARKBLUE);

        Edge* edge = stepStates[step][i];
        while (edge) {
            float toAngle = 2.0f * PI * edge->to / size;
            int tx = static_cast<int>(x + radius * cos(toAngle));
            int ty = static_cast<int>(y + radius * sin(toAngle));
            DrawLine(vx, vy, tx, ty, BLACK);
            DrawTextEx(font, TextFormat("%d", edge->weight), {static_cast<float>((vx + tx) / 2), static_cast<float>((vy + ty) / 2)}, 20, 1, RED);
            edge = edge->next;
        }
    }
    DrawTextEx(font, steps[step].c_str(), {static_cast<float>(x), static_cast<float>(y + 300)}, 20, 1, BLACK);
}