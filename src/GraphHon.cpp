#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>

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

void generateRandomGraph(std::vector<Edge>& edges, int numNodes, int numEdges) {
    edges.clear();
    srand(time(0));

    // Tạo một đồ thị đường (path graph) để đảm bảo tính liên thông cơ bản
    for (int i = 1; i < numNodes; ++i) {
        edges.push_back({ i, i + 1, GetRandomValue(1, 10) });
    }

    // Thêm các cạnh ngẫu nhiên cho đến khi đạt đủ số lượng cạnh
    while (edges.size() < numEdges && edges.size() < (long unsigned int) (numNodes * (numNodes - 1) / 2)) {
        int from = GetRandomValue(1, numNodes);
        int to = GetRandomValue(1, numNodes);
        int weight = GetRandomValue(1, 10);
        if (from != to && !edgeExists(edges, from, to)) {
            edges.push_back({ from, to, weight });
        }
    }
    // kiểm tra số cạnh tạo ra để tránh vòng lặp vô hạn.
    // kiểm tra tính liên thông sau khi thêm đủ số edge.
    if (!isGraphConnected(edges,numNodes)){
        edges.clear();
        for (int i = 1; i < numNodes; ++i) {
            edges.push_back({ i, i + 1, GetRandomValue(1, 10) });
        }
        for (int i = 1; i < numNodes /2; ++i) {
             edges.push_back({ 1, i+2 , GetRandomValue(1, 10) });
        }
    }
}

// Hàm để lấy vị trí index trong chuỗi dựa trên dòng và cột
int GetCursorIndex(const std::string& text, int row, int col) {
    int currentRow = 0;
    int currentCol = 0;
    for (int i = 0; i < text.length(); ++i) {
        if (currentRow == row && currentCol == col) {
            return i;
        }
        if (text[i] == '\n') {
            currentRow++;
            currentCol = 0;
        } else {
            currentCol++;
        }
    }
    return text.length(); // Nếu vị trí vượt quá độ dài chuỗi
}

// Hàm lấy cột từ vị trí index trong chuỗi
int GetColFromIndex(const std::string& text, int index) {
    int col = 0;
    for (int i = index-1; i >=0; --i) {
        if (text[i] == '\n') {
            break;
        }
        col++;
    }
    return col;
}

// Hàm tính toán vị trí con trỏ trên màn hình dựa trên dòng và cột
Vector2 GetCursorScreenPos(const std::string& text, int row, int col, Rectangle rect) {
    int currentRow = 0;
    int currentCol = 0;
    int x = rect.x + 5;
    int y = rect.y + 5;
    for (int i = 0; i < text.length(); ++i) {
        if (currentRow == row && currentCol == col) {
            return {static_cast<float>(x), static_cast<float>(y)};
        }
        if (text[i] == '\n') {
            y += 20;
            x = rect.x + 5;
            currentRow++;
            currentCol = 0;
        } else {
            x += MeasureText(text.substr(i, 1).c_str(), 20);
            currentCol++;
        }
    }
    return {static_cast<float>(x), static_cast<float>(y)};
}

std::string ValidateMatrixInput(const std::string& input, std::vector<std::vector<int>>& adjacencyMatrix, int& numNodes) {
    adjacencyMatrix.clear();
    std::stringstream ss(input);
    std::string line;

    // Đọc số node từ dòng đầu tiên
    if (!std::getline(ss, line)) {
        return "Lỗi: Không có dữ liệu đầu vào.";
    }
    try {
        numNodes = std::stoi(line);
    } catch (const std::invalid_argument& e) {
        return "Lỗi: Dòng đầu tiên không phải là số hợp lệ.";
    }

    if (numNodes <= 0) {
        return "Lỗi: Số node phải lớn hơn 0.";
    }

    int rowCount = 0;
    int colCount = -1;

    // Đọc ma trận từ các dòng còn lại
    while (std::getline(ss, line)) {
        std::vector<int> row;
        std::stringstream lineStream(line);
        int value;
        char comma;
        int currentRowColCount = 0;
        while (lineStream >> value) {
            row.push_back(value);
            if (lineStream.peek() == ',' || lineStream.peek() == ' ') {
                lineStream >> comma;
            }
            currentRowColCount++;
        }
        if (colCount != -1 && colCount != currentRowColCount) {
            return "Lỗi: Số cột không đồng nhất ở dòng " + std::to_string(rowCount + 2) + "."; // Cộng 2 vì dòng đầu tiên là số node
        }
        colCount = currentRowColCount;
        adjacencyMatrix.push_back(row);
        rowCount++;
    }

    // Kiểm tra ma trận vuông
    if (rowCount != numNodes || colCount != numNodes) {
        return "Lỗi: Ma trận không vuông hoặc kích thước không khớp với số node.";
    }

    return ""; // Trả về chuỗi rỗng nếu ma trận hợp lệ
}

void ResetStates(std::vector<Edge>& edges, std::vector<Vector2>& nodePositions,
    bool& isCreating, bool& isRandomizing, bool& isShowingExamples, bool& showMatrixInput,
    bool& graphDrawn, std::string& numNodesStr, std::string& numEdgesStr,
    std::string& matrixInput, bool& nodesFocused, bool& edgesFocused) {
edges.clear();
nodePositions.clear();
isCreating = false;
isRandomizing = false;
isShowingExamples = false;
showMatrixInput = false;
graphDrawn = false;
numNodesStr = "";
numEdgesStr = "";
matrixInput = " ";
nodesFocused = false;
edgesFocused = false;
}

int main() {
    const int screenWidth = 1400;
    const int screenHeight = 1000;
    
    InitWindow(screenWidth, screenHeight, "Graph Creator");
    SetTargetFPS(60);

    Rectangle createButton = {10, 10, 100, 30};
    Rectangle randomButton = {10, createButton.y + createButton.height + 10, 100, 30};
    Rectangle exampleButton = {10, randomButton.y + randomButton.height + 10, 100, 30};
    Rectangle inputButton = {10, exampleButton.y + exampleButton.height + 10, 100, 30};
    Rectangle fileButton = {10, inputButton.y + inputButton.height + 10, 100, 30}; // Nút File

    int exampleNumNodes = 0; // Biến lưu trữ số lượng node cho đồ thị mẫu
    int exampleNumEdges = 0; // Biến lưu trữ số lượng edge cho đồ thị mẫu

    bool createButtonClicked = false;
    bool exampleButtonClicked = false;
    bool showExampleButtons = false; // Flag to show example graph buttons
    bool randomButtonClicked = false; // Biến flag cho nút random

    // Các biến cờ trạng thái cho từng chức năng
    bool isCreating = false;
    bool isRandomizing = false;
    bool isShowingExamples = false;

    bool inputMode = false;
    std::string numNodesStr = "";
    std::string numEdgesStr = "";
    std::string errorMessage = "";
    std::vector<Edge> edges;
    bool graphDrawn = false;
    std::vector<Vector2> nodePositions;

    Rectangle nodesInput = { createButton.x + createButton.width + 30, 10, 100, 30 };
    Rectangle edgesInput = { nodesInput.x + nodesInput.width + 30, 10, 100, 30 };
    bool nodesFocused = false;
    bool edgesFocused = false;
    bool canCreateGraph = true;
    bool showError = false;

   // Thay đổi vị trí và kích thước nút
   Rectangle k5Button = { randomButton.x + randomButton.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Complete Graph (K5)
   Rectangle c6Button = { k5Button.x + k5Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Cycle Graph (C6)
   Rectangle p4Button = { c6Button.x + c6Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Path Graph (P4)
   Rectangle s7Button = { p4Button.x + p4Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Star Graph (S7)
  
   // Input
   bool matrixInputFocused = false;
   bool showMatrixInput = false;
   std::string matrixInput = " ";
   int cursorColumn = 0;
   int cursorRow = 0; 
   float cursorTimer = 0.0f; // Thời gian nhấp nháy con trỏ
   std::string errorMessageInput = ""; // Biến để lưu thông báo lỗi

   Rectangle inputWindow = {screenWidth / 4, screenHeight / 4, screenWidth / 2, screenHeight / 2};
   Rectangle matrixInputRect = {inputWindow.x + 10, inputWindow.y + 40, inputWindow.width - 20, inputWindow.height - 90};
   Rectangle submitButton = {inputWindow.x + 10, inputWindow.y + inputWindow.height - 40, 80, 30}; // Nút Submit
   Rectangle closeButton = {submitButton.x + submitButton.width + 10, submitButton.y, 80, 30}; // Nút Back

    while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GRAY);

    if (isCreating) {
        // Logic cho chức năng Create
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
            //logic ve do thi khi nhan enter
             if (numNodesStr == "" || numEdgesStr == "") {
                canCreateGraph = false;
                showError = true;
                errorMessage = "Vui lòng nhập số node và edge.";
            } else {
                int numNodes = std::stoi(numNodesStr);
                int numEdges = std::stoi(numEdgesStr);

                if (numNodes <= 0 || numEdges < 0 || numEdges > numNodes * (numNodes - 1) / 2 || numEdges < numNodes - 1) {
                    canCreateGraph = false;
                    showError = true;
                    errorMessage = "Can't create a graph";
                } else {
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
                        isCreating = false;
                        numNodesStr = "";
                        numEdgesStr = "";
                    }
                }
            }
        }
         if (showError && isCreating) {
            DrawText(errorMessage.c_str(), createButton.x + createButton.width + 30, createButton.y + 10, 20, RED);
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&
               !CheckCollisionPointRec(GetMousePosition(),nodesInput) &&
               !CheckCollisionPointRec(GetMousePosition(),edgesInput)&&
               !CheckCollisionPointRec(GetMousePosition(), createButton) &&
                !CheckCollisionPointRec(GetMousePosition(),randomButton)&&
                !CheckCollisionPointRec(GetMousePosition(), exampleButton)&&
                !CheckCollisionPointRec(GetMousePosition(), inputButton))
            {
               isCreating = false;
               graphDrawn = false;
               numNodesStr = "";
               numEdgesStr = "";
               edges.clear();
               nodePositions.clear();
            }
    } else if (isRandomizing) {
        // Logic cho chức năng Random
        edges.clear();
        nodePositions.clear();
        int numNodes = GetRandomValue(2, 7);
        int numEdges = GetRandomValue(numNodes - 1, numNodes * (numNodes - 1) / 2);
        generateRandomGraph(edges, numNodes, numEdges);

        nodePositions.resize(numNodes);
        for (int i = 0; i < numNodes; ++i) {
            float angle = 2.0f * PI * i / numNodes;
            float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
            nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
        }
        graphDrawn = true;
        isRandomizing = false;
        numNodesStr = "";
        numEdgesStr = "";
    }

        if (CheckCollisionPointRec(GetMousePosition(), createButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            createButtonClicked = true;
            inputMode = true;
            graphDrawn = false;
            nodePositions.clear();
            canCreateGraph = true;
            showError = false;
            errorMessage = "";
            exampleNumNodes = 0;
            exampleNumEdges = 0;
        }
    
        if (CheckCollisionPointRec(GetMousePosition(), randomButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            randomButtonClicked = true;
            inputMode = true;
            graphDrawn = false;
            nodePositions.clear();
            canCreateGraph = true;
            showError = false;
            errorMessage = "";
            exampleNumNodes = 0;
            exampleNumEdges = 0;
        }
    
        if (CheckCollisionPointRec(GetMousePosition(), exampleButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            exampleButtonClicked = true;
            showExampleButtons = !showExampleButtons;
            graphDrawn = false;
            nodePositions.clear();
            exampleNumNodes = 0;
            exampleNumEdges = 0;
        }
       
        if (CheckCollisionPointRec(GetMousePosition(), inputButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showMatrixInput = true;
            matrixInput = " ";
            cursorColumn = 0;
            cursorRow = 0;
        }

        DrawRectangleRec(createButton, WHITE);
        DrawText("Create", createButton.x + (createButton.width - MeasureText("Create", 20)) / 2, createButton.y + 10, 20, BLACK);
    
        DrawRectangleRec(randomButton, WHITE);
        DrawText("Random", randomButton.x + (randomButton.width - MeasureText("Random", 20)) / 2, randomButton.y + 10, 20, BLACK);
    
        DrawRectangleRec(exampleButton, WHITE);
        DrawText("Examples", exampleButton.x + (exampleButton.width - MeasureText("Examples", 20)) / 2, exampleButton.y + 10, 20, BLACK);

        DrawRectangleRec(inputButton, WHITE);
        DrawText("Input", inputButton.x + (inputButton.width - MeasureText("Input", 20)) / 2, inputButton.y + 10, 20, BLACK);
       
        DrawRectangleRec(fileButton, WHITE); 
        DrawText("File", fileButton.x + (fileButton.width - MeasureText("File", 20)) / 2, fileButton.y + 10, 20, BLACK);
 
        if (showExampleButtons) {
            DrawRectangleRec(k5Button, WHITE);
            DrawText("K5", k5Button.x + 5, k5Button.y + 5, 20, BLACK);
    
            DrawRectangleRec(c6Button, WHITE);
            DrawText("C6", c6Button.x + 5, c6Button.y + 5, 20, BLACK);
    
            DrawRectangleRec(p4Button, WHITE);
            DrawText("P4", p4Button.x + 5, p4Button.y + 5, 20, BLACK);
    
            DrawRectangleRec(s7Button, WHITE);
            DrawText("S7", s7Button.x + 5, s7Button.y + 5, 20, BLACK);
    
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
                exampleNumNodes = 5;
                exampleNumEdges = 10;
                numNodesStr = "";
                numEdgesStr = "";
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
                numNodesStr = "";
                numEdgesStr = "";
            }
    
            if (CheckCollisionPointRec(GetMousePosition(), p4Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                generatePathGraph(edges, 4, nodePositions, screenWidth, screenHeight);
                graphDrawn = true;
                showExampleButtons = false;
                inputMode = false;
                exampleNumNodes = 4;
                exampleNumEdges = 3;
                numNodesStr = "";
                numEdgesStr = "";
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
                numNodesStr = "";
                numEdgesStr = "";
            }
        }
        
        if (inputMode && randomButtonClicked) {
            edges.clear();
            nodePositions.clear();
            int numNodes = GetRandomValue(2, 7);
            int numEdges = GetRandomValue(numNodes - 1, numNodes * (numNodes - 1) / 2);
            generateRandomGraph(edges, numNodes, numEdges);
    
            nodePositions.resize(numNodes);
            for (int i = 0; i < numNodes; ++i) {
                float angle = 2.0f * PI * i / numNodes;
                float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
                nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
            }
    
            graphDrawn = true;
            inputMode = false;
            randomButtonClicked = false;
            numNodesStr = "";
            numEdgesStr = "";
        }
    
        if (inputMode && !randomButtonClicked) {
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
    
                    if (numNodes <= 0 || numEdges < 0 || numEdges > numNodes * (numNodes - 1) / 2 || numEdges < numNodes - 1) {
                        canCreateGraph = false;
                        showError = true;
                        errorMessage = "Can't create a graph";
                    } else {
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
    
                        if (!isGraphConnected(edges, numNodes)) {
                            canCreateGraph = false;
                            showError = true;
                            errorMessage = "Can't create connected graph with this data.";
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
       
        if (showMatrixInput) {
            DrawRectangleRec(inputWindow, LIGHTGRAY);
            DrawText("Input Adjacency Matrix", inputWindow.x + 10, inputWindow.y + 10, 20, BLACK);
            DrawRectangleRec(matrixInputRect, WHITE);

            // Vẽ thông báo lỗi (nếu có)
            DrawText(errorMessageInput.c_str(), matrixInputRect.x + 5, matrixInputRect.y + 5, 20, RED);

            // Vẽ con trỏ nhấp nháy
            cursorTimer += GetFrameTime();
            if (cursorTimer >= 0.5f) { 
               cursorTimer = 0.0f;
            }
            if (matrixInputFocused && cursorTimer < 0.25f) { // Vẽ con trỏ khi focused và trong nửa đầu chu kỳ
               // Tính toán vị trí x và y của con trỏ chính xác hơn
               Vector2 cursorPosition = GetCursorScreenPos(matrixInput, cursorRow, cursorColumn, matrixInputRect);
               DrawLine(cursorPosition.x, cursorPosition.y, cursorPosition.x, cursorPosition.y + 20, BLACK);
           }
            

            DrawText(matrixInput.c_str(), matrixInputRect.x + 5, matrixInputRect.y + 5, 20, BLACK);

            if (CheckCollisionPointRec(GetMousePosition(), matrixInputRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                matrixInputFocused = true;
            }

            if (matrixInputFocused) {
                int key = GetCharPressed();
                if (key >= 32 && key <= 125) {
                    matrixInput.insert(GetCursorIndex(matrixInput, cursorRow, cursorColumn), 1, (char)key);
                    cursorColumn++;
                }
                if (IsKeyPressed(KEY_BACKSPACE) && (cursorColumn > 0 || cursorRow > 0)) {
                    int deleteIndex = GetCursorIndex(matrixInput, cursorRow, cursorColumn) - 1;
                    if(matrixInput[deleteIndex] == '\n'){
                        cursorRow--;
                        int index = GetCursorIndex(matrixInput, cursorRow, INT_MAX);
                        cursorColumn = GetColFromIndex(matrixInput, index);
                    } else{
                        cursorColumn--;
                    }
                    matrixInput.erase(deleteIndex, 1);
                }
                if (IsKeyPressed(KEY_LEFT) && (cursorColumn > 0 || cursorRow > 0)) {
                    if (cursorColumn > 0){
                        cursorColumn--;
                    }else{
                        cursorRow--;
                        int index = GetCursorIndex(matrixInput, cursorRow, INT_MAX);
                        cursorColumn = GetColFromIndex(matrixInput, index);
                    }
                }
                if (IsKeyPressed(KEY_RIGHT) && GetCursorIndex(matrixInput, cursorRow, cursorColumn) < matrixInput.length()) {

                    if (matrixInput[GetCursorIndex(matrixInput, cursorRow, cursorColumn)] == '\n'){
                        cursorRow++;
                        cursorColumn = 0;
                    }else{
                        cursorColumn++;
                    }
                }
                if (IsKeyPressed(KEY_UP) && cursorRow > 0) {
                    cursorRow--;
                    cursorColumn = std::min(cursorColumn, GetColFromIndex(matrixInput, GetCursorIndex(matrixInput, cursorRow, INT_MAX)));
                }
                if (IsKeyPressed(KEY_DOWN) && GetCursorIndex(matrixInput, cursorRow + 1, 0) <= matrixInput.length()) {
                    cursorRow++;
                    cursorColumn = std::min(cursorColumn, GetColFromIndex(matrixInput, GetCursorIndex(matrixInput, cursorRow, INT_MAX)));
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    matrixInput.insert(GetCursorIndex(matrixInput, cursorRow, cursorColumn), 1, '\n');
                    cursorRow++;
                    cursorColumn = 0;
                }
            }
           
            // Nút Submit
            DrawRectangleRec(submitButton, WHITE);
            DrawText("Submit", submitButton.x + 10, submitButton.y + 10, 20, BLACK);
            if (CheckCollisionPointRec(GetMousePosition(), submitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                std::vector<std::vector<int>> adjacencyMatrix;
                int numNodes;
                errorMessageInput = ValidateMatrixInput(matrixInput, adjacencyMatrix, numNodes);

                if (errorMessageInput.empty()) {
                    // Ma trận hợp lệ, vẽ đồ thị
                    edges.clear();
                    nodePositions.clear();
                    nodePositions.resize(numNodes);

                // Tạo danh sách cạnh từ ma trận
                for (int i = 0; i < numNodes; ++i) {
                    for (int j = i + 1; j < numNodes; ++j) {
                        if (adjacencyMatrix[i][j] != 0) {
                            edges.push_back({ i + 1, j + 1, adjacencyMatrix[i][j] });
                        }
                    }
                }

        graphDrawn = true;
        showMatrixInput = false; // Ẩn màn hình nhập ma trận
        matrixInputFocused = false;

        // Tính toán vị trí nút
        float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
        for (int i = 0; i < numNodes; ++i) {
           float angle = 2.0f * PI * i / numNodes;
           nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
        }
    }
}
            // Nút Back
            DrawRectangleRec(closeButton, WHITE);
            DrawText("Close", closeButton.x + 10, closeButton.y + 10, 20, BLACK);
            if (CheckCollisionPointRec(GetMousePosition(), closeButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                showMatrixInput = false;
                matrixInputFocused = false;
                matrixInput = ""; 
                cursorColumn = 0; 
                cursorRow = 0;
                errorMessageInput = ""; 
            }


        if (showError && inputMode) {
            DrawText(errorMessage.c_str(), createButton.x + createButton.width + 30, createButton.y + 10, 20, RED);
        }
    }
        if (graphDrawn) {
            int numNodesToDraw=nodePositions.size();
        if (exampleNumNodes > 0) {
            numNodesToDraw = exampleNumNodes;
        } else if (numNodesStr != "") {
            numNodesToDraw = std::stoi(numNodesStr);
        } else {
            numNodesToDraw = nodePositions.size(); // Sửa lại thành số lượng node thực tế
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