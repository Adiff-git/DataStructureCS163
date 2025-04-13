#include "../inc/AVL.h"

#include "../inc/animation.h"
#include "../inc/Button.h"
#include <sstream>
#include <cstdlib>
#include <stack>
#include <iostream>
#include <cmath>
#include <fstream>
#include "../inc/tinyfiledialogs.h"

std::stack<AVLTree> treeUndoState;
std::stack<AVLTree> treeRedoState;
#define NODE_RADIUS 20.0f

AVLTreeVisualizer::AVLTreeVisualizer() 
    : inputText(""), 
      inputActive(false), 
      handleSpace{0.0f, 0.0f, 1500.0f, 75.0f}, 
      inputBox{20.0f, 20.0f, 200.0f, 30.0f}, 
      currentState(IDLE),
      pathIndex(0),
      rotationIndex(0),
      searchFound(false),
      stateTimer(0.0f),
      resultTimer(0.0f),
      operationValue(0),
      pendingInsertValue(0),
      animationSpeed(0.5f),
      draggingSlider(false),
      paused(false),
      initButton(10.0f, 0.0f, 80.0f, 30.0f, "Init", WHITE, WHITE, GetFontDefault(), 20),
      insertButton(90.0f, 0.0f, 80.0f, 30.0f, "Insert", WHITE, WHITE, GetFontDefault(), 20),
      deleteButton(170.0f, 0.0f, 80.0f, 30.0f, "Delete", WHITE, WHITE, GetFontDefault(), 20),
      searchButton(250.0f, 0.0f, 80.0f, 30.0f, "Search", WHITE, WHITE, GetFontDefault(), 20),
      loadFileButton(330.0f, 0.0f, 80.0f, 30.0f, "Load", WHITE, WHITE, GetFontDefault(), 20),
      rewindButton(0.0f, 0.0f, 70.0f, 30.0f, "|<<", WHITE, WHITE, GetFontDefault(), 20),
      previousButton(0.0f, 0.0f, 70.0f, 30.0f, "Prev", WHITE, WHITE, GetFontDefault(), 20),
      playPauseButton(0.0f, 0.0f, 70.0f, 30.0f, "Pause", WHITE, WHITE, GetFontDefault(), 20),
      nextButton(0.0f, 0.0f, 70.0f, 30.0f, "Next", WHITE, WHITE, GetFontDefault(), 20),
      fastForwardButton(0.0f, 0.0f, 70.0f, 30.0f, ">>|", WHITE, WHITE, GetFontDefault(), 20)
{
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();

    float buttonWidth = 80.0f;
    float controlButtonWidth = 70.0f;
    float buttonHeight = 30.0f;
    float buttonSpacing = 5.0f;

    float speedBarWidth = 390.0f;
    float speedBarHeight = 20.0f;
    float speedBarStartX = (screenWidth / 2.0f) - (speedBarWidth / 2.0f);
    float speedBarY = screenHeight - speedBarHeight - 70.0f;
    speedBar = { speedBarStartX, speedBarY, speedBarWidth, speedBarHeight };
    sliderHandle = { speedBar.x + (animationSpeed * (speedBar.width - 10.0f)), speedBar.y + 2.0f, 10.0f, 16.0f };

    float controlBarWidth = (controlButtonWidth + buttonSpacing) * 5 - buttonSpacing;
    float controlBarStartX = (screenWidth / 2.0f) - (controlBarWidth / 2.0f);
    float controlBarY = speedBar.y + speedBarHeight + 10.0f;

    float leftButtonY = speedBar.y - buttonHeight - 10.0f;

    Font defaultFont = GetFontDefault();

    initButton = Button(10.0f, leftButtonY, buttonWidth, buttonHeight, "Init", GRAY, WHITE, defaultFont, 20);
    insertButton = Button(90.0f, leftButtonY, buttonWidth, buttonHeight, "Insert", GRAY, WHITE, defaultFont, 20);
    deleteButton = Button(170.0f, leftButtonY, buttonWidth, buttonHeight, "Delete", GRAY, WHITE, defaultFont, 20);
    searchButton = Button(250.0f, leftButtonY, buttonWidth, buttonHeight, "Search", GRAY, WHITE, defaultFont, 20);
    loadFileButton = Button(330.0f, leftButtonY, buttonWidth, buttonHeight, "Load", GRAY, WHITE, defaultFont, 20);
    rewindButton = Button(controlBarStartX + 0 * (controlButtonWidth + buttonSpacing), controlBarY, controlButtonWidth, buttonHeight, "|<<", GRAY, WHITE, defaultFont, 20);
    previousButton = Button(controlBarStartX + 1 * (controlButtonWidth + buttonSpacing), controlBarY, controlButtonWidth, buttonHeight, "Prev", GRAY, WHITE, defaultFont, 20);
    playPauseButton = Button(controlBarStartX + 2 * (controlButtonWidth + buttonSpacing), controlBarY, controlButtonWidth, buttonHeight, "Pause", GRAY, WHITE, defaultFont, 20);
    nextButton = Button(controlBarStartX + 3 * (controlButtonWidth + buttonSpacing), controlBarY, controlButtonWidth, buttonHeight, "Next", GRAY, WHITE, defaultFont, 20);
    fastForwardButton = Button(controlBarStartX + 4 * (controlButtonWidth + buttonSpacing), controlBarY, controlButtonWidth, buttonHeight, ">>|", GRAY, WHITE, defaultFont, 20);
}

