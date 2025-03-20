#include "raylib.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string.h>

struct Edge {
    int start, end, weight;
};

bool compareEdges(const Edge& a, const Edge& b) {
    return a.weight < b.weight;
}

int find(std::map<int, int>& parent, int i) {
    if (parent[i] == i)
        return i;
    return find(parent, parent[i]);
}

void unionSets(std::map<int, int>& parent, int i, int j) {
    int root_i = find(parent, i);
    int root_j = find(parent, j);
    parent[root_i] = root_j;
}

float distance(Vector2 p1, Vector2 p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "MST Visualization");

    std::vector<Vector2> vertices;
    std::vector<Edge> graph;
    std::vector<Edge> mst;
    std::map<int, int> parent;

    bool addingVertex = true;
    int selectedVertex = -1;

    int numNodes = 5;
    int numEdges = 8;
    char nodeBuffer[10] = "5";
    char edgeBuffer[10] = "8";

    Rectangle nodeInputRect = {10, 10, 50, 20};
    Rectangle edgeInputRect = {70, 10, 50, 20};
    Rectangle createButtonRect = {130, 10, 120, 20};

    bool nodeInputActive = false;
    bool edgeInputActive = false;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Input
        Vector2 mousePos = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, nodeInputRect)) {
                nodeInputActive = true;
                edgeInputActive = false;
            } else if (CheckCollisionPointRec(mousePos, edgeInputRect)) {
                edgeInputActive = true;
                nodeInputActive = false;
            } else if (CheckCollisionPointRec(mousePos, createButtonRect)) {
                vertices.clear();
                graph.clear();
                mst.clear();
                parent.clear();

                numNodes = std::atoi(nodeBuffer);
                numEdges = std::atoi(edgeBuffer);

                std::srand(std::time(0));
                for (int i = 0; i < numNodes; ++i) {
                    vertices.push_back({(float)(std::rand() % (screenWidth - 50) + 25), (float)(std::rand() % (screenHeight - 50) + 25)});
                }

                for (int i = 0; i < numEdges; ++i) {
                    int start = std::rand() % numNodes;
                    int end = std::rand() % numNodes;
                    int weight = std::rand() % 50 + 1;
                    if (start != end) {
                        graph.push_back({start, end, weight});
                    }
                }
            } else if (addingVertex) {
                vertices.push_back(mousePos);
            } else {
                int closestVertex = -1;
                float closestDist = 1000000.0f;
                for (int i = 0; i < vertices.size(); ++i) {
                    float dist = distance(mousePos, vertices[i]);
                    if (dist < 20.0f && dist < closestDist) {
                        closestVertex = i;
                        closestDist = dist;
                    }
                }
                if (closestVertex != -1) {
                    if (selectedVertex == -1) {
                        selectedVertex = closestVertex;
                    } else {
                        std::cout << "enter the weight of the Edge: ";
                        int weight = 0;
                        std::cin >> weight;
                        graph.push_back({selectedVertex, closestVertex, weight});
                        selectedVertex = -1;
                    }
                }
            }
        }

        if (nodeInputActive) {
            int key = GetCharPressed();
            if (key >= 32 && key <= 125 && strlen(nodeBuffer) < 9) {
                nodeBuffer[strlen(nodeBuffer)] = (char)key;
                nodeBuffer[strlen(nodeBuffer) + 1] = '\0';
            }
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(nodeBuffer) > 0) {
                nodeBuffer[strlen(nodeBuffer) - 1] = '\0';
            }
        }

        if (edgeInputActive) {
            int key = GetCharPressed();
            if (key >= 32 && key <= 125 && strlen(edgeBuffer) < 9) {
                edgeBuffer[strlen(edgeBuffer)] = (char)key;
                edgeBuffer[strlen(edgeBuffer) + 1] = '\0';
            }
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(edgeBuffer) > 0) {
                edgeBuffer[strlen(edgeBuffer) - 1] = '\0';
            }
        }

        // Draw UI
        DrawRectangleRec(nodeInputRect, LIGHTGRAY);
        DrawText(nodeBuffer, nodeInputRect.x + 5, nodeInputRect.y + 5, 10, BLACK);
        DrawRectangleRec(edgeInputRect, LIGHTGRAY);
        DrawText(edgeBuffer, edgeInputRect.x + 5, edgeInputRect.y + 5, 10, BLACK);
        DrawRectangleRec(createButtonRect, GREEN);
        DrawText("Create Random", createButtonRect.x + 5, createButtonRect.y + 5, 10, BLACK);

        // Draw graph
        for (const Edge& edge : graph) {
            DrawLineV(vertices[edge.start], vertices[edge.end], GRAY);
            Vector2 midPoint = {(vertices[edge.start].x + vertices[edge.end].x) / 2, (vertices[edge.start].y + vertices[edge.end].y) / 2};
            DrawText(std::to_string(edge.weight).c_str(), midPoint.x, midPoint.y, 20, DARKGRAY);
        }

        // Draw MST
        for (const Edge& edge : mst) {
            DrawLineV(vertices[edge.start], vertices[edge.end], RED);
        }

        // Draw vertices
        for (const Vector2& vertex : vertices) {
            DrawCircleV(vertex, 10, BLUE);
        }

        if (IsKeyPressed(KEY_E)) {
            addingVertex = false;
        }

        if (IsKeyPressed(KEY_V)) {
            addingVertex = true;
        }

        if (IsKeyPressed(KEY_R)) {
            graph.clear();
            mst.clear();
            vertices.clear();
            parent.clear();
            addingVertex = true;
            selectedVertex = -1;
        }

        // Kruskal
        if (IsKeyPressed(KEY_SPACE) && !graph.empty()) {
            mst.clear();
            parent.clear();
            for (int i = 0; i < vertices.size(); ++i) {
                parent[i] = i;
            }

            std::vector<Edge> sortedEdges = graph;
            std::sort(sortedEdges.begin(), sortedEdges.end(), compareEdges);

            for (const Edge& edge : sortedEdges) {
                if (find(parent, edge.start) != find(parent, edge.end)) {
                    mst.push_back(edge);
                    unionSets(parent, edge.start, edge.end);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}