#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>


// Kích thước ô ma trận
const int CELL_WIDTH = 30;
const int CELL_HEIGHT = 20;
const int MAX_LINE_LENGTH = 30; // Số ký tự tối đa trên mỗi dòng

// Vị trí cuộn ma trận
Vector2 scrollOffset = {0, 0};

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
    bool matrixInputFocused = false;
    bool matrixSubmitted = false;
    std::vector<Edge> matrixEdges;
    int cursorPosition = 0; // Vị trí con trỏ chuột
    float cursorTimer = 0.0f; // Thời gian nhấp nháy con trỏ
    int maxCols = 50; // Số cột tối đa trong ô nhập (điều chỉnh theo ý bạn)
    std::string errorMessage = "";

   // Thay đổi vị trí và kích thước nút
   Rectangle k5Button = { randomButton.x + randomButton.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Complete Graph (K5)
   Rectangle c6Button = { k5Button.x + k5Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Cycle Graph (C6)
   Rectangle p4Button = { c6Button.x + c6Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Path Graph (P4)
   Rectangle s7Button = { p4Button.x + p4Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Star Graph (S7)
 
   // Ma trận mẫu (dạng văn bản)
    std::string matrixInput =
        "9 0 8 12 0 0 0 0 0\n"
        "8 0 13 25 9 0 0 0 0\n"
        "12 13 0 14 0 0 21 0 0\n"
        "0 25 14 0 20 8 12 12 16\n"
        "0 9 0 20 0 19 0 0 0\n"
        "0 0 0 8 19 0 0 11 0\n"
        "0 0 21 12 0 0 0 0 11\n"
        "0 0 0 12 0 11 0 0 9\n"
        "0 0 0 16 0 0 11 9 0";

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

    if (showInputScreen) {
        Rectangle inputWindow = {screenWidth / 4, screenHeight / 4, screenWidth / 2, screenHeight / 2};
        Rectangle matrixInputRect = {inputWindow.x + 10, inputWindow.y + 40, inputWindow.width - 20, inputWindow.height - 90};
    
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

        // **Khung màu trắng chứa ma trận**
        Rectangle matrixDisplayRect = {matrixInputRect.x, matrixInputRect.y, matrixInputRect.width, matrixInputRect.height};
        DrawRectangleRec(matrixDisplayRect, WHITE);

        // Xử lý cuộn ma trận
        if (IsKeyDown(KEY_UP)) scrollOffset.y += 5;
        if (IsKeyDown(KEY_DOWN)) scrollOffset.y -= 5;
        if (IsKeyDown(KEY_LEFT)) scrollOffset.x += 5;
        if (IsKeyDown(KEY_RIGHT)) scrollOffset.x -= 5;
    
        // Giới hạn vùng cuộn
        int matrixWidth = 9 * CELL_WIDTH; // Giả sử ma trận có 9 cột
        int matrixHeight = 9 * CELL_HEIGHT; // Giả sử ma trận có 9 hàng
        scrollOffset.x = std::max(scrollOffset.x, 0.0f);
        scrollOffset.y = std::max(scrollOffset.y, 0.0f);
        scrollOffset.x = std::min(scrollOffset.x, (float)(matrixWidth - matrixInputRect.width));
        scrollOffset.y = std::min(scrollOffset.y, (float)(matrixHeight - matrixInputRect.height));

        // Chuyển matrixInput thành vector<string> để vẽ
        std::vector<std::string> matrixRows;
        std::stringstream ss(matrixInput);
        std::string line;
        while (std::getline(ss, line)) {
            matrixRows.push_back(line);
        }
    
        // Vẽ ma trận
        for (int row = 0; row < matrixRows.size(); ++row) {
            DrawText(matrixRows[row].c_str(), matrixInputRect.x - scrollOffset.x, matrixInputRect.y + row * CELL_HEIGHT - scrollOffset.y, 20, BLACK);
        }
    
        // Xử lý click chuột vào ma trận để di chuyển con trỏ
        if (CheckCollisionPointRec(GetMousePosition(), matrixInputRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = {GetMousePosition().x + scrollOffset.x - matrixInputRect.x , GetMousePosition().y + scrollOffset.y - matrixInputRect.y};
            int clickedRow = mousePos.y / CELL_HEIGHT;
            int clickedCol = 0;
            float currentWidth = 0;
    
            if (clickedRow < matrixRows.size()) {
                for (int i = 0; i < matrixRows[clickedRow].length(); ++i) {
                    float charWidth = MeasureText(matrixRows[clickedRow].substr(i, 1).c_str(), 20);
                    if (currentWidth + charWidth / 2 >= mousePos.x) {
                        cursorPosition = 0;
                        for (int j = 0; j < clickedRow; ++j) {
                            cursorPosition += matrixRows[j].length() + 1; // +1 cho '\n'
                        }
                        cursorPosition += i;
                        matrixInputFocused = true;
                        break;
                    }
                    currentWidth += charWidth;
                }
                 // Nếu click sau kí tự cuối cùng của row
                if (!matrixInputFocused){
                     cursorPosition = 0;
                    for (int j = 0; j < clickedRow; ++j) {
                        cursorPosition += matrixRows[j].length() + 1; // +1 cho '\n'
                    }
                    cursorPosition += matrixRows[clickedRow].length();
                    matrixInputFocused = true;
                }
            }
        }
    
        // Vẽ con trỏ
        if (matrixInputFocused) {
            int cursorX = matrixInputRect.x - scrollOffset.x;
            int cursorY = matrixInputRect.y - scrollOffset.y;
            int row = 0;
            int col = 0;
            for (int i = 0; i < cursorPosition; ++i) {
                if (matrixInput[i] == '\n') {
                    row++;
                    col = 0;
                } else {
                    col++;
                }
            }
            cursorX += MeasureText(matrixRows[row].substr(0, col).c_str(), 20);
            cursorY += row * CELL_HEIGHT;
            DrawLine(cursorX, cursorY, cursorX, cursorY + 20, RED);
        }
    
        // Xử lý nhập liệu
        if (matrixInputFocused) {
            int key = GetCharPressed();
            if (key >= 32 && key <= 125) {
                // Thêm ký tự và tự động xuống dòng nếu đến cuối dòng
                matrixInput.insert(cursorPosition, 1, (char)key);
                cursorPosition++;

                // Kiểm tra và xuống dòng nếu cần
                int currentRowLength = 0;
                for (int i = 0; i < cursorPosition; ++i) {
                    if (matrixInput[i] == '\n') {
                        currentRowLength = 0;
                    } else {
                        currentRowLength++;
                    }
                }

                if (currentRowLength >= MAX_LINE_LENGTH) {
                    matrixInput.insert(cursorPosition, "\n");
                    cursorPosition++;
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE) && cursorPosition > 0) {
                matrixInput.erase(cursorPosition - 1, 1);
                cursorPosition--;
            }
    
            // Di chuyển con trỏ bằng phím mũi tên
            if (IsKeyPressed(KEY_LEFT) && cursorPosition > 0) cursorPosition--;
            if (IsKeyPressed(KEY_RIGHT) && cursorPosition < matrixInput.length()) cursorPosition++;
    
            // di chuyển lên xuống
            if (IsKeyPressed(KEY_UP)){
                int currentRow = 0;
                int currentCol = 0;
                int tempPos = 0;
    
                for (int i =0; i < matrixInput.length(); i++){
                    if (i == cursorPosition){
                        break;
                    }
    
                    if (matrixInput[i] == '\n'){
                        currentRow ++;
                        currentCol = 0;
                    }else {
                        currentCol++;
                    }
                    tempPos ++;
                }
    
                if (currentRow > 0){
                    int countNewline = 0;
                    for (int i = 0; i < matrixInput.length(); i++){
                        if (matrixInput[i] == '\n'){
                            countNewline++;
                        }
    
                        if (countNewline == currentRow - 1){
    
                            int colMove = 0;
                            for(int k = i + 1; k < matrixInput.length();k++){
    
                                if(matrixInput[k] == '\n') break;
                                 colMove ++;
                            }
    
                             cursorPosition = i +1 ;
                             cursorPosition += std::min(currentCol,colMove);
    
                            break;
                        }
                    }
                }
    
            }
    
            if (IsKeyPressed(KEY_DOWN)){
    
                int currentRow = 0;
                int currentCol = 0;
                int tempPos =0;
    
                for (int i = 0; i < matrixInput.length();i ++){
                    if (i == cursorPosition) break;
                     if (matrixInput[i] =='\n'){
                        currentRow++;
                        currentCol =0;
                     } else {
                        currentCol ++;
                     }
    
                    tempPos++;
                }
    
                int countNewline = 0;
    
                for (int i = 0; i < matrixInput.length(); i++){
                    if (matrixInput[i] == '\n') countNewline++;
                }
    
                if (currentRow < countNewline){
    
                    int moveLine = 0;
    
                    for (int i =0; i < matrixInput.length(); i++){
    
                        if (matrixInput[i] == '\n'){
                            moveLine ++;
                        }
    
                        if(moveLine == currentRow + 1){
                             int colMove = 0;
                             for (int k = i+1; k < matrixInput.length(); k++){
    
                                 if (matrixInput[k] == '\n') break;
                                 colMove ++;
                             }
                            cursorPosition = i +1;
                            cursorPosition+= std::min(currentCol,colMove);
                            break;
                        }
                    }
    
                }
            }
        }
    
        // Xử lý nút back và submit
        if (CheckCollisionPointRec(GetMousePosition(), backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showInputScreen = false;
            matrixInputFocused = false;
        }
        if (CheckCollisionPointRec(GetMousePosition(), submitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showInputScreen = false;
            matrixInputFocused = false;
            // Xử lý data ở đây
            // Ví dụ: In ra ma trận để kiểm tra
            std::cout << "Ma trận đã nhập:\n" << matrixInput << std::endl;
    
            // Hoặc bạn có thể chuyển ma trận thành mảng 2D để xử lý tiếp
            // ... (code chuyển đổi matrixInput thành mảng 2D)
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