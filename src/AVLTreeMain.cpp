#include "AVLTreeMain.h"

#include "AVLTree.h"

#include "Button.h"

#include "tinyfiledialogs.h"

#include <sstream>

#include <cstdlib>

#include <stack>

#include <iostream>

#include <cmath>

#include <fstream>



std::stack<AVLTree> treeUndoState;

std::stack<AVLTree> treeRedoState;

#define NODE_RADIUS 20.0f



AVLTreeVisualizer::AVLTreeVisualizer() 

    : inputText(""),

      inputActive(false),

      handleSpace{0.0f, 0.0f, 1500.0f, 75.0f},

      inputBox{10.0f, 610.0f, 100.0f, 25.0f}, // Điều chỉnh tọa độ giống Linked List

      initButtonRect{30.0f, 700.0f, 80.0f, 30.0f},

      insertButtonRect{130.0f, 700.0f, 80.0f, 30.0f},

      deleteButtonRect{230.0f, 700.0f, 80.0f, 30.0f},

      searchButtonRect{330.0f, 700.0f, 80.0f, 30.0f},

      loadFileButtonRect{430.0f, 700.0f, 80.0f, 30.0f},

      rewindButtonRect{600.0f, 700.0f, 80.0f, 30.0f}, // Điều chỉnh giống Linked List

      previousButtonRect{700.0f, 700.0f, 80.0f, 30.0f},

      playPauseButtonRect{800.0f, 700.0f, 80.0f, 30.0f},

      nextButtonRect{900.0f, 700.0f, 80.0f, 30.0f},

      fastForwardButtonRect{1000.0f, 700.0f, 80.0f, 30.0f},

      randomButtonRect{120.0f, 610.0f, 25.0f, 25.0f}, // Điều chỉnh giống Linked List

      speedBar{600.0f + 35, 670.0f, 280.0f, 20.0f}, // Điều chỉnh giống Linked List

      sliderHandle{0.0f, 0.0f, 10.0f, 16.0f},

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

      initButtonClicked(false),

      insertButtonClicked(false),

      deleteButtonClicked(false),

      searchButtonClicked(false),

      loadFileButtonClicked(false),

      rewindButtonClicked(false),

      previousButtonClicked(false),

      playPauseButtonClicked(false),

      nextButtonClicked(false),

      fastForwardButtonClicked(false),

      randomButtonClicked(false),

      initButtonMessage("Init"),

      insertButtonMessage("Insert"),

      deleteButtonMessage("Delete"),

      searchButtonMessage("Search"),

      loadFileButtonMessage("Load"),

      rewindButtonMessage("|<<"),

      previousButtonMessage("Prev"),

      playPauseButtonMessage("Pause"),

      nextButtonMessage("Next"),

      fastForwardButtonMessage(">>|"),

      randomButtonMessage("?"),

      manualStepping(false),

      stateHistoryIndex(0),

      shouldClose(false), // Khởi tạo biến

      backButtonClicked(false) // Khởi tạo biến

{

    sliderHandle = { speedBar.x + (animationSpeed * (speedBar.width - 10.0f)), speedBar.y + 2.0f, 10.0f, 16.0f };

    // Tải hình ảnh nút "Back"

    backButtonTexture = LoadTexture("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\images\\back_button.png");

    if (backButtonTexture.id == 0) {

        std::cerr << "Failed to load back_button.png for AVLTree\n";

    }

}



