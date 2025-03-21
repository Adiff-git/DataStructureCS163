#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

struct Edge {
    int from;
    int to;
    int weight;
};

bool edgeExists(const std::vector<Edge>& edges, int from, int to) {
    for (const auto& edge : edges) {
        if ((edge.from == from && edge.to == to) || (edge.from == to && edge.to == from)) {
            return true;
        }
    }
    return false;
}

// Hàm kiểm tra tính liên thông của đồ thị bằng DFS
bool isGraphConnected(const std::vector<Edge>& edges, int numNodes) {
    if (numNodes <= 1) return true; // Đồ thị có 0 hoặc 1 node luôn liên thông

    std::vector<bool> visited(numNodes, false);
    std::vector<int> stack;

    // Chọn node đầu tiên làm điểm bắt đầu DFS
    stack.push_back(0); // Chọn node 0 làm gốc (đã hiệu chỉnh để node bắt đầu từ 1)
    visited[0] = true;

    while (!stack.empty()) {
        int currentNode = stack.back();
        stack.pop_back();

        for (const auto& edge : edges) {
            int from = edge.from - 1; // Hiệu chỉnh chỉ số node
            int to = edge.to - 1;     // Hiệu chỉnh chỉ số node

            if (from == currentNode && !visited[to]) {
                visited[to] = true;
                stack.push_back(to);
            } else if (to == currentNode && !visited[from]) {
                visited[from] = true;
                stack.push_back(from);
            }
        }
    }

    // Kiểm tra xem tất cả các node đã được thăm hay chưa
    for (bool v : visited) {
        if (!v) return false;
    }

    return true;
}

int main() {
    const int screenWidth = 1400;
    const int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "Graph Creator");
    SetTargetFPS(60);

    Rectangle createButton = { 10, 10, 100, 30 };
    bool createButtonClicked = false;

    bool inputMode = false;
    std::string numNodesStr = "";
    std::string numEdgesStr = "";
    std::vector<Edge> edges;
    bool graphDrawn = false;
    std::vector<Vector2> nodePositions;

    Rectangle nodesInput = { createButton.x + createButton.width + 30, 10, 100, 30 };
    Rectangle edgesInput = { nodesInput.x + nodesInput.width + 30, 10, 100, 30 };
    bool nodesFocused = false;
    bool edgesFocused = false;
    bool canCreateGraph = true;
    bool showError = false;
    std::string errorMessage = "";

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (CheckCollisionPointRec(GetMousePosition(), createButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            createButtonClicked = true;
            inputMode = true;
            graphDrawn = false;
            nodePositions.clear();
            canCreateGraph = true;
            showError = false;
            errorMessage = "";
        }

        DrawRectangleRec(createButton, WHITE);
        DrawText("Create", createButton.x + (createButton.width - MeasureText("Create", 20)) / 2, createButton.y + 10, 20, BLACK);

        if (inputMode) {
            if (canCreateGraph) {
                DrawText("N:", nodesInput.x - 25, nodesInput.y + 5, 20, WHITE);
                DrawText("E:", edgesInput.x - 25, edgesInput.y + 5, 20, WHITE);

                DrawRectangleRec(nodesInput, LIGHTGRAY);
                DrawText(numNodesStr.c_str(), nodesInput.x + 5, nodesInput.y + 5, 20, BLACK);
                if (CheckCollisionPointRec(GetMousePosition(), nodesInput) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    nodesFocused = true;
                    edgesFocused = false;
                }
                if (nodesFocused) {
                    int key = GetCharPressed();
                    if (key >= 32 && key <= 125) {
                        numNodesStr += (char)key;
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && numNodesStr.length() > 0) {
                        numNodesStr.pop_back();
                    }
                }

                DrawRectangleRec(edgesInput, LIGHTGRAY);
                DrawText(numEdgesStr.c_str(), edgesInput.x + 5, edgesInput.y + 5, 20, BLACK);
                if (CheckCollisionPointRec(GetMousePosition(), edgesInput) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    edgesFocused = true;
                    nodesFocused = false;
                }
                if (edgesFocused) {
                    int key = GetCharPressed();
                    if (key >= 32 && key <= 125) {
                        numEdgesStr += (char)key;
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && numEdgesStr.length() > 0) {
                        numEdgesStr.pop_back();
                    }
                }
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (numNodesStr == "" || numEdgesStr == "") {
                    canCreateGraph = false;
                    showError = true;
                    errorMessage = "Vui lòng nhập số node và edge.";
                } else {
                    int numNodes = std::stoi(numNodesStr);
                    int numEdges = std::stoi(numEdgesStr);

                    // Kiểm tra khả năng tạo đồ thị
                    if (numNodes <= 0 || numEdges < 0 || numEdges > numNodes * (numNodes - 1) / 2 || numEdges < numNodes - 1) {
                    canCreateGraph = false;
                    showError = true;
                    errorMessage = "Can't create a graph";
                    } else {
                        // Thử tạo đồ thị
                        edges.clear();
                        srand(time(0));
                        while (edges.size() < numEdges) {
                            int from = GetRandomValue(1, numNodes);
                            int to = GetRandomValue(1, numNodes);
                            int weight = GetRandomValue(1, 10);
                            if (from != to && !edgeExists(edges, from, to)) {
                                edges.push_back({ from, to, weight });
                            }
                        }

                        // Kiểm tra tính liên thông
                        if (!isGraphConnected(edges, numNodes)) {
                            canCreateGraph = false;
                            showError = true;
                            errorMessage = "Không thể tạo graph liên thông với số liệu này.";
                        } else {
                            nodePositions.resize(numNodes);
                            int nodeRadius = 20;
                            float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
                            for (int i = 0; i < numNodes; ++i) {
                                float angle = 2.0f * PI * i / numNodes;
                                nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
                            }
                            graphDrawn = true;
                            inputMode = false;
                        }
                    }
                }
            }
        }

        if (showError && inputMode) {
            DrawText(errorMessage.c_str(), createButton.x + createButton.width + 30, createButton.y + 10, 20, RED);
        }

        if (graphDrawn && numNodesStr != "" && numEdgesStr != "") {
            int numNodes = std::stoi(numNodesStr);
            int nodeRadius = 20;
            for (const auto& edge : edges) {
                Vector2 fromPos = nodePositions[edge.from - 1];
                Vector2 toPos = nodePositions[edge.to - 1];
                DrawLineV(fromPos, toPos, DARKBLUE);
                Vector2 weightPos = {
                    fromPos.x + (toPos.x - fromPos.x) * 0.5f,
                    fromPos.y + (toPos.y - fromPos.y) * 0.5f
                };
                DrawText(TextFormat("%d", edge.weight), weightPos.x, weightPos.y, 20, SKYBLUE);
            }

            for (int i = 0; i < numNodes; ++i) {
                DrawCircleV(nodePositions[i], nodeRadius, ORANGE);
                DrawCircleV(nodePositions[i], nodeRadius - 2, WHITE);
                DrawText(TextFormat("%d", i + 1), nodePositions[i].x - MeasureText(TextFormat("%d", i + 1), 20) / 2, nodePositions[i].y - 10, 20, DARKGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}