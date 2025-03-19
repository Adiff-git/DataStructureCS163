#include <iostream>
#include "raylib.h"
#include <vector>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <math.h>

using namespace std;

struct Node {
    int key;
    Node* left;
    Node* right;
    Vector2 position;
};

Node* newNode(int key) {
    Node* node = new Node();
    node->key = key;
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

Node* insert(Node* root, int key) {
    if (root == nullptr) {
        return newNode(key);
    }
    if (key < root->key) {
        root->left = insert(root->left, key);
    } else if (key > root->key) {
        root->right = insert(root->right, key);
    }
    return root;
}

Node* minValueNode(Node* node) {
    Node* current = node;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

Node* deleteNode(Node* root, int key) {
    if (root == nullptr) return root;
    if (key < root->key) root->left = deleteNode(root->left, key);
    else if (key > root->key) root->right = deleteNode(root->right, key);
    else {
        if (root->left == nullptr) {
            Node* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            Node* temp = root->left;
            delete root;
            return temp;
        }
        Node* temp = minValueNode(root->right);
        root->key = temp->key;
        root->right = deleteNode(root->right, temp->key);
    }
    return root;
}

Node* search(Node* root, int key) {
    if (root == nullptr || root->key == key) {
        return root;
    }
    if (key < root->key) {
        return search(root->left, key);
    }
    return search(root->right, key);
}

void inorderTraversal(Node* root, string& result) {
    if (root != nullptr) {
        inorderTraversal(root->left, result);
        result += to_string(root->key) + " ";
        inorderTraversal(root->right, result);
    }
}

void calculateNodePositions(Node* root, float x, float y, float xOffset, float yOffset) {
    if (root != nullptr) {
        root->position = {x, y};
        if (root->left) calculateNodePositions(root->left, x - xOffset, y + yOffset, xOffset / 2, yOffset);
        if (root->right) calculateNodePositions(root->right, x + xOffset, y + yOffset, xOffset / 2, yOffset);
    }
}

void drawBST(Node* root) {
    if (root != nullptr) {
        if (root->left) DrawLine(root->position.x, root->position.y, root->left->position.x, root->left->position.y, RAYWHITE);
        if (root->right) DrawLine(root->position.x, root->position.y, root->right->position.x, root->right->position.y, RAYWHITE);
        DrawCircle(root->position.x, root->position.y, 20, LIGHTGRAY);
        DrawText(to_string(root->key).c_str(), root->position.x - 10, root->position.y - 10, 20, BLACK);
        drawBST(root->left);
        drawBST(root->right);
    }
}

void DrawInputField(Rectangle rect, char* text, bool isActive, int fontSize);

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Binary Search Tree");
    SetTargetFPS(60);

    Node* root = nullptr;

    float buttonHeight = 30;
    int fontSize = 20;
    int resultFontSize = 30;

    float treeRootX = screenWidth / 2;
    float treeRootY = 200;

    string searchResult = "";
    string traverseResult = "";
    string functionLabels[] = {"Insert", "Delete", "Search", "Traverse"};

    Rectangle toggleButton = {10, 10, 30, 4 * (buttonHeight + 10) + 10};
    bool functionsVisible = true;

    float functionWidths[4];
    for (int i = 0; i < 4; i++) {
        functionWidths[i] = MeasureText(functionLabels[i].c_str(), fontSize) + 20;
    }
    float maxFunctionWidth = *max_element(functionWidths, functionWidths + 4);

    Rectangle functionsRect = {40, 10, maxFunctionWidth + 20, 4 * (buttonHeight + 10) + 10};
    Rectangle functionRects[4];
    for (int i = 0; i < 4; i++) {
        functionRects[i] = {functionsRect.x + 10, functionsRect.y + 10 + i * (buttonHeight + 10), maxFunctionWidth, buttonHeight};
    }

    Rectangle inputRects[4];
    char inputValues[4][10] = {{0}, {0}, {0}, {0}};
    bool inputActive[4] = {false, false, false, false};
    bool showInput[4] = {false, false, false, false};

    for (int i = 0; i < 4; i++) {
        inputRects[i] = {functionsRect.x + functionsRect.width + 10, functionsRect.y + 10 + i * (buttonHeight + 10), 80, buttonHeight};
    }

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, toggleButton)) {
            functionsVisible = !functionsVisible;
            if (!functionsVisible) {
                for (int i = 0; i < 4; i++) {
                    showInput[i] = false;
                    inputActive[i] = false;
                }
            }
        }

        if (functionsVisible) {
            for (int i = 0; i < 4; i++) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, functionRects[i])) {
                    for (int j = 0; j < 4; j++) {
                        showInput[j] = false;
                        inputActive[j] = false;
                    }
                    if (i != 3) {
                        showInput[i] = true;
                    }
                }
            }

            for (int i = 0; i < 4; i++) {
                if (showInput[i] && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, inputRects[i])) {
                    inputActive[i] = true;
                }
            }

            for (int i = 0; i < 4; i++) {
                if (inputActive[i] && showInput[i]) {
                    int keyPress = GetCharPressed();
                    while (keyPress > 0) {
                        if ((keyPress >= '0' && keyPress <= '9')) {
                            if (strlen(inputValues[i]) < 9) {
                                inputValues[i][strlen(inputValues[i])] = (char)keyPress;
                                inputValues[i][strlen(inputValues[i]) + 1] = '\0';
                            }
                        }
                        keyPress = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        if (strlen(inputValues[i]) > 0) {
                            inputValues[i][strlen(inputValues[i]) - 1] = '\0';
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_ENTER)) {
                for (int i = 0; i < 4; i++) {
                    if (showInput[i]) {
                        if (i == 0) root = insert(root, atoi(inputValues[i]));
                        else if (i == 1) root = deleteNode(root, atoi(inputValues[i]));
                        else if (i == 2) {
                            int value = atoi(inputValues[i]);
                            Node* found = search(root, value);
                            searchResult = found ? "Found " + to_string(value) : "Not Found " + to_string(value);
                        }
                        inputValues[i][0] = '\0';
                        showInput[i] = false;
                        inputActive[i] = false;
                        calculateNodePositions(root, treeRootX, treeRootY, 100, 80);
                    }
                }
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, functionRects[3])) {
                traverseResult = "";
                inorderTraversal(root, traverseResult);
                showInput[3] = true;
            }
        }

        BeginDrawing();
        ClearBackground(DARKBROWN);

        DrawRectangleRec({10, 10, 30, functionsRect.height}, GRAY);

        if (functionsVisible) {
            DrawTriangle({25, 10 + functionsRect.height / 2 - 10}, {15, 10 + functionsRect.height / 2}, {25, 10 + functionsRect.height / 2 + 10}, BLACK);
        } else {
            DrawTriangle({15, 10 + functionsRect.height / 2 - 10}, {25, 10 + functionsRect.height / 2}, {15, 10 + functionsRect.height / 2 + 10}, BLACK);
        }

        if (functionsVisible) {
            DrawRectangleRec(functionsRect, LIGHTGRAY);

            for (int i = 0; i < 4; i++) {
                DrawRectangleRec(functionRects[i], BLUE);
                DrawText(functionLabels[i].c_str(), functionRects[i].x + 10, functionRects[i].y + 5, fontSize, WHITE);
            }

            for (int i = 0; i < 4; i++) {
                if (showInput[i]) {
                    DrawInputField(inputRects[i], inputValues[i], inputActive[i], fontSize);
                }
            }
            if(showInput[3]){
                DrawText(traverseResult.c_str(), 10, screenHeight - 200, resultFontSize, WHITE);
            }
        }

        drawBST(root);

        if (!searchResult.empty()) {
            DrawText(searchResult.c_str(), screenWidth - 250, screenHeight - 150, resultFontSize, WHITE);
        }
        if (!traverseResult.empty() && !showInput[3]) {
            DrawText(traverseResult.c_str(), screenWidth - 250, screenHeight - 100, resultFontSize, WHITE);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void DrawInputField(Rectangle rect, char* text, bool isActive, int fontSize) {
    DrawRectangleRec(rect, isActive ? LIGHTGRAY : WHITE);
    DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, BLACK);

    float textWidth = MeasureText(text, fontSize);
    float textX = rect.x + (rect.width - textWidth) / 2;
    float textY = rect.y + (rect.height - fontSize) / 2;
    DrawText(text, textX, textY, fontSize, BLACK);

    if (isActive) {
        float cursorWidth = 2;
        float cursorX = textX + textWidth;
        float cursorY = textY;
        float cursorHeight = fontSize;

        if ((int)(GetTime() * 2) % 2 == 0) {
            DrawRectangle(cursorX, cursorY, cursorWidth, cursorHeight, BLACK);
        }
    }
}
