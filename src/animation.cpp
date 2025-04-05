#include "../inc/AVL.h"
#include "../inc/Color.h"
#include "../inc/animation.h"
#include <sstream>
#include <cstdlib>
#include <stack>
#include <iostream>
#include <cmath>
#include <fstream>

std::stack<AVLTree> treeUndoState;
std::stack<AVLTree> treeRedoState;
#define NODE_RADIUS 20.0f

AVLTreeVisualizer::AVLTreeVisualizer() {
    inputText = "";
    inputActive = false;
    handleSpace = { 0.0f, 0.0f, 1600.0f, 75.0f };
    inputBox     = { 20.0f, 20.0f, 200.0f, 30.0f };
    insertButton = { 230.0f, 20.0f, 80.0f, 30.0f };
    deleteButton = { 320.0f, 20.0f, 80.0f, 30.0f };
    searchButton = { 410.0f, 20.0f, 90.0f, 30.0f };
    randomButton = { 510.0f, 20.0f, 90.0f, 30.0f };
    clearButton  = { 610.0f, 20.0f, 70.0f, 30.0f };
    loadFileButton = { 690.0f, 20.0f, 110.0f, 30.0f };
    previousButton = { 1000.0f, 20.0f, 105.0f, 30.0f };
    nextButton = { 1120.0f, 20.0f, 65.0f, 30.0f }; 

    currentState = IDLE;
    pathIndex = 0;
    rotationIndex = 0;
    searchFound = false;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
    operationValue = 0;
    pendingInsertValue = 0;

    animationSpeed = 0.5f;
    // Move speed bar to top-right corner, below the operation buttons
    speedBar = { 1000.0f, 60.0f, 200.0f, 20.0f };
    sliderHandle = { speedBar.x + (animationSpeed * (speedBar.width - 10.0f)), speedBar.y + 2.0f, 10.0f, 16.0f };
    draggingSlider = false;

    // Move animation control buttons to top-right, below speed bar
    playButton = { speedBar.x, speedBar.y + 30.0f, 50.0f, 30.0f };
    pauseButton = { speedBar.x + 60.0f, speedBar.y + 30.0f, 50.0f, 30.0f };
    backwardButton = { speedBar.x + 120.0f, speedBar.y + 30.0f, 50.0f, 30.0f };
    forwardButton = { speedBar.x + 180.0f, speedBar.y + 30.0f, 50.0f, 30.0f };
    paused = false;

    // Load Raleway font
    ralewayFont = LoadFont("resources/Raleway-Regular.ttf");
}

AVLTreeVisualizer::~AVLTreeVisualizer() {
    UnloadFont(ralewayFont);  // Unload the font when the visualizer is destroyed
}

void AVLTreeVisualizer::handleInput() {
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive = false;
    }

    if (inputActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && inputText.length() < 6) {
                inputText += (char)key;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
            inputText.pop_back();
        }
    }

    if (CheckCollisionPointRec(mousePos, insertButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        int insertValue = std::stoi(inputText);
        animateInsert(insertValue);
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, deleteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        int deleteValue = std::stoi(inputText);
        animateDelete(deleteValue);
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, searchButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        int searchValue = std::stoi(inputText);
        animateSearch(searchValue);
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, randomButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateRandom();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, clearButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateClear();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, loadFileButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateLoadFile();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, previousButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animatePrevious();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, nextButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateNext();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, speedBar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        draggingSlider = true;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        draggingSlider = false;
    }
    if (draggingSlider) {
        float newX = mousePos.x - speedBar.x;
        if (newX < 0) newX = 0;
        if (newX > speedBar.width - 10.0f) newX = speedBar.width - 10.0f;
        animationSpeed = newX / (speedBar.width - 10.0f);
        sliderHandle.x = speedBar.x + newX;
    }

    if (CheckCollisionPointRec(mousePos, playButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        paused = false;
    }
    if (CheckCollisionPointRec(mousePos, pauseButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        paused = true;
    }
    if (CheckCollisionPointRec(mousePos, forwardButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && currentState == TRAVERSING) {
        if (pathIndex < currentPath.size()) {
            pathIndex++;
            stateTimer = 0.0f;
        }
    }
    if (CheckCollisionPointRec(mousePos, backwardButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && currentState == TRAVERSING) {
        if (pathIndex > 0) {
            pathIndex--;
            highlightNodes.erase(currentPath[pathIndex]);
            stateTimer = 0.0f;
        }
    }
}

void AVLTreeVisualizer::updateAnimation(float deltaTime) {
    if (currentState == IDLE || paused) {
        return;
    }

    stateTimer += deltaTime * (animationSpeed * 5.0f);

    float duration = 0.0f;

    switch (currentState) {
        case TRAVERSING:
            if (stateTimer >= 0.5f && pathIndex < currentPath.size()) {
                Node* currentNode = currentPath[pathIndex];
                highlightNodes.insert(currentNode);
                notificationMessage = "Traverse: " + std::to_string(currentNode->data);
                pathIndex++;
                stateTimer = 0.0f;
            }
            if (pathIndex >= currentPath.size()) {
                if (currentOperation == "insert") {
                    currentState = INSERTING;
                } else if (currentOperation == "delete") {
                    if (!tree.search(operationValue)) {
                        notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";
                        stateTimer = 0.0f;
                        currentState = SHOWING_RESULT;
                    } else {
                        currentState = HIGHLIGHTING_BEFORE_DELETE;
                    }
                } else if (currentOperation == "search") {
                    currentState = SEARCHING;
                }
                stateTimer = 0.0f;
            }
            break;

        case INSERTING:
            if (stateTimer >= 0.5f) {
                if (!tree.search(pendingInsertValue)) {
                    tree.insert(tree.root, pendingInsertValue);
                    AVLTree treeReplica(tree);
                    treeUndoState.push(treeReplica);
                    notificationMessage = "Inserted " + std::to_string(pendingInsertValue);
                } else {
                    notificationMessage = "Duplicate " + std::to_string(pendingInsertValue) + " Not Inserted";
                }
                currentPath = tree.getInsertionPath(pendingInsertValue);
                pathIndex = currentPath.size();
                currentState = SHOWING_RESULT;
                resultTimer = 0.0f;
                stateTimer = 0.0f;
            }
            break;

        case HIGHLIGHTING_BEFORE_DELETE:
            if (stateTimer >= 0.5f) {
                notificationMessage = "Deleting " + std::to_string(operationValue);
                currentState = DELETING;
                stateTimer = 0.0f;
            }
            break;

        case DELETING:
            if (stateTimer >= 0.5f) {
                tree.remove(tree.root, operationValue);
                AVLTree treeReplica(tree);
                treeUndoState.push(treeReplica);
                notificationMessage = std::to_string(operationValue) + " Deleted";
                currentState = SHOWING_RESULT;
                resultTimer = 0.0f;
                stateTimer = 0.0f;
            }
            break;

        case ROTATING:
            notificationMessage = "Executing rotation";
            currentState = SHOWING_RESULT;
            resultTimer = 0.0f;
            break;

        case SHOWING_RESULT:
            resultTimer += deltaTime;
            duration = (currentOperation == "search" || currentOperation == "delete") && !searchFound ? 4.0f : 2.0f;
            if (resultTimer >= duration) {
                currentState = IDLE;
                currentPath.clear();
                pathIndex = 0;
                rotationIndex = 0;
                searchFound = false;
                highlightNodes.clear();
            }
            break;

        case SEARCHING:
            if (stateTimer >= 0.5f && pathIndex < currentPath.size()) {
                Node* currentNode = currentPath[pathIndex];
                highlightNodes.insert(currentNode);
                notificationMessage = "Searching: " + std::to_string(currentNode->data);
                if (currentNode->data == operationValue) {
                    searchFound = true;
                    notificationMessage = "Found " + std::to_string(operationValue);
                    currentState = SHOWING_RESULT;
                    stateTimer = 0.0f;
                    resultTimer = 0.0f;
                    break;
                }
                pathIndex++;
                stateTimer = 0.0f;
            }
            if (pathIndex >= currentPath.size() && !searchFound) {
                notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";
                stateTimer = 0.0f;
                currentState = SHOWING_RESULT;
            }
            break;
    }
}

void AVLTreeVisualizer::drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight) {
    if (!node) return;

    Color nodeColor = WHITE;
    if (highlight.count(node)) {
        if (currentState == SEARCHING && node->data == operationValue && searchFound) {
            nodeColor = GREEN;
        } else if (currentState == HIGHLIGHTING_BEFORE_DELETE && node->data == operationValue) {
            nodeColor = ORANGE;
        } else {
            nodeColor = YELLOW;
        }
    } else if (currentState == SHOWING_RESULT && node->data == operationValue) {
        if (currentOperation == "insert") {
            nodeColor = GREEN;
        } else if (currentOperation == "delete") {
            nodeColor = RED;
        }
    }

    DrawCircle(x, y, NODE_RADIUS, nodeColor);
    std::string valueStr = std::to_string(node->data);
    DrawTextEx(ralewayFont, valueStr.c_str(), { x - MeasureText(valueStr.c_str(), 20) / 2, y - 10 }, 20, 1, BLACK);

    if (node->left) {
        float leftX = tree.getSubtreeWidth(node->left->right);
        float hypotenus = sqrt(60.0f * 60.0f + leftX * leftX);
        float startLineX = x - NODE_RADIUS * leftX / hypotenus;
        float startLineY = y + NODE_RADIUS * 60.0f / hypotenus;
        DrawLine(startLineX, startLineY, x - leftX, y + 60, BLACK);
        drawTree(node->left, x - leftX, y + 60, offset / 2, highlight);
    }

    if (node->right) {
        float rightX = tree.getSubtreeWidth(node->right->left);
        float hypotenus = sqrt(60.0f * 60.0f + rightX * rightX);
        float startLineX = x + NODE_RADIUS * rightX / hypotenus;
        float startLineY = y + NODE_RADIUS * 60.0f / hypotenus;
        DrawLine(startLineX, startLineY, x + rightX, y + 60, BLACK);
        drawTree(node->right, x + rightX, y + 60, offset / 2, highlight);
    }
}

void AVLTreeVisualizer::draw() {
    std::set<Node*> highlightNodes(currentPath.begin(), currentPath.begin() + pathIndex);

    DrawRectangleRec(handleSpace, HandleInputSpaceBG);
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);
    DrawTextEx(ralewayFont, inputText.c_str(), { inputBox.x + 5, inputBox.y + 5 }, 20, 1, BLACK);

    DrawRectangleRec(insertButton, GREEN);
    DrawRectangleRec(deleteButton, ORANGE);
    DrawRectangleRec(searchButton, BLUE);
    DrawRectangleRec(randomButton, PURPLE);
    DrawRectangleRec(clearButton, RED);
    DrawRectangleRec(loadFileButton, SKYBLUE);
    DrawRectangleRec(previousButton, BlueButton);
    DrawRectangleRec(nextButton, BlueButton);

    DrawTextEx(ralewayFont, "Insert", { insertButton.x + 10, insertButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Delete", { deleteButton.x + 10, deleteButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Search", { searchButton.x + 10, searchButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Random", { randomButton.x + 10, randomButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Clear", { clearButton.x + 10, clearButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Load File", { loadFileButton.x + 10, loadFileButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Previous", { previousButton.x + 10, previousButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Next", { nextButton.x + 10, nextButton.y + 5 }, 20, 1, WHITE);

    if (tree.root) {
        drawTree(tree.root, GetScreenWidth() / 2, 120, 200, highlightNodes);
    }

    if (!notificationMessage.empty()) {
        int messageSize = MeasureText(notificationMessage.c_str(), 20);
        DrawTextEx(ralewayFont, notificationMessage.c_str(), { static_cast<float>(GetScreenWidth() - messageSize - 10), 10 }, 20, 1, BLACK);
    }

    if (currentState == SHOWING_RESULT && currentOperation == "search" && !searchFound) {
        std::string messageNotFound = std::to_string(operationValue) + " is not in the tree";
        int messageSize = MeasureText(messageNotFound.c_str(), 20);
        DrawTextEx(ralewayFont, messageNotFound.c_str(), { static_cast<float>(GetScreenWidth() - messageSize - 10), 30 }, 20, 1, RED);
    }

    std::string pseudocodeText = getPseudocode();
    int lineHeight = 20;
    int lineCount = 0;
    std::istringstream stream(pseudocodeText);
    std::string line;
    while (std::getline(stream, line)) {
        lineCount++;
    }
    int startY = GetScreenHeight() - (lineCount * lineHeight) - 10;
    stream.clear();
    stream.str(pseudocodeText);
    while (std::getline(stream, line)) {
        DrawTextEx(ralewayFont, line.c_str(), { 10, static_cast<float>(startY) }, 20, 1, BLACK);
        startY += lineHeight;
    }

    DrawRectangleRec(speedBar, LIGHTGRAY);
    DrawRectangleLinesEx(speedBar, 2, GRAY);
    DrawRectangleRec(sliderHandle, BLUE);
    DrawTextEx(ralewayFont, "Speed", { speedBar.x - 50, speedBar.y + 2 }, 20, 1, BLACK);

    std::string speedText = "Speed: " + std::to_string(static_cast<int>(animationSpeed * 500)) + "%";
    int speedTextSize = MeasureText(speedText.c_str(), 20);
    DrawTextEx(ralewayFont, speedText.c_str(), { speedBar.x + (speedBar.width - speedTextSize) / 2, speedBar.y - 25 }, 20, 1, BLACK);

    DrawRectangleRec(playButton, GREEN);
    DrawRectangleRec(pauseButton, RED);
    DrawRectangleRec(forwardButton, GRAY);
    DrawRectangleRec(backwardButton, GRAY);
    DrawTextEx(ralewayFont, "Play", { playButton.x + 5, playButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "Pause", { pauseButton.x + 5, pauseButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, ">>", { forwardButton.x + 10, forwardButton.y + 5 }, 20, 1, WHITE);
    DrawTextEx(ralewayFont, "<<", { backwardButton.x + 10, backwardButton.y + 5 }, 20, 1, WHITE);
}

void AVLTreeVisualizer::animateInsert(int value) {
    currentOperation = "insert";
    operationValue = value;
    pendingInsertValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    setNotificationMessage("Inserting " + std::to_string(value));
    currentState = TRAVERSING;
}

void AVLTreeVisualizer::animateDelete(int value) {
    currentOperation = "delete";
    operationValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    setNotificationMessage("Deleting " + std::to_string(value));
    currentState = TRAVERSING;
    highlightNodes.clear();
    if (!currentPath.empty()) highlightNodes.insert(currentPath[pathIndex]);
}

void AVLTreeVisualizer::animateSearch(int value) {
    currentOperation = "search";
    operationValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    searchFound = tree.search(value);
    setNotificationMessage("Searching " + std::to_string(value));
    currentState = TRAVERSING;
}

void AVLTreeVisualizer::animateRandom() {
    tree.~AVLTree();
    new (&tree) AVLTree(); 
    int* randomValues = LoadRandomSequence(15, 1, 100);
    for (int i = 0; i < 15; i++) {
        tree.insert(tree.root, randomValues[i]);
    }
    AVLTree treeReplica(tree);
    treeUndoState.push(treeReplica);
    std::cout << treeUndoState.size() << std::endl;
    UnloadRandomSequence(randomValues);
    currentOperation = "random";
    currentPath.clear();
    pathIndex = 0;
    currentState = SHOWING_RESULT;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
}

void AVLTreeVisualizer::animateClear() {
    tree.~AVLTree();
    new (&tree) AVLTree();
    currentOperation = "";
    currentPath.clear();
    pathIndex = 0;
    currentState = IDLE;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
}

void AVLTreeVisualizer::animateLoadFile() {
    std::string filePath = "numbers.txt";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        notificationMessage = "Failed to open file: " + filePath;
        currentOperation = "loadfile";
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        return;
    }

    tree.~AVLTree();
    new (&tree) AVLTree();
    int value;
    while (file >> value) {
        tree.insert(tree.root, value);
    }
    file.close();

    AVLTree treeReplica(tree);
    treeUndoState.push(treeReplica);
    notificationMessage = "Loaded data from " + filePath;
    currentOperation = "loadfile";
    currentPath.clear();
    pathIndex = 0;
    currentState = SHOWING_RESULT;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
}

void AVLTreeVisualizer::animatePrevious() {
    if (!treeUndoState.empty()) {
        AVLTree treeReplica(tree);
        treeRedoState.push(treeReplica);
        tree = treeUndoState.top();
        treeUndoState.pop();
        currentOperation = "";
        currentPath.clear();
        pathIndex = 0;
        currentState = IDLE;
        stateTimer = 0.0f;
        resultTimer = 0.0f;
    }
}

void AVLTreeVisualizer::animateNext() {
    if (!treeRedoState.empty()) {
        AVLTree treeReplica(tree);
        treeUndoState.push(treeReplica);
        tree = treeRedoState.top();
        treeRedoState.pop();
        currentOperation = "";
        currentPath.clear();
        pathIndex = 0;
        currentState = IDLE;
        stateTimer = 0.0f;
        resultTimer = 0.0f;
    }
}

std::string AVLTreeVisualizer::getPseudocode() {
    std::stringstream pseudocode;
    if (currentOperation == "insert") {
        pseudocode << "Pseudocode for Insert Operation:\n"
                   << "1. Start at the root.\n"
                   << "2. Compare the value with the current node.\n"
                   << "3. If the value is smaller, move left; if larger, move right.\n"
                   << "4. Insert the value when a leaf node is found.\n"
                   << "5. Rebalance the tree if needed.\n";
    } 
    else if (currentOperation == "delete") {
        pseudocode << "Pseudocode for Delete Operation:\n"
                   << "1. Find the node to delete.\n"
                   << "2. If it has one or no child, replace it with its child or null.\n"
                   << "3. If it has two children, find the inorder successor.\n"
                   << "4. Replace the node with its inorder successor.\n"
                   << "5. Rebalance the tree if needed.\n";
    } 
    else if (currentOperation == "search") {
        pseudocode << "Pseudocode for Search Operation:\n"
                   << "1. Start at the root.\n"
                   << "2. Compare the value with the current node.\n"
                   << "3. If the value is smaller, move left; if larger, move right.\n"
                   << "4. Return the node if the value is found.\n"
                   << "5. If not found, return null.\n";
    } 
    else {
        pseudocode << "No operation selected.\n";
    }
    return pseudocode.str();
}

void AVLTreeVisualizer::setNotificationMessage(const std::string& message) {
    notificationMessage = message;
    stateTimer = 0.0f;
    currentState = SHOWING_RESULT;
}