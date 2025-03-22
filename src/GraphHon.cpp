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

int main() {
    const int screenWidth = 1400;
    const int screenHeight = 1000;
    
    InitWindow(screenWidth, screenHeight, "Graph Creator");
    SetTargetFPS(60);

    Rectangle createButton = {10, 10, 100, 30};
    Rectangle randomButton = {10, createButton.y + createButton.height + 10, 100, 30};
    Rectangle exampleButton = {10, randomButton.y + randomButton.height + 10, 100, 30};
    Rectangle inputButton = {10, exampleButton.y + exampleButton.height + 10, 100, 30}; // Nút Input mới

    int exampleNumNodes = 0; // Biến lưu trữ số lượng node cho đồ thị mẫu
    int exampleNumEdges = 0; // Biến lưu trữ số lượng edge cho đồ thị mẫu

    bool createButtonClicked = false;
    bool exampleButtonClicked = false;
    bool showExampleButtons = false; // Flag to show example graph buttons
    bool randomButtonClicked = false; // Biến flag cho nút random
    bool showInputScreen=false; // Biến flag cho màn hình nhập

    // Các biến cờ trạng thái cho từng chức năng
    bool isCreating = false;
    bool isRandomizing = false;
    bool isShowingExamples = false;
    bool isInputting = false;

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
    std::string matrixInput = "5 10\n1 2 28\n1 3 13\n1 4 13\n1 5 22\n2 3 27\n2 4 13\n2 5 13\n3 4 19\n3 5 14\n4 5 19";
    bool matrixInputFocused = false;
    bool matrixSubmitted = false;
    std::vector<Edge> matrixEdges;
    int cursorPosition = matrixInput.length(); // Vị trí con trỏ chuột
    float cursorTimer = 0.0f; // Thời gian nhấp nháy con trỏ
    int maxCols = 50; // Số cột tối đa trong ô nhập (điều chỉnh theo ý bạn)
    std::string errorMessage = "";

   // Thay đổi vị trí và kích thước nút
   Rectangle k5Button = { randomButton.x + randomButton.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Complete Graph (K5)
   Rectangle c6Button = { k5Button.x + k5Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Cycle Graph (C6)
   Rectangle p4Button = { c6Button.x + c6Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Path Graph (P4)
   Rectangle s7Button = { p4Button.x + p4Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Star Graph (S7)

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

    if (showInputScreen)
    {
        Rectangle inputWindow = {screenWidth / 4, screenHeight / 4, screenWidth / 2, screenHeight / 2};
        DrawRectangleRec(inputWindow, LIGHTGRAY);
        DrawText("Input Matrix:", inputWindow.x + 10, inputWindow.y + 10, 20, BLACK);

        // Nút Back
        Rectangle backButton = {inputWindow.x + 10, inputWindow.y + inputWindow.height - 40, 80, 30};
        DrawRectangleRec(backButton, WHITE);
        DrawText("Back", backButton.x + 10, backButton.y + 10, 20, BLACK);

        // Nút Submit
        Rectangle submitButton = {backButton.x + backButton.width + 10, backButton.y, 80, 30};
        DrawRectangleRec(submitButton, WHITE);
        DrawText("Submit", submitButton.x + 10, submitButton.y + 10, 20, BLACK);

        // Ô nhập matrix
        Rectangle matrixInputRect = {inputWindow.x + 10, inputWindow.y + 40, inputWindow.width - 20, inputWindow.height - 90};
        DrawRectangleRec(matrixInputRect, LIGHTGRAY);

        // Vẽ ô nhập liệu có màu khác biệt
        Rectangle inputTextArea = {matrixInputRect.x + 5, matrixInputRect.y + 5, matrixInputRect.width - 10, matrixInputRect.height - 10};
        DrawRectangleRec(inputTextArea, WHITE); // Đặt màu nền là trắng

        // Vẽ văn bản nhập liệu
        DrawText(matrixInput.c_str(), inputTextArea.x, inputTextArea.y, 20, BLACK);

         // Xử lý sự kiện click chuột vào ô nhập liệu
         if (CheckCollisionPointRec(GetMousePosition(), inputTextArea) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            matrixInputFocused = true;

            // Xác định vị trí con trỏ chuột khi click vào dòng
            Vector2 mousePos = GetMousePosition();
            float lineHeight = 20; // Chiều cao dòng
            float charWidth = MeasureText("A", 20) * 0.5f; // Ước lượng chiều rộng ký tự
            int row = (mousePos.y - inputTextArea.y) / lineHeight;
            int col = (mousePos.x - inputTextArea.x) / charWidth;

            cursorPosition = 0;
            int currentRow = 0;
            int currentCol = 0;
            for (int i = 0; i < matrixInput.length(); ++i) {
                if (currentRow == row && currentCol == col) {
                    break;
                }
                if (matrixInput[i] == '\n') {
                    currentRow++;
                    currentCol = 0;
                } else {
                    currentCol++;
                }
                cursorPosition++;
            }
        }

         if (matrixInputFocused) {
             // Xử lý nhập liệu
             int key = GetCharPressed();
             if (key >= 32 && key <= 125) {
                 // Kiểm tra chiều rộng và xuống dòng nếu cần thiết
                 int currentRow = 0;
                 int currentCol = 0;
                 for (int i = 0; i < cursorPosition; ++i) {
                     if (matrixInput[i] == '\n') {
                         currentRow++;
                         currentCol = 0;
                     } else {
                         currentCol++;
                     }
                 }

                 if (currentCol >= maxCols) {
                     matrixInput.insert(cursorPosition, "\n");
                     cursorPosition++;
                 }

                 matrixInput.insert(cursorPosition, 1, (char)key);
                 cursorPosition++;
             }
             if (key == '\n') {
                 // Xử lý phím Enter để xuống dòng
                 matrixInput.insert(cursorPosition, "\n");
                 cursorPosition++;
             }
             if (IsKeyPressed(KEY_BACKSPACE) && cursorPosition > 0) {
                if (cursorPosition >0)
                {
                 matrixInput.erase(cursorPosition - 1, 1);
                 cursorPosition--;
                }
             }
             if (IsKeyPressed(KEY_LEFT) && cursorPosition > 0) {
                 cursorPosition--;
             }
             if (IsKeyPressed(KEY_RIGHT) && cursorPosition < matrixInput.length()) {
                 cursorPosition++;
             }
             if (IsKeyPressed(KEY_UP)) {
                 // Xử lý phím lên để di chuyển con trỏ lên hàng trên
                    int currentRow = 0;
                    int currentCol = 0;
                    int currentPos = 0;
                    int newRow = -1;
                    int newCol = -1;

                    for (int i = 0; i < matrixInput.length(); ++i) {
                        if (i == cursorPosition) {
                            break;
                        }
                        if (matrixInput[i] == '\n') {
                            currentRow++;
                            currentCol = 0;
                        } else {
                            currentCol++;
                        }
                        currentPos++;
                    }

                    if (currentRow > 0) {
                        int count = 0;
                        for (int i = 0; i < matrixInput.length(); ++i) {
                            if (matrixInput[i] == '\n') {
                                count++;
                            }
                            if (count == currentRow - 1) {
                                newRow = count;
                                newCol = 0;

                                // Tìm vị trí con trỏ mới dựa trên cột hiện tại
                                int lineLength = 0;
                                for (int j = i + 1; j < matrixInput.length() && matrixInput[j] != '\n'; ++j) {
                                    lineLength++;
                                }
                                newCol = std::min(currentCol, lineLength);

                                // Tìm vị trí con trỏ mới trong chuỗi
                                cursorPosition = i + 1;
                                for (int j = 0; j < newCol; ++j) {
                                    cursorPosition++;
                                }
                                break;
                            }
                        }
                    }
             }
             if (IsKeyPressed(KEY_DOWN)) {
                  // Xử lý phím xuống để di chuyển con trỏ xuống hàng dưới
                  int currentRow = 0;
                  int currentCol = 0;
                  int currentPos = 0;
                  int newRow = -1;
                  int newCol = -1;

                  for (int i = 0; i < matrixInput.length(); ++i) {
                      if (i == cursorPosition) {
                          break;
                      }
                      if (matrixInput[i] == '\n') {
                          currentRow++;
                          currentCol = 0;
                      } else {
                          currentCol++;
                      }
                      currentPos++;
                  }

                  int lineCount = 0;
                  for (int i = 0; i < matrixInput.length(); ++i) {
                      if (matrixInput[i] == '\n') {
                          lineCount++;
                      }
                  }

                  if (currentRow < lineCount) {
                      int count = 0;
                      for (int i = 0; i < matrixInput.length(); ++i) {
                          if (matrixInput[i] == '\n') {
                              count++;
                          }
                          if (count == currentRow + 1) {
                              newRow = count;
                              newCol = 0;

                              // Tìm vị trí con trỏ mới dựa trên cột hiện tại
                              int lineLength = 0;
                              for (int j = i + 1; j < matrixInput.length() && matrixInput[j] != '\n'; ++j) {
                                  lineLength++;
                              }
                              newCol = std::min(currentCol, lineLength);

                              // Tìm vị trí con trỏ mới trong chuỗi
                              cursorPosition = i + 1;
                              for (int j = 0; j < newCol; ++j) {
                                  cursorPosition++;
                              }
                              break;
                          }
                      }
                  }
              }
         }
        
        // Vẽ con trỏ chuột nhấp nháy
        if (matrixInputFocused) {
            cursorTimer += GetFrameTime();
            if (cursorTimer >= 0.5f) { // Thời gian nhấp nháy (0.5 giây)
                cursorTimer -= 0.5f;
            }
            if (cursorTimer < 0.25f) {
                // Tính toán vị trí con trỏ dựa trên vị trí hàng và cột
                int row = 0;
                int col = 0;
                int currentPos = 0;
                for (int i = 0; i < matrixInput.length(); ++i) {
                    if (i == cursorPosition) {
                        break;
                    }
                    if (matrixInput[i] == '\n') {
                        row++;
                        col = 0;
                    } else {
                        col++;
                    }
                    currentPos++;
                }

                float cursorX = inputTextArea.x + MeasureText(matrixInput.substr(0, cursorPosition).c_str(), 20); // Vị trí X của ô
                float cursorY = inputTextArea.y + row * 20; // Vị trí Y của ô
                float charWidth = MeasureText("A", 20) * 0.5f; // Chiều rộng của ký tự
                float lineHeight = 20; // Chiều cao của dòng

                // Vẽ con trỏ chuột (hình chữ nhật nhấp nháy)
                Rectangle cursorRect = {cursorX - charWidth * 0.25f, cursorY, charWidth, lineHeight};
                DrawRectangleRec(cursorRect, BLACK); // Vẽ hình chữ nhật đen
            }
        }

        if (CheckCollisionPointRec(GetMousePosition(), backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showInputScreen = false;
            matrixInput = "5 10\n1 2 28\n1 3 13\n1 4 13\n1 5 22\n2 3 27\n2 4 13\n2 5 13\n3 4 19\n3 5 14\n4 5 19"; // Reset giá trị ban đầu
            matrixSubmitted = false;
            matrixEdges.clear();
        }

        if (CheckCollisionPointRec(GetMousePosition(), submitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Xử lý khi nút Submit được nhấn
            matrixSubmitted = true;
            matrixInputFocused = false;

            // Phân tích dữ liệu nhập vào thành các cạnh (Edge)
            std::stringstream ss(matrixInput);
            int numNodes, numEdges;
            ss >> numNodes >> numEdges;
            matrixEdges.clear();

            int from, to, weight;
            while (ss >> from >> to >> weight) {
                matrixEdges.push_back({from, to, weight});
            }
        }
    }
    else 
    {
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
            showInputScreen = true;
        }
        
        DrawRectangleRec(createButton, WHITE);
        DrawText("Create", createButton.x + (createButton.width - MeasureText("Create", 20)) / 2, createButton.y + 10, 20, BLACK);
    
        DrawRectangleRec(randomButton, WHITE);
        DrawText("Random", randomButton.x + (randomButton.width - MeasureText("Random", 20)) / 2, randomButton.y + 10, 20, BLACK);
    
        DrawRectangleRec(exampleButton, WHITE);
        DrawText("Examples", exampleButton.x + (exampleButton.width - MeasureText("Examples", 20)) / 2, exampleButton.y + 10, 20, BLACK);

        DrawRectangleRec(inputButton, WHITE);
        DrawText("Input", inputButton.x + (inputButton.width - MeasureText("Input", 20)) / 2, inputButton.y + 10, 20, BLACK);

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
    
            for (int i = 0; i < numNodesToDraw; ++i) { // Vẽ tất cả các node
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
    }
    EndDrawing();
}

CloseWindow();
return 0;
}