AVLTreeVisualizer::~AVLTreeVisualizer() {

    // Giải phóng texture của nút "Back"

    UnloadTexture(backButtonTexture);

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



    Font defaultFont = GetFontDefault();

    const char* tempMessage = nullptr;



    if (DrawButton("Init", initButtonRect, defaultFont, initButtonClicked, tempMessage)) {

        animateRandom();

        inputText.clear();

    }

    if (DrawButton("Insert", insertButtonRect, defaultFont, insertButtonClicked, tempMessage) && !inputText.empty()) {

        int insertValue = std::stoi(inputText);

        animateInsert(insertValue);

        inputText.clear();

    }

    if (DrawButton("Delete", deleteButtonRect, defaultFont, deleteButtonClicked, tempMessage) && !inputText.empty()) {

        int deleteValue = std::stoi(inputText);

        animateDelete(deleteValue);

        inputText.clear();

    }

    if (DrawButton("Search", searchButtonRect, defaultFont, searchButtonClicked, tempMessage) && !inputText.empty()) {

        int searchValue = std::stoi(inputText);

        animateSearch(searchValue);

        inputText.clear();

    }

    if (DrawButton("Load", loadFileButtonRect, defaultFont, loadFileButtonClicked, tempMessage)) {

        animateLoadFile();

        inputText.clear();

    }

    if (DrawButton("|<<", rewindButtonRect, defaultFont, rewindButtonClicked, tempMessage)) {

        animatePrevious();

        inputText.clear();

    }

    if (DrawButton("Prev", previousButtonRect, defaultFont, previousButtonClicked, tempMessage)) {

        stepBackward();

        inputText.clear();

    }

    if (DrawButton(paused ? "Play" : "Pause", playPauseButtonRect, defaultFont, playPauseButtonClicked, tempMessage)) {

        paused = !paused;

        playPauseButtonMessage = paused ? "Play" : "Pause";

    }

    if (DrawButton("Next", nextButtonRect, defaultFont, nextButtonClicked, tempMessage)) {

        stepForward();

        inputText.clear();

    }

    if (DrawButton(">>|", fastForwardButtonRect, defaultFont, fastForwardButtonClicked, tempMessage)) {

        if (currentState != IDLE && !currentPath.empty()) {

            paused = true;

            playPauseButtonMessage = "Play";



            pathIndex = currentPath.size();

            highlightNodes.clear();

            for (Node* node : currentPath) {

                highlightNodes.insert(node);

            }



            if (currentOperation == "insert") {

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

            } else if (currentOperation == "delete") {

                if (!tree.search(operationValue)) {

                    notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";

                } else {

                    tree.remove(tree.root, operationValue);

                    AVLTree treeReplica(tree);

                    treeUndoState.push(treeReplica);

                    notificationMessage = "Node " + std::to_string(operationValue) + " Deleted";

                }

                currentState = SHOWING_RESULT;

            } else if (currentOperation == "search") {

                if (searchFound) {

                    notificationMessage = "Found " + std::to_string(operationValue);

                } else {

                    notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";

                }

                currentState = SHOWING_RESULT;

            }

            stateTimer = 0.0f;

            resultTimer = 0.0f;

        }

    }

    if (DrawButton("?", randomButtonRect, defaultFont, randomButtonClicked, tempMessage)) {

        int randomValue = GetRandomValue(1, 100);

        inputText = std::to_string(randomValue);

        inputActive = false;

    }



    if (IsKeyPressed(KEY_C)) {

        animateClear();

        inputText.clear();

    }

    if (IsKeyPressed(KEY_L)) {

        animateLoadFile();

        inputText.clear();

    }



    // Xử lý tương tác với thanh tốc độ

    if (CheckCollisionPointRec(GetMousePosition(), speedBar) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

        float mouseX = GetMouseX();

        float t = (mouseX - speedBar.x) / (speedBar.width - 10.0f);

        t = std::max(0.0f, std::min(t, 1.0f)); // Giới hạn t trong [0, 1]



        // Tính animationSpeed từ t (ánh xạ phi tuyến giống Linked List)

        float minSpeed = 0.1f;

        float maxSpeed = 10.0f;

        if (t <= 0.5f) {

            // Đoạn từ t = 0 đến t = 0.5 ánh xạ từ speed = 0.1 đến speed = 1

            animationSpeed = minSpeed + (t / 0.5f) * (1.0f - minSpeed);

        } else {

            // Đoạn từ t = 0.5 đến t = 1 ánh xạ từ speed = 1 đến speed = 10

            animationSpeed = 1.0f + ((t - 0.5f) / 0.5f) * (maxSpeed - 1.0f);

        }

        animationSpeed = std::max(minSpeed, std::min(animationSpeed, maxSpeed));

    }

    sliderHandle.x = speedBar.x + (animationSpeed - 0.1f) / (10.0f - 0.1f) * (speedBar.width - 10.0f);

}



void AVLTreeVisualizer::stepBackward() {

    if (currentState == IDLE || currentPath.empty()) {

        return;

    }



    paused = true;

    playPauseButtonMessage = "Play";



    if (currentState == TRAVERSING || currentState == SEARCHING) {

        if (pathIndex > 0) {

            pathIndex--;

            highlightNodes.clear();

            for (int i = 0; i < pathIndex; i++) {

                highlightNodes.insert(currentPath[i]);

            }

            if (pathIndex > 0) {

                notificationMessage = "Traverse: " + std::to_string(currentPath[pathIndex - 1]->data);

            } else {

                notificationMessage = "Start of traversal";

            }

            stateTimer = 0.0f;

        }

    } else if (currentState == INSERTING) {

        currentState = TRAVERSING;

        pathIndex = currentPath.size();

        highlightNodes.clear();

        for (Node* node : currentPath) {

            highlightNodes.insert(node);

        }

        notificationMessage = "Traverse: " + std::to_string(currentPath[pathIndex - 1]->data);

        stateTimer = 0.0f;

    } else if (currentState == HIGHLIGHTING_BEFORE_DELETE) {

        currentState = TRAVERSING;

        pathIndex = currentPath.size();

        highlightNodes.clear();

        for (Node* node : currentPath) {

            highlightNodes.insert(node);

        }

        notificationMessage = "Traverse: " + std::to_string(currentPath[pathIndex - 1]->data);

        stateTimer = 0.0f;

    } else if (currentState == DELETING) {

        currentState = HIGHLIGHTING_BEFORE_DELETE;

        notificationMessage = "Deleting " + std::to_string(operationValue);

        stateTimer = 0.0f;

    } else if (currentState == SHOWING_RESULT) {

        if (currentOperation == "insert") {

            currentState = INSERTING;

            notificationMessage = "Inserting " + std::to_string(pendingInsertValue);

        } else if (currentOperation == "delete") {

            currentState = DELETING;

            notificationMessage = "Deleting " + std::to_string(operationValue);

        } else if (currentOperation == "search") {

            currentState = SEARCHING;

            pathIndex = currentPath.size();

            highlightNodes.clear();

            for (Node* node : currentPath) {

                highlightNodes.insert(node);

            }

            notificationMessage = "Searching: " + std::to_string(currentPath[pathIndex - 1]->data);

        }

        stateTimer = 0.0f;

        resultTimer = 0.0f;

    }

}



void AVLTreeVisualizer::stepForward() {

    if (currentState == IDLE || currentPath.empty()) {

        return;

    }



    paused = true;

    playPauseButtonMessage = "Play";



    if (currentState == TRAVERSING || currentState == SEARCHING) {

        if (pathIndex < currentPath.size()) {

            Node* currentNode = currentPath[pathIndex];

            highlightNodes.insert(currentNode);

            notificationMessage = "Traverse: " + std::to_string(currentNode->data);

            pathIndex++;

            stateTimer = 0.0f;

        }

        if (pathIndex >= currentPath.size()) {

            if (currentOperation == "insert") {

                currentState = INSERTING;

                notificationMessage = "Inserting " + std::to_string(pendingInsertValue);

            } else if (currentOperation == "delete") {

                if (!tree.search(operationValue)) {

                    notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";

                    currentState = SHOWING_RESULT;

                } else {

                    currentState = HIGHLIGHTING_BEFORE_DELETE;

                    notificationMessage = "Deleting " + std::to_string(operationValue);

                }

            } else if (currentOperation == "search") {

                if (pathIndex >= currentPath.size()) {

                    if (searchFound) {

                        notificationMessage = "Found " + std::to_string(operationValue);

                        currentState = SHOWING_RESULT;

                    } else {

                        notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";

                        currentState = SEARCH_NOT_FOUND;

                    }

                    stateTimer = 0.0f;

                    resultTimer = 0.0f;

                }

            }

        }

    } else if (currentState == INSERTING) {

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

    } else if (currentState == HIGHLIGHTING_BEFORE_DELETE) {

        notificationMessage = "Deleting " + std::to_string(operationValue);

        currentState = DELETING;

        stateTimer = 0.0f;

    } else if (currentState == DELETING) {

        tree.remove(tree.root, operationValue);

        AVLTree treeReplica(tree);

        treeUndoState.push(treeReplica);

        notificationMessage = "Node " + std::to_string(operationValue) + " Deleted";

        currentState = SHOWING_RESULT;

        resultTimer = 0.0f;

        stateTimer = 0.0f;

    } else if (currentState == SEARCH_NOT_FOUND) {

        currentState = SHOWING_RESULT;

        highlightNodes.clear();

        stateTimer = 0.0f;

        resultTimer = 0.0f;

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

                notificationMessage = "Node " + std::to_string(operationValue) + " Deleted";

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

    DrawCircleLines(x, y, NODE_RADIUS, BLACK);

    std::string valueStr = std::to_string(node->data);

    DrawText(valueStr.c_str(), x - static_cast<float>(MeasureText(valueStr.c_str(), 20)) / 2.0f, y - 10.0f, 20, BLACK);



    if (node->left) {

        float leftX = tree.getSubtreeWidth(node->left->right);

        float hypotenuse = sqrt(60.0f * 60.0f + leftX * leftX);

        float startLineX = x - NODE_RADIUS * leftX / hypotenuse;

        float startLineY = y + NODE_RADIUS * 60.0f / hypotenuse;

        DrawLine(startLineX, startLineY, x - leftX, y + 60.0f, BLACK);

        drawTree(node->left, x - leftX, y + 60.0f, offset / 2.0f, highlight);

    }



    if (node->right) {

        float rightX = tree.getSubtreeWidth(node->right->left);

        float hypotenuse = sqrt(60.0f * 60.0f + rightX * rightX);

        float startLineX = x + NODE_RADIUS * rightX / hypotenuse;

        float startLineY = y + NODE_RADIUS * 60.0f / hypotenuse;

        DrawLine(startLineX, startLineY, x + rightX, y + 60.0f, BLACK);

        drawTree(node->right, x + rightX, y + 60.0f, offset / 2.0f, highlight);

    }

}



void AVLTreeVisualizer::draw() {

    std::set<Node*> highlightNodes(currentPath.begin(), currentPath.begin() + pathIndex);



    ClearBackground(WHITE);



    // Draw pseudocode first to calculate its width

    std::string pseudocodeText = getPseudocode();

    int lineHeight = 20;

    int lineCount = 0;

    std::istringstream stream(pseudocodeText);

    std::string line;

    std::vector<std::string> lines;

    while (std::getline(stream, line)) {

        lines.push_back(line);

        lineCount++;

    }



    int pseudoCodeTopY = 50;

    int maxLineWidth = 0;

    for (const auto& l : lines) {

        int lineWidth = MeasureText(l.c_str(), 20);

        if (lineWidth > maxLineWidth) maxLineWidth = lineWidth;

    }



    // Calculate the offset to shift the title and tree further to the right

    float pseudoCodeRightEdge = 10.0f + static_cast<float>(maxLineWidth) + 10.0f; // Right edge of pseudocode background

    float offset = pseudoCodeRightEdge + 300.0f; // Increased buffer to 300 for more space



    // Draw title with offset

    int titleSize = MeasureText("AVL Tree Visualizer", 30);

    float titleX = offset;

    float titleY = 37.0f;

    DrawText("AVL Tree Visualizer", titleX, titleY, 30, BLACK);



    // Draw speed slider and label

    int speedLabelSize = MeasureText("Speed", 20);

    Rectangle speedLabelBgRect = { speedBar.x - 35.0f, speedBar.y - 20.0f - 5.0f, static_cast<float>(speedLabelSize) + 10.0f, 30.0f };

    DrawRectangleRec(speedLabelBgRect, Fade(LIGHTGRAY, 0.8f));

    DrawText("Speed", speedBar.x - 30.0f, speedBar.y - 20.0f, 20, BLACK);



    // Hiển thị giá trị tốc độ (tương tự Linked List)

    char* speedText = (char*)malloc(20);

    sprintf(speedText, "x%.2f", animationSpeed * 2);

    DrawText(speedText, speedBar.x + speedBar.width + 10.0f, speedBar.y, 20, BLACK);

    free(speedText);



    // Tính toán vị trí của nút kéo dựa trên tốc độ

    float t;

    float minSpeed = 0.1f;

    float maxSpeed = 10.0f;

    if (animationSpeed <= 1.0f) {

        // Đoạn từ 0.1 đến 1 ánh xạ từ t = 0 đến t = 0.5

        t = (animationSpeed - minSpeed) / (1.0f - minSpeed) * 0.5f;

    } else {

        // Đoạn từ 1 đến 10 ánh xạ từ t = 0.5 đến t = 1

        t = 0.5f + (animationSpeed - 1.0f) / (maxSpeed - 1.0f) * 0.5f;

    }

    float sliderPos = t * (speedBar.width - 10.0f);

    Rectangle thumbRect = { speedBar.x + sliderPos, speedBar.y, 10.0f, speedBar.height };

    DrawRectangleRec(speedBar, LIGHTGRAY);

    DrawRectangleRec(thumbRect, DARKGRAY);



    // Draw buttons with centered text

    Font defaultFont = GetFontDefault();

    auto drawCenteredButton = [&](const char* text, Rectangle rect, bool& clicked, const std::string& message) {

        Vector2 mousePos = GetMousePosition();

        clicked = CheckCollisionPointRec(mousePos, rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        DrawRectangleRec(rect, LIGHTGRAY);

        DrawRectangleLinesEx(rect, 2, clicked ? BLUE : GRAY);



        int fontSize = (rect.width < 50) ? 16 : 20;

        int textSize = MeasureText(text, fontSize);

        float textX = rect.x + (rect.width - static_cast<float>(textSize)) / 2.0f;

        float textY = rect.y + (rect.height - static_cast<float>(fontSize)) / 2.0f;

        DrawText(text, textX, textY, fontSize, BLACK);

    };



    drawCenteredButton("Init", initButtonRect, initButtonClicked, initButtonMessage);

    drawCenteredButton("Insert", insertButtonRect, insertButtonClicked, insertButtonMessage);

    drawCenteredButton("Delete", deleteButtonRect, deleteButtonClicked, deleteButtonMessage);

    drawCenteredButton("Search", searchButtonRect, searchButtonClicked, searchButtonMessage);

    drawCenteredButton("Load", loadFileButtonRect, loadFileButtonClicked, loadFileButtonMessage);

    drawCenteredButton("|<<", rewindButtonRect, rewindButtonClicked, rewindButtonMessage);

    drawCenteredButton("Prev", previousButtonRect, previousButtonClicked, previousButtonMessage);

    drawCenteredButton(paused ? "Play" : "Pause", playPauseButtonRect, playPauseButtonClicked, playPauseButtonMessage);

    drawCenteredButton("Next", nextButtonRect, nextButtonClicked, nextButtonMessage);

    drawCenteredButton(">>|", fastForwardButtonRect, fastForwardButtonClicked, fastForwardButtonMessage);

    drawCenteredButton("?", randomButtonRect, randomButtonClicked, randomButtonMessage);



    // Draw tree with offset

    if (tree.root) {

        drawTree(tree.root, offset, 120.0f, 200.0f, highlightNodes);

    }



    // Draw notification

    if (!notificationMessage.empty()) {

        float maxWidth = static_cast<float>(GetScreenWidth()) * 0.8f;

        std::string displayMessage = notificationMessage;

        int messageSize = MeasureText(displayMessage.c_str(), 20);

        if (messageSize > maxWidth) {

            std::string truncatedMessage = notificationMessage;

            while (MeasureText((truncatedMessage + "...").c_str(), 20) > maxWidth && !truncatedMessage.empty()) {

                truncatedMessage.pop_back();

            }

            displayMessage = truncatedMessage + "...";

            messageSize = MeasureText(displayMessage.c_str(), 20);

        }

        float yPos = 740.0f;

        float xPos = (static_cast<float>(GetScreenWidth()) - static_cast<float>(messageSize)) / 2.0f;

        Rectangle bgRect = { xPos - 5.0f, yPos - 5.0f, static_cast<float>(messageSize) + 10.0f, 25.0f };

        DrawRectangleRec(bgRect, Fade(LIGHTGRAY, 0.8f));

        DrawText(displayMessage.c_str(), xPos, yPos, 20, BLACK);

    }



    // Draw pseudocode

    Rectangle pseudoCodeBgRect = { 10.0f - 5.0f, static_cast<float>(pseudoCodeTopY) - 5.0f, static_cast<float>(maxLineWidth) + 10.0f, static_cast<float>(lineCount * lineHeight) + 10.0f };

    DrawRectangleRec(pseudoCodeBgRect, Fade(LIGHTGRAY, 0.8f));



    int startY = pseudoCodeTopY;

    int lineIndex = 0;

    int activeLine = -1;



    if (currentOperation == "insert") {

        if (currentState == TRAVERSING) {

            if (pathIndex == 0) activeLine = 1;

            else if (pathIndex <= currentPath.size()) {

                int traversalLine = 2 + (pathIndex - 1) % 5;

                if (traversalLine <= 6) activeLine = traversalLine;

                else activeLine = 7;

            }

        } else if (currentState == INSERTING) {

            activeLine = 9;

        } else if (currentState == SHOWING_RESULT) {

            activeLine = 10;

        }

    } else if (currentOperation == "delete") {

        if (currentState == TRAVERSING || currentState == HIGHLIGHTING_BEFORE_DELETE) {

            if (pathIndex == 0) activeLine = 1;

            else if (pathIndex <= currentPath.size()) {

                int traversalLine = 2 + (pathIndex - 1) % 5;

                if (traversalLine <= 6) activeLine = traversalLine;

                else activeLine = 7;

            }

        } else if (currentState == DELETING) {

            activeLine = 10;

        } else if (currentState == SEARCH_NOT_FOUND) {

            activeLine = 9;

        } else if (currentState == SHOWING_RESULT) {

            activeLine = 11;

        }

    } else if (currentOperation == "search") {

        if (currentState == TRAVERSING || currentState == SEARCHING) {

            if (pathIndex == 0) activeLine = 1;

            else if (pathIndex <= currentPath.size()) {

                int traversalLine = 2 + (pathIndex - 1) % 5;

                if (traversalLine <= 6) activeLine = traversalLine;

                else activeLine = 7;

            }

        } else if (currentState == SEARCH_NOT_FOUND || (currentState == SHOWING_RESULT && !searchFound)) {

            activeLine = 9;

        } else if (currentState == SHOWING_RESULT && searchFound) {

            activeLine = 7;

        }

    }



    for (const auto& l : lines) {

        Color textColor = (lineIndex == activeLine) ? YELLOW : BLACK;

        DrawText(l.c_str(), 10, startY, 20, textColor);

        startY += lineHeight;

        lineIndex++;

    }



    DrawRectangleRec(inputBox, LIGHTGRAY);

    DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);

    DrawText(inputText.c_str(), inputBox.x + 5.0f, inputBox.y + 5.0f, 20, BLACK);



    // Vẽ nút "Back" ở góc trái trên với kích thước tương đương một node

    Rectangle backButtonBounds = { 20.0f, 20.0f, 65.0f, 65.0f }; // Kích thước nút 65x65 pixels, tương đương nodeWidth

    DrawBackButton(backButtonTexture, backButtonBounds, backButtonClicked);



    // Nếu nhấn ESC, quay lại màn hình chính

    if (IsKeyPressed(KEY_ESCAPE)) {

        shouldClose = true;

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

        const char* filterPatterns[] = {"*.txt"};

    const char* filePath = tinyfd_openFileDialog(

        "Select a Text File",

        "",

        1,

        filterPatterns,

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



    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {

        notificationMessage = "Failed to open file: " + std::string(filePath);

        currentOperation = "loadfile";

        currentState = SHOWING_RESULT;

        resultTimer = 0.0f;

        return;

    }



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

        if (!line.empty()) {

            hasContent = true;

            std::istringstream iss(line);

            std::string token;

            while (iss >> token) {

                try {

                    int value = std::stoi(token);

                    tree.insert(tree.root, value);

                    numberCount++;

                } catch (const std::invalid_argument&) {

                    continue;

                } catch (const std::out_of_range&) {

                    continue;

                }

            }

        }

    }



    file.close();



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

        pseudocode << "Node* insert(Node* root, int value) {\n"

                   << "  Node* curr = root;\n"

                   << "  while (curr != nullptr) {\n"

                   << "    if (value < curr->data)\n"

                   << "      curr = curr->left;\n"

                   << "    else if (value > curr->data)\n"

                   << "      curr = curr->right;\n"

                   << "    else return curr; // Duplicate\n"

                   << "  }\n"

                   << "  curr = new Node(value);\n"

                   << "  rebalance(root);\n"

                   << "  return curr;\n"

                   << "}";

    } else if (currentOperation == "delete") {

        pseudocode << "Node* remove(Node* root, int value) {\n"

                   << "  Node* curr = root;\n"

                   << "  while (curr != nullptr) {\n"

                   << "    if (value < curr->data)\n"

                   << "      curr = curr->left;\n"

                   << "    else if (value > curr->data)\n"

                   << "      curr = curr->right;\n"

                   << "    else break; // Found\n"

                   << "  }\n"

                   << "  if (curr == nullptr) return root;\n"

                   << "  root = deleteNode(curr);\n"

                   << "  rebalance(root);\n"

                   << "  return root;\n"

                   << "}";

    } else if (currentOperation == "search") {

        pseudocode << "Node* search(Node* root, int value) {\n"

                   << "  Node* curr = root;\n"

                   << "  while (curr != nullptr) {\n"

                   << "    if (value < curr->data)\n"

                   << "      curr = curr->left;\n"

                   << "    else if (value > curr->data)\n"

                   << "      curr = curr->right;\n"

                   << "    else return curr; // Found\n"

                   << "  }\n"

                   << "  return nullptr; // Not found\n"

                   << "}";

    } else {

        pseudocode << "No operation selected;";

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



float AVLTreeVisualizer::UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue) {

    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, slider) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

        float t = (mousePos.x - slider.x) / slider.width;

        t = std::max(0.0f, std::min(t, 1.0f)); // Giới hạn t trong [0, 1]

        return minValue + t * (maxValue - minValue);

    }

    return currentValue;

}