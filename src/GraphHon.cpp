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

void generateCompleteGraph(std::vector<Edge>& edges, int numNodes) {
    edges.clear();
    for (int i = 1; i <= numNodes; ++i) {
        for (int j = i + 1; j <= numNodes; ++j) {
            edges.push_back({ i, j, 1 }); // Weight = 1 for simplicity
        }
    }
}

void generateCycleGraph(std::vector<Edge>& edges, int numNodes) {
    edges.clear();
    for (int i = 1; i < numNodes; ++i) {
        edges.push_back({ i, i + 1, 1 });
    }
    edges.push_back({ numNodes, 1, 1 });
}

void generatePathGraph(std::vector<Edge>& edges, int numNodes, std::vector<Vector2>& nodePositions, int screenWidth, int screenHeight) {
    edges.clear();
    nodePositions.resize(numNodes);

    float spacing = 150.0f; // Khoảng cách giữa các node
    float startX = (screenWidth - (numNodes - 1) * spacing) / 2.0f;
    float centerY = screenHeight / 2.0f;

    for (int i = 0; i < numNodes; ++i) {
        nodePositions[i] = { startX + i * spacing, centerY };
        if (i < numNodes - 1) {
            edges.push_back({ i + 1, i + 2, 1 });
        }
    }
}

void generateStarGraph(std::vector<Edge>& edges, int numNodes) {
    edges.clear();
    for (int i = 2; i <= numNodes; ++i) {
        edges.push_back({ 1, i, 1 }); // Node 1 is the center
    }
}

int main() {
    const int screenWidth = 1400;
    const int screenHeight = 1000;
    
    InitWindow(screenWidth, screenHeight, "Graph Creator");
    SetTargetFPS(60);

    Rectangle createButton = { 10, 10, 100, 30 };
    Rectangle exampleButton = { 10, 50, 100, 30 };
    
    int exampleNumNodes = 0; // Biến lưu trữ số lượng node cho đồ thị mẫu
    int exampleNumEdges = 0; // Biến lưu trữ số lượng edge cho đồ thị mẫu

    bool createButtonClicked = false;
    bool exampleButtonClicked = false;
    bool showExampleButtons = false; // Flag to show example graph buttons

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

   // Thay đổi vị trí và kích thước nút
   Rectangle k5Button = { exampleButton.x + exampleButton.width + 10, 50, 60, 30 }; // Complete Graph (K5)
   Rectangle c6Button = { k5Button.x + k5Button.width + 10, 50, 60, 30 }; // Cycle Graph (C6)
   Rectangle p4Button = { c6Button.x + c6Button.width + 10, 50, 60, 30 }; // Path Graph (P4)
   Rectangle s7Button = { p4Button.x + p4Button.width + 10, 50, 60, 30 }; // Star Graph (S7)

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

         // Khi nhấn nút "Create" hoặc "Example Graphs"
         if (CheckCollisionPointRec(GetMousePosition(), createButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            createButtonClicked = true;
            inputMode = true;
            graphDrawn = false;
            nodePositions.clear();
            canCreateGraph = true;
            showError = false;
            errorMessage = "";
            exampleNumNodes = 0; // Reset số node ví dụ
            exampleNumEdges = 0; // Reset số cạnh ví dụ
        }

        if (CheckCollisionPointRec(GetMousePosition(), exampleButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            exampleButtonClicked = true;
            showExampleButtons = !showExampleButtons;
            graphDrawn = false; // Xóa đồ thị khi nhấn "Example Graphs"
            nodePositions.clear();
            exampleNumNodes = 0; // Reset số node ví dụ
            exampleNumEdges = 0; // Reset số cạnh ví dụ
        }

        DrawRectangleRec(createButton, WHITE);
        DrawText("Create", createButton.x + (createButton.width - MeasureText("Create", 20)) / 2, createButton.y + 10, 20, BLACK);

        DrawRectangleRec(exampleButton, WHITE);
        DrawText("Examples", exampleButton.x + (exampleButton.width - MeasureText("Examples", 20)) / 2, exampleButton.y + 10, 20, BLACK);

        // Draw Example Graph Buttons if showExampleButtons is true
        if (showExampleButtons) {
            DrawRectangleRec(k5Button, WHITE);
            DrawText("K5", k5Button.x + 5, k5Button.y + 5, 20, BLACK);

            DrawRectangleRec(c6Button, WHITE);
            DrawText("C6", c6Button.x + 5, c6Button.y + 5, 20, BLACK);

            DrawRectangleRec(p4Button, WHITE);
            DrawText("P4", p4Button.x + 5, p4Button.y + 5, 20, BLACK);

            DrawRectangleRec(s7Button, WHITE);
            DrawText("S7", s7Button.x + 5, s7Button.y + 5, 20, BLACK);

            // Handle Example Graph Button Clicks
            if (CheckCollisionPointRec(GetMousePosition(), k5Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                generateCompleteGraph(edges, 5);
                nodePositions.resize(5);
                for (int i = 0; i < 5; ++i) {
                    float angle = 2.0f * PI * i / 5;
                    float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
                    nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
                }
                graphDrawn = true;
                showExampleButtons = false;
                inputMode = false;
                exampleNumNodes = 5; // Cập nhật số lượng node
                exampleNumEdges = 10; // Cập nhật số lượng edge (5 * (5 - 1) / 2)
                numNodesStr = ""; // Đặt lại numNodesStr
                numEdgesStr = ""; // Đặt lại numEdgesStr
            }

            if (CheckCollisionPointRec(GetMousePosition(), c6Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                generateCycleGraph(edges, 6);
                nodePositions.resize(6);
                for (int i = 0; i < 6; ++i) {
                    float angle = 2.0f * PI * i / 6;
                    float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
                    nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
                }
                graphDrawn = true;
                showExampleButtons = false;
                inputMode = false;
                exampleNumNodes = 6;
                exampleNumEdges = 6;
                numNodesStr = ""; // Đặt lại numNodesStr
                numEdgesStr = ""; // Đặt lại numEdgesStr
            }

            if (CheckCollisionPointRec(GetMousePosition(), p4Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                generatePathGraph(edges, 4, nodePositions, screenWidth, screenHeight);
                graphDrawn = true;
                showExampleButtons = false;
                inputMode = false;
                exampleNumNodes = 4;
                exampleNumEdges = 3;
                numNodesStr = ""; // Đặt lại numNodesStr
                numEdgesStr = ""; // Đặt lại numEdgesStr
            }

            if (CheckCollisionPointRec(GetMousePosition(), s7Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                generateStarGraph(edges, 7);
                nodePositions.resize(7);
                for (int i = 0; i < 7; ++i) {
                    float angle = 2.0f * PI * i / 7;
                    float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
                    nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
                }
                graphDrawn = true;
                showExampleButtons = false;
                inputMode = false;
                exampleNumNodes = 7;
                exampleNumEdges = 6;
                numNodesStr = ""; // Đặt lại numNodesStr
                numEdgesStr = ""; // Đặt lại numEdgesStr
            }
        }
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
       
        if (graphDrawn) {
            int numNodesToDraw;
            if (exampleNumNodes > 0) {
                numNodesToDraw = exampleNumNodes;
            } else if (numNodesStr != "") {
                numNodesToDraw = std::stoi(numNodesStr);
            } else {
                numNodesToDraw = 0; // Hoặc giá trị mặc định khác
            }
            int nodeRadius = 20;
            for (const auto& edge : edges) {
                Vector2 fromPos = nodePositions[edge.from - 1];
                Vector2 toPos = nodePositions[edge.to - 1];
                DrawLineV(fromPos, toPos, DARKBLUE);
                Vector2 weightPos = { fromPos.x + (toPos.x - fromPos.x) * 0.5f, fromPos.y + (toPos.y - fromPos.y) * 0.5f };
                DrawText(TextFormat("%d", edge.weight), weightPos.x, weightPos.y, 20, SKYBLUE);
            }

            for (int i = 0; i < numNodesToDraw; ++i) {
                DrawCircleV(nodePositions[i], nodeRadius, ORANGE);
                DrawCircleV(nodePositions[i], nodeRadius - 2, WHITE);
                DrawText(TextFormat("%d", i + 1), nodePositions[i].x - MeasureText(TextFormat("%d", i + 1), 20) / 2, nodePositions[i].y - 10, 20, DARKGRAY);
            }
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