AVLTreeVisualizer::~AVLTreeVisualizer() {
    // No font to unload
}

void AVLTreeVisualizer::handleInput() {
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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

    if (initButton.isClicked()) {
        animateRandom();
        inputText.clear();
    }
    if (insertButton.isClicked() && !inputText.empty()) {
        int insertValue = std::stoi(inputText);
        animateInsert(insertValue);
        inputText.clear();
    }
    if (deleteButton.isClicked() && !inputText.empty()) {
        int deleteValue = std::stoi(inputText);
        animateDelete(deleteValue);
        inputText.clear();
    }
    if (searchButton.isClicked() && !inputText.empty()) {
        int searchValue = std::stoi(inputText);
        animateSearch(searchValue);
        inputText.clear();
    }
    if (loadFileButton.isClicked()) {
        animateLoadFile();
        inputText.clear();
    }

    if (previousButton.isClicked() && currentState == TRAVERSING) {
        if (pathIndex > 0) {
            pathIndex--;
            highlightNodes.erase(currentPath[pathIndex]);
            stateTimer = 0.0f;
        }
    }

    if (nextButton.isClicked() && currentState == TRAVERSING) {
        if (pathIndex < currentPath.size()) {
            pathIndex++;
            stateTimer = 0.0f;
        }
    }

    if (playPauseButton.isClicked()) {
        paused = !paused;
        playPauseButton.setLabel(paused ? "Play" : "Pause");
        playPauseButton.setBgColor(paused ? GREEN : RED);
    }

    if (rewindButton.isClicked() && currentState == TRAVERSING) {
        pathIndex = 0;
        highlightNodes.clear();
        stateTimer = 0.0f;
    }

    if (fastForwardButton.isClicked() && currentState == TRAVERSING) {
        pathIndex = currentPath.size();
        highlightNodes.clear();
        for (Node* node : currentPath) {
            highlightNodes.insert(node);
        }
        stateTimer = 0.0f;
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
            stateTimer = 0.0f;
        }
    }

    if (IsKeyPressed(KEY_C)) {
        animateClear();
        inputText.clear();
    }
    if (IsKeyPressed(KEY_L)) {
        animateLoadFile();
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
                    stateTimer = 0.0f;
                }
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
            if (stateTimer >= 0.5f) {
                if (pathIndex < currentPath.size()) {
                    Node* currentNode = currentPath[pathIndex];
                    highlightNodes.insert(currentNode);
                    notificationMessage = "Searching: " + std::to_string(currentNode->data);
                    pathIndex++;
                    stateTimer = 0.0f;
                } else {
                    if (searchFound) {
                        notificationMessage = "Found " + std::to_string(operationValue);
                        currentState = SHOWING_RESULT;
                        stateTimer = 0.0f;
                        resultTimer = 0.0f;
                    } else {
                        notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";
                        currentState = SEARCH_NOT_FOUND;
                        stateTimer = 0.0f;
                    }
                }
            }
            break;

        case SEARCH_NOT_FOUND:
            if (stateTimer >= 1.0f) {
                currentState = SHOWING_RESULT;
                highlightNodes.clear();
                stateTimer = 0.0f;
                resultTimer = 0.0f;
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
    DrawText(valueStr.c_str(), x - MeasureText(valueStr.c_str(), 20) / 2, y - 10, 20, BLACK);

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

    ClearBackground(BEIGE);
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);
    DrawText(inputText.c_str(), inputBox.x + 5, inputBox.y + 5, 20, BLACK);

    int titleSize = MeasureText("AVL Tree Visualizer", 30);
    float titleX = (GetScreenWidth() - titleSize) / 2.0f;
    float titleY = 37.0f;
    DrawText("AVL Tree Visualizer", titleX, titleY, 30, BLACK);

    int speedLabelSize = MeasureText("Speed", 20);
    Rectangle speedLabelBgRect = { speedBar.x, speedBar.y - 20.0f - 5, (float)speedLabelSize + 10, 30.0f };
    DrawRectangleRec(speedLabelBgRect, Fade(LIGHTGRAY, 0.8f));
    DrawText("Speed", speedBar.x + 5, speedBar.y - 20.0f, 20, BLACK);

    std::string speedText = "Speed: " + std::to_string(static_cast<int>(animationSpeed * 500)) + "%";
    int speedTextSize = MeasureText(speedText.c_str(), 20);
    float speedTextX = speedBar.x + (speedBar.width / 2.0f) - (speedTextSize / 2.0f);
    Rectangle speedTextBgRect = { speedTextX - 5, speedBar.y - 40.0f - 5, (float)speedTextSize + 10, 30.0f };
    DrawRectangleRec(speedTextBgRect, Fade(LIGHTGRAY, 0.8f));
    DrawText(speedText.c_str(), speedTextX, speedBar.y - 40.0f, 20, BLACK);

    DrawRectangleRec(speedBar, LIGHTGRAY);
    DrawRectangleLinesEx(speedBar, 2, GRAY);
    DrawRectangleRec(sliderHandle, BLUE);

    initButton.draw();
    insertButton.draw();
    deleteButton.draw();
    searchButton.draw();
    loadFileButton.draw();
    rewindButton.draw();
    previousButton.draw();
    playPauseButton.draw();
    nextButton.draw();
    fastForwardButton.draw();

    if (tree.root) {
        drawTree(tree.root, GetScreenWidth() / 2, 120, 200, highlightNodes);
    }

    if (!notificationMessage.empty()) {
        float maxWidth = static_cast<float>(GetScreenWidth()) * 0.8f;
        std::string displayMessage = notificationMessage;
        int messageSize = MeasureText(displayMessage.c_str(), 20);
        if (messageSize > maxWidth) {
            std::string truncatedMessage = displayMessage;
            while (MeasureText((truncatedMessage + "...").c_str(), 20) > maxWidth && !truncatedMessage.empty()) {
                truncatedMessage.pop_back();
            }
            displayMessage = truncatedMessage + "...";
            messageSize = MeasureText(displayMessage.c_str(), 20);
        }
        float yPos = 640.0f;
        float xPos = (GetScreenWidth() - messageSize) / 2.0f;
        Rectangle bgRect = { xPos - 5.0f, yPos - 5.0f, static_cast<float>(messageSize + 10), 25.0f };
        DrawRectangleRec(bgRect, Fade(LIGHTGRAY, 0.8f));
        DrawText(displayMessage.c_str(), xPos, yPos, 20, BLACK);
    }

    std::string pseudocodeText = getPseudocode();
    int lineHeight = 20;
    int lineCount = 0;
    std::istringstream stream(pseudocodeText);
    std::string line;
    while (std::getline(stream, line)) lineCount++;

    int pseudoCodeTopY = initButton.rect.y - (lineCount * lineHeight) - 10;
    stream.clear(); stream.str(pseudocodeText);
    int maxLineWidth = 0;
    while (std::getline(stream, line)) {
        int lineWidth = MeasureText(line.c_str(), 20);
        if (lineWidth > maxLineWidth) maxLineWidth = lineWidth;
    }

    Rectangle pseudoCodeBgRect = { 10.0f - 5, (float)pseudoCodeTopY - 5, (float)maxLineWidth + 10, (float)(lineCount * lineHeight) + 10 };
    DrawRectangleRec(pseudoCodeBgRect, Fade(LIGHTGRAY, 0.8f));

    int startY = pseudoCodeTopY;
    stream.clear(); stream.str(pseudocodeText);
    while (std::getline(stream, line)) {
        DrawText(line.c_str(), 10, startY, 20, BLACK);
        startY += lineHeight;
    }
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
    highlightNodes.clear();
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
    const char* filePath = tinyfd_openFileDialog(
        "Select a Text File",
        "",
        1,
        (const char*[]){"*.txt"},
        "Text files (*.txt)",
        0
    );

    if (!filePath) {
        notificationMessage = "No file selected";
        currentOperation = "loadfile";
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        return;
    }

    std::cout << "Selected file path: " << filePath << std::endl;

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        notificationMessage = "Failed to open file: " + std::string(filePath);
        currentOperation = "loadfile";
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        return;
    }

    std::cout << "File opened successfully: " << filePath << std::endl;

    char bom[3];
    file.read(bom, 3);
    if (!(bom[0] == (char)0xEF && bom[1] == (char)0xBB && bom[2] == (char)0xBF)) {
        file.seekg(0);
    }

    tree.~AVLTree();
    new (&tree) AVLTree();

    std::string line;
    int numberCount = 0;
    bool hasContent = false;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        std::cout << "Read line: '" << line << "'" << std::endl;
        if (!line.empty()) {
            hasContent = true;
            std::istringstream iss(line);
            std::string token;
            while (iss >> token) {
                std::cout << "Read token: " << token << std::endl;
                try {
                    int value = std::stoi(token);
                    std::cout << "Inserting: " << value << std::endl;
                    tree.insert(tree.root, value);
                    numberCount++;
                } catch (const std::invalid_argument&) {
                    std::cout << "Invalid number: " << token << std::endl;
                    continue;
                } catch (const std::out_of_range&) {
                    std::cout << "Out of range: " << token << std::endl;
                    continue;
                }
            }
        } else {
            std::cout << "Empty line encountered" << std::endl;
        }
    }

    file.close();

    std::cout << "Tree after loading: ";
    tree.printInOrder();
    std::cout << std::endl;

    if (numberCount > 0) {
        AVLTree treeReplica(tree);
        treeUndoState.push(treeReplica);
        notificationMessage = "Loaded " + std::to_string(numberCount) + " numbers from " + std::string(filePath);
    } else if (!hasContent) {
        notificationMessage = "File is empty: " + std::string(filePath);
    } else {
        notificationMessage = "No valid numbers found in " + std::string(filePath);
    }

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
    } else if (currentOperation == "delete") {
        pseudocode << "Pseudocode for Delete Operation:\n"
                   << "1. Find the node to delete.\n"
                   << "2. If it has one or no child, replace it with its child or null.\n"
                   << "3. If it has two children, find the inorder successor.\n"
                   << "4. Replace the node with its inorder successor.\n"
                   << "5. Rebalance the tree if needed.\n";
    } else if (currentOperation == "search") {
        pseudocode << "Pseudocode for Search Operation:\n"
                   << "1. Start at the root.\n"
                   << "2. Compare the value with the current node.\n"
                   << "3. If the value is smaller, move left; if larger, move right.\n"
                   << "4. Return the node if the value is found.\n"
                   << "5. If not found, return null.\n";
    } else {
        pseudocode << "No operation selected.\n";
    }
    return pseudocode.str();
}

void AVLTreeVisualizer::setNotificationMessage(const std::string& message) {
    notificationMessage = message;
    stateTimer = 0.0f;
    currentState = SHOWING_RESULT;
}

std::string AVLTreeVisualizer::getNotificationMessage() const {
    return notificationMessage;
}