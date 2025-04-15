#include "AVLTreeMain.h"
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
      inputBox{50, 610, 100, 25},
      initButtonRect{30, 700, 80, 30},
      insertButtonRect{130, 700, 80, 30},
      deleteButtonRect{230, 700, 80, 30},
      searchButtonRect{330, 700, 80, 30},
      loadFileButtonRect{430, 700, 80, 30},
      rewindButtonRect{600, 700, 80, 30},
      previousButtonRect{700, 700, 80, 30},
      playPauseButtonRect{800, 700, 80, 30},
      nextButtonRect{900, 700, 80, 30},
      fastForwardButtonRect{1000, 700, 80, 30},
      randomButtonRect{160, 610, 25, 25},
      speedBar{600 + 100, 670, 280, 20},
      sliderHandle{0, 0, 10, 16},
      currentState(IDLE),
      pathIndex(0),
      rotationIndex(0),
      searchFound(false),
      stateTimer(0.0f),
      resultTimer(0.0f),
      operationValue(0),
      pendingInsertValue(0),
      animationSpeed(1.0f),
      draggingSlider(false),
      paused(false),
      notificationMessage(""),
      curStep(0),
      totalSteps(0),
      currentStep(0),
      showInitOption(false),
      showInsertOption(false),
      showDeleteOption(false),
      showSearchOption(false),
      showLoadOption(false),
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
      rewindButtonMessage("First"),
      previousButtonMessage("Prev"),
      playPauseButtonMessage("Pause"),
      nextButtonMessage("Next"),
      fastForwardButtonMessage("Last"),
      randomButtonMessage("?"),
      shouldClose(false),
      backButtonClicked(false)
{
    sliderHandle = { speedBar.x + (animationSpeed - 0.1f) / (10.0f - 0.1f) * (speedBar.width - 10.0f), speedBar.y + 2.0f, 10.0f, 16.0f };
    backButtonTexture = LoadTexture("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\images\\BackButton.png");
}

AVLTreeVisualizer::~AVLTreeVisualizer() {
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

    // Kiểm tra nút "BackButton"
    if (backButtonClicked) {
        shouldClose = true;
        backButtonClicked = false; // Reset trạng thái sau khi xử lý
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        shouldClose = true;
    }
}

void AVLTreeVisualizer::resetAnimation() {
    curStep = 0;
    pathIndex = 0;
    rotationIndex = 0;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
    currentState = TRAVERSING;
    currentStep = 0;

    // Kiểm tra và truy cập vector an toàn
    if (currentOperation == "insert" && !insertPaths.empty()) {
        currentPath = insertPaths[0];
        highlightNodes = insertHighlightNodes[0];
        notificationMessage = insertNotifications[0];
        currentState = insertStates[0];
        tree = insertTreeStates[0]; // Khôi phục trạng thái cây
    } else if (currentOperation == "delete" && !deletePaths.empty()) {
        currentPath = deletePaths[0];
        highlightNodes = deleteHighlightNodes[0];
        notificationMessage = deleteNotifications[0];
        currentState = deleteStates[0];
        tree = deleteTreeStates[0]; // Khôi phục trạng thái cây
    } else if (currentOperation == "search" && !searchPaths.empty()) {
        currentPath = searchPaths[0];
        highlightNodes = searchHighlightNodes[0];
        notificationMessage = searchNotifications[0];
        currentState = searchStates[0];
        tree = searchTreeStates[0]; // Khôi phục trạng thái cây
    } else {
        currentPath.clear();
        highlightNodes.clear();
        currentState = IDLE;
        totalSteps = 0;
        currentStep = 0;
    }

    paused = true;
}

void AVLTreeVisualizer::drawOperationMenu() {
    Font defaultFont = GetFontDefault();

    DrawButton("Init", initButtonRect, defaultFont, initButtonClicked, initButtonMessage.c_str());
    if (initButtonClicked) {
        showInitOption = !showInitOption;
        showInsertOption = false;
        showDeleteOption = false;
        showSearchOption = false;
        showLoadOption = false;
        initButtonClicked = false;
    }
    if (showInitOption) {
        drawInitOptions();
    }

    DrawButton("Insert", insertButtonRect, defaultFont, insertButtonClicked, insertButtonMessage.c_str());
    if (insertButtonClicked) {
        showInsertOption = !showInsertOption;
        showInitOption = false;
        showDeleteOption = false;
        showSearchOption = false;
        showLoadOption = false;
        insertButtonClicked = false;
    }
    if (showInsertOption) {
        drawInsertOptions();
    }

    DrawButton("Delete", deleteButtonRect, defaultFont, deleteButtonClicked, deleteButtonMessage.c_str());
    if (deleteButtonClicked) {
        showDeleteOption = !showDeleteOption;
        showInitOption = false;
        showInsertOption = false;
        showSearchOption = false;
        showLoadOption = false;
        deleteButtonClicked = false;
    }
    if (showDeleteOption) {
        drawDeleteOptions();
    }

    DrawButton("Search", searchButtonRect, defaultFont, searchButtonClicked, searchButtonMessage.c_str());
    if (searchButtonClicked) {
        showSearchOption = !showSearchOption;
        showInitOption = false;
        showInsertOption = false;
        showDeleteOption = false;
        showLoadOption = false;
        searchButtonClicked = false;
    }
    if (showSearchOption) {
        drawSearchOptions();
    }

    DrawButton("Load", loadFileButtonRect, defaultFont, loadFileButtonClicked, loadFileButtonMessage.c_str());
    if (loadFileButtonClicked) {
        showLoadOption = !showLoadOption;
        showInitOption = false;
        showInsertOption = false;
        showDeleteOption = false;
        showSearchOption = false;
        loadFileButtonClicked = false;
    }
    if (showLoadOption) {
        drawLoadOptions();
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        Rectangle optionPos = {30, 580, 380, 120};
        if (showInitOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, initButtonRect)) {
            showInitOption = false;
        }
        if (showInsertOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, insertButtonRect)) {
            showInsertOption = false;
        }
        if (showDeleteOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, deleteButtonRect)) {
            showDeleteOption = false;
        }
        if (showSearchOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, searchButtonRect)) {
            showSearchOption = false;
        }
        if (showLoadOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, loadFileButtonRect)) {
            showLoadOption = false;
        }
    }
}

void AVLTreeVisualizer::drawInitOptions() {
    static int nValue = 0;
    static char nText[4] = "0";
    static bool nInputEnabled = false;
    static int nMax = 20;

    DrawText("N: ", 20, 610, 20, BLACK);
    DrawRectangleRec(inputBox, WHITE);
    DrawRectangleLinesEx(inputBox, 1, BLACK);
    if (nInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) nInputEnabled = false;
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && strlen(nText) < 3) {
                int len = strlen(nText);
                nText[len] = (char)key;
                nText[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(nText) > 0) {
            nText[strlen(nText) - 1] = '\0';
        }
        if (atoi(nText) > nMax) {
            nText[0] = '2';
            nText[1] = '0';
            nText[2] = '\0';
        }
    }
    nValue = atoi(nText);
    DrawText(nText, inputBox.x + 5, inputBox.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        nInputEnabled = true;
    }

    Font defaultFont = GetFontDefault();
    DrawButton("?", randomButtonRect, defaultFont, randomButtonClicked, randomButtonMessage.c_str());
    if (randomButtonClicked) {
        nValue = GetRandomValue(1, 20);
        sprintf(nText, "%d", nValue);
        randomButtonClicked = false;
    }

    Rectangle okRect = {350, 650, 40, 30};
    DrawButton("OK", okRect, defaultFont, initButtonClicked, initButtonMessage.c_str());
    if (initButtonClicked) {
        animateRandom(nValue);
        inputText.clear();
        showInitOption = false;
        initButtonClicked = false;
    }
}

void AVLTreeVisualizer::drawInsertOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;

    DrawText("V: ", 20, 610, 20, BLACK);
    DrawRectangleRec(inputBox, WHITE);
    DrawRectangleLinesEx(inputBox, 1, BLACK);
    if (valueInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) valueInputEnabled = false;
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && strlen(valueText) < 3) {
                int len = strlen(valueText);
                valueText[len] = (char)key;
                valueText[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(valueText) > 0) {
            valueText[strlen(valueText) - 1] = '\0';
        }
        if (atoi(valueText) > valueMax) {
            valueText[0] = '9';
            valueText[1] = '9';
            valueText[2] = '\0';
        }
    }
    value = atoi(valueText);
    DrawText(valueText, inputBox.x + 5, inputBox.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
    }

    Font defaultFont = GetFontDefault();
    DrawButton("?", randomButtonRect, defaultFont, randomButtonClicked, randomButtonMessage.c_str());
    if (randomButtonClicked) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        randomButtonClicked = false;
    }

    Rectangle okRect = {350, 650, 40, 30};
    DrawButton("OK", okRect, defaultFont, insertButtonClicked, insertButtonMessage.c_str());
    if (insertButtonClicked) {
        animateInsert(value);
        inputText.clear();
        showInsertOption = false;
        insertButtonClicked = false;
    }
}

void AVLTreeVisualizer::drawDeleteOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;

    DrawText("V: ", 20, 610, 20, BLACK);
    DrawRectangleRec(inputBox, WHITE);
    DrawRectangleLinesEx(inputBox, 1, BLACK);
    if (valueInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) valueInputEnabled = false;
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && strlen(valueText) < 3) {
                int len = strlen(valueText);
                valueText[len] = (char)key;
                valueText[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(valueText) > 0) {
            valueText[strlen(valueText) - 1] = '\0';
        }
        if (atoi(valueText) > valueMax) {
            valueText[0] = '9';
            valueText[1] = '9';
            valueText[2] = '\0';
        }
    }
    value = atoi(valueText);
    DrawText(valueText, inputBox.x + 5, inputBox.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
    }

    Font defaultFont = GetFontDefault();
    DrawButton("?", randomButtonRect, defaultFont, randomButtonClicked, randomButtonMessage.c_str());
    if (randomButtonClicked) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        randomButtonClicked = false;
    }

    Rectangle okRect = {350, 650, 40, 30};
    DrawButton("OK", okRect, defaultFont, deleteButtonClicked, deleteButtonMessage.c_str());
    if (deleteButtonClicked) {
        animateDelete(value);
        inputText.clear();
        showDeleteOption = false;
        deleteButtonClicked = false;
    }
}

void AVLTreeVisualizer::drawSearchOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;

    DrawText("V: ", 20, 610, 20, BLACK);
    DrawRectangleRec(inputBox, WHITE);
    DrawRectangleLinesEx(inputBox, 1, BLACK);
    if (valueInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) valueInputEnabled = false;
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && strlen(valueText) < 3) {
                int len = strlen(valueText);
                valueText[len] = (char)key;
                valueText[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(valueText) > 0) {
            valueText[strlen(valueText) - 1] = '\0';
        }
        if (atoi(valueText) > valueMax) {
            valueText[0] = '9';
            valueText[1] = '9';
            valueText[2] = '\0';
        }
    }
    value = atoi(valueText);
    DrawText(valueText, inputBox.x + 5, inputBox.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
    }

    Font defaultFont = GetFontDefault();
    DrawButton("?", randomButtonRect, defaultFont, randomButtonClicked, randomButtonMessage.c_str());
    if (randomButtonClicked) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        randomButtonClicked = false;
    }

    Rectangle okRect = {350, 650, 40, 30};
    DrawButton("OK", okRect, defaultFont, searchButtonClicked, searchButtonMessage.c_str());
    if (searchButtonClicked) {
        animateSearch(value);
        inputText.clear();
        showSearchOption = false;
        searchButtonClicked = false;
    }
}

void AVLTreeVisualizer::drawLoadOptions() {
    Font defaultFont = GetFontDefault();
    Rectangle uploadRect = {30, 650, 80, 30};
    DrawButton("Upload", uploadRect, defaultFont, loadFileButtonClicked, loadFileButtonMessage.c_str());
    if (loadFileButtonClicked) {
        animateLoadFile();
        inputText.clear();
        showLoadOption = false;
        loadFileButtonClicked = false;
    }
}

void AVLTreeVisualizer::drawAnimationMenu() {
    float buttonWidth = 80;
    float buttonHeight = 30;
    float spacing = 20;
    float sliderWidth = 280;
    float sliderHeight = 20;
    float posX = 600;
    float buttonPosY = 700;

    DrawRectangleRec(speedBar, LIGHTGRAY);
    float t;
    float minSpeed = 0.1f;
    float maxSpeed = 10.0f;
    if (animationSpeed <= 1.0f) {
        t = (animationSpeed - minSpeed) / (1.0f - minSpeed) * 0.5f;
    } else {
        t = 0.5f + (animationSpeed - 1.0f) / (maxSpeed - 1.0f) * 0.5f;
    }
    float sliderPos = t * (sliderWidth - 10.0f);
    Rectangle thumbRect = { speedBar.x + sliderPos, speedBar.y, 10.0f, speedBar.height };
    DrawRectangleRec(thumbRect, DARKGRAY);
    DrawText("Speed", posX + 35, 670, 20, BLACK);
    char* speedText = (char*)malloc(20);
    sprintf(speedText, "x%.2f", animationSpeed / 2);
    DrawText(speedText, speedBar.x + speedBar.width + 10, speedBar.y, 20, BLACK);
    free(speedText);

    if (CheckCollisionPointRec(GetMousePosition(), speedBar) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float mouseX = GetMouseX();
        t = (mouseX - speedBar.x) / (speedBar.width - 10.0f);
        t = std::max(0.0f, std::min(t, 1.0f));
        if (t <= 0.5f) {
            animationSpeed = minSpeed + (t / 0.5f) * (1.0f - minSpeed);
        } else {
            animationSpeed = 1.0f + ((t - 0.5f) / 0.5f) * (maxSpeed - 1.0f);
        }
        animationSpeed = std::max(minSpeed, std::min(animationSpeed, maxSpeed));
    }

    Font defaultFont = GetFontDefault();
    DrawButton("First", rewindButtonRect, defaultFont, rewindButtonClicked, rewindButtonMessage.c_str());
    if (rewindButtonClicked) {
        resetAnimation();
        paused = true;
        rewindButtonClicked = false;
    }

    DrawButton("Prev", previousButtonRect, defaultFont, previousButtonClicked, previousButtonMessage.c_str());
    if (previousButtonClicked) {
        stepBackward();
        paused = true;
        playPauseButtonMessage = "Play";
        previousButtonClicked = false;
    }

    DrawButton(paused ? "Play" : "Pause", playPauseButtonRect, defaultFont, playPauseButtonClicked, playPauseButtonMessage.c_str());
    if (playPauseButtonClicked) {
        paused = !paused;
        playPauseButtonMessage = paused ? "Play" : "Pause";
        playPauseButtonClicked = false;
    }

    DrawButton("Next", nextButtonRect, defaultFont, nextButtonClicked, nextButtonMessage.c_str());
    if (nextButtonClicked) {
        stepForward();
        paused = true;
        playPauseButtonMessage = "Play";
        nextButtonClicked = false;
    }

    DrawButton("Last", fastForwardButtonRect, defaultFont, fastForwardButtonClicked, fastForwardButtonMessage.c_str());
    if (fastForwardButtonClicked) {
        // Kiểm tra xem có bước nào để tua không
        if (totalSteps == 0) {
            fastForwardButtonClicked = false;
            return;
        }

        paused = true;
        playPauseButtonMessage = "Play";

        currentStep = totalSteps - 1;

        if (currentOperation == "insert" && currentStep < insertPaths.size()) {
            currentPath = insertPaths[currentStep];
            highlightNodes = insertHighlightNodes[currentStep];
            notificationMessage = insertNotifications[currentStep];
            currentState = insertStates[currentStep];
            tree = insertTreeStates[currentStep]; // Khôi phục trạng thái cây
            pathIndex = currentStep;

            if (currentState == INSERTING || currentState == SHOWING_RESULT) {
                if (!tree.search(pendingInsertValue)) {
                    tree.insert(tree.root, pendingInsertValue);
                    AVLTree treeReplica(tree);
                    treeUndoState.push(treeReplica);
                }
            }
        } else if (currentOperation == "delete" && currentStep < deletePaths.size()) {
            currentPath = deletePaths[currentStep];
            highlightNodes = deleteHighlightNodes[currentStep];
            notificationMessage = deleteNotifications[currentStep];
            currentState = deleteStates[currentStep];
            tree = deleteTreeStates[currentStep]; // Khôi phục trạng thái cây
            pathIndex = currentStep;

            if (currentState == DELETING || currentState == SHOWING_RESULT) {
                if (tree.search(operationValue)) {
                    tree.remove(tree.root, operationValue);
                    AVLTree treeReplica(tree);
                    treeUndoState.push(treeReplica);
                }
            }
        } else if (currentOperation == "search" && currentStep < searchPaths.size()) {
            currentPath = searchPaths[currentStep];
            highlightNodes = searchHighlightNodes[currentStep];
            notificationMessage = searchNotifications[currentStep];
            currentState = searchStates[currentStep];
            tree = searchTreeStates[currentStep]; // Khôi phục trạng thái cây
            pathIndex = currentStep;
        }

        stateTimer = 0.0f;
        resultTimer = 0.0f;

        fastForwardButtonClicked = false;
    }
}

void AVLTreeVisualizer::draw() {
    ClearBackground(WHITE);

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

    float pseudoCodeRightEdge = 10.0f + static_cast<float>(maxLineWidth) + 10.0f;
    float offset = pseudoCodeRightEdge + 300.0f;

    // Xóa khung màu xám (bỏ phần vẽ Rectangle)
    // Rectangle pseudoCodeBgRect = { 10.0f - 5.0f, static_cast<float>(pseudoCodeTopY) - 5.0f, static_cast<float>(maxLineWidth) + 10.0f, static_cast<float>(lineCount * lineHeight) + 10.0f };
    // DrawRectangleRec(pseudoCodeBgRect, Fade(LIGHTGRAY, 0.8f));

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

    DrawText("AVL Tree Visualizer", offset, 37.0f, 30, BLACK);

    if (tree.root) {
        drawTree(tree.root, offset, 120.0f, 200.0f, highlightNodes);
    }

    drawOperationMenu();
    drawAnimationMenu();

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

    // Điều chỉnh tọa độ nút "BackButton" lên trên (y từ 20 thành 10)
    Rectangle backButtonBounds = { 20.0f, 10.0f, 65.0f, 65.0f }; // Sửa y từ 20 thành 10
    DrawBackButton(backButtonTexture, backButtonBounds, backButtonClicked);
}

void AVLTreeVisualizer::animateRandom(int n) {
    tree.~AVLTree();
    tree = AVLTree();  // Sửa cú pháp tái khởi tạo
    for (int i = 0; i < n; i++) {
        int value = GetRandomValue(1, 100);
        tree.insert(tree.root, value);
    }
    AVLTree treeReplica(tree);
    treeUndoState.push(treeReplica);
    currentOperation = "random";
    currentPath.clear();
    pathIndex = 0;
    currentState = SHOWING_RESULT;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
    notificationMessage = "Initialized with " + std::to_string(n) + " nodes";
}

void AVLTreeVisualizer::animateInsert(int value) {
    currentOperation = "insert";
    operationValue = value;
    pendingInsertValue = value;

    // Xóa dữ liệu animation cũ
    insertPaths.clear();
    insertHighlightNodes.clear();
    insertNotifications.clear();
    insertStates.clear();
    insertTreeStates.clear();

    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    setNotificationMessage("Inserting " + std::to_string(value));
    currentState = TRAVERSING;

    // Lưu bước đầu tiên
    insertPaths.push_back(currentPath);
    insertHighlightNodes.push_back(highlightNodes);
    insertNotifications.push_back(notificationMessage);
    insertStates.push_back(currentState);
    insertTreeStates.push_back(AVLTree(tree)); // Lưu trạng thái cây

    totalSteps = 0;
    currentStep = 0;
}

void AVLTreeVisualizer::animateDelete(int value) {
    currentOperation = "delete";
    operationValue = value;

    // Xóa dữ liệu animation cũ
    deletePaths.clear();
    deleteHighlightNodes.clear();
    deleteNotifications.clear();
    deleteStates.clear();
    deleteTreeStates.clear();

    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    setNotificationMessage("Deleting " + std::to_string(value));
    currentState = TRAVERSING;
    highlightNodes.clear();
    if (!currentPath.empty()) highlightNodes.insert(currentPath[pathIndex]);

    // Lưu bước đầu tiên
    deletePaths.push_back(currentPath);
    deleteHighlightNodes.push_back(highlightNodes);
    deleteNotifications.push_back(notificationMessage);
    deleteStates.push_back(currentState);
    deleteTreeStates.push_back(AVLTree(tree)); // Lưu trạng thái cây

    totalSteps = 0;
    currentStep = 0;
}

void AVLTreeVisualizer::animateSearch(int value) {
    currentOperation = "search";
    operationValue = value;

    // Xóa dữ liệu animation cũ
    searchPaths.clear();
    searchHighlightNodes.clear();
    searchNotifications.clear();
    searchStates.clear();
    searchTreeStates.clear();

    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    searchFound = tree.search(value);
    highlightNodes.clear();
    setNotificationMessage("Searching " + std::to_string(value));
    currentState = TRAVERSING;

    // Lưu bước đầu tiên
    searchPaths.push_back(currentPath);
    searchHighlightNodes.push_back(highlightNodes);
    searchNotifications.push_back(notificationMessage);
    searchStates.push_back(currentState);
    searchTreeStates.push_back(AVLTree(tree)); // Lưu trạng thái cây

    totalSteps = 0;
    currentStep = 0;
}

void AVLTreeVisualizer::animateClear() {
    tree.~AVLTree();
    tree = AVLTree();
    currentOperation = "";
    currentPath.clear();
    pathIndex = 0;
    currentState = IDLE;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
    notificationMessage = "Tree cleared";
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
    tree = AVLTree();

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
        notificationMessage = "Reverted to previous state";
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
        notificationMessage = "Moved to next state";
    }
}

void AVLTreeVisualizer::stepBackward() {
    // Kiểm tra xem có bước nào để tua lại không
    if (totalSteps == 0 || currentStep <= 0) {
        return;
    }

    paused = true;
    playPauseButtonMessage = "Play";

    currentStep--;

    // Kiểm tra và truy cập vector an toàn
    if (currentOperation == "insert" && currentStep < insertPaths.size()) {
        currentPath = insertPaths[currentStep];
        highlightNodes = insertHighlightNodes[currentStep];
        notificationMessage = insertNotifications[currentStep];
        currentState = insertStates[currentStep];
        tree = insertTreeStates[currentStep]; // Khôi phục trạng thái cây
        pathIndex = currentStep;
    } else if (currentOperation == "delete" && currentStep < deletePaths.size()) {
        currentPath = deletePaths[currentStep];
        highlightNodes = deleteHighlightNodes[currentStep];
        notificationMessage = deleteNotifications[currentStep];
        currentState = deleteStates[currentStep];
        tree = deleteTreeStates[currentStep]; // Khôi phục trạng thái cây
        pathIndex = currentStep;
    } else if (currentOperation == "search" && currentStep < searchPaths.size()) {
        currentPath = searchPaths[currentStep];
        highlightNodes = searchHighlightNodes[currentStep];
        notificationMessage = searchNotifications[currentStep];
        currentState = searchStates[currentStep];
        tree = searchTreeStates[currentStep]; // Khôi phục trạng thái cây
        pathIndex = currentStep;
    } else {
        // Nếu không có dữ liệu hợp lệ, đặt về trạng thái mặc định
        currentStep = 0;
        return;
    }

    stateTimer = 0.0f;
    resultTimer = 0.0f;
}

void AVLTreeVisualizer::stepForward() {
    // Kiểm tra xem có bước nào để tua tiến không
    if (totalSteps == 0 || currentStep >= totalSteps - 1) {
        return;
    }

    paused = true;
    playPauseButtonMessage = "Play";

    currentStep++;

    // Kiểm tra và truy cập vector an toàn
    if (currentOperation == "insert" && currentStep < insertPaths.size()) {
        currentPath = insertPaths[currentStep];
        highlightNodes = insertHighlightNodes[currentStep];
        notificationMessage = insertNotifications[currentStep];
        currentState = insertStates[currentStep];
        tree = insertTreeStates[currentStep]; // Khôi phục trạng thái cây
        pathIndex = currentStep;

        // Nếu bước này là INSERTING hoặc SHOWING_RESULT, áp dụng lại Insert
        if (currentState == INSERTING || currentState == SHOWING_RESULT) {
            if (!tree.search(pendingInsertValue)) {
                tree.insert(tree.root, pendingInsertValue);
                AVLTree treeReplica(tree);
                treeUndoState.push(treeReplica);
            }
        }
    } else if (currentOperation == "delete" && currentStep < deletePaths.size()) {
        currentPath = deletePaths[currentStep];
        highlightNodes = deleteHighlightNodes[currentStep];
        notificationMessage = deleteNotifications[currentStep];
        currentState = deleteStates[currentStep];
        tree = deleteTreeStates[currentStep]; // Khôi phục trạng thái cây
        pathIndex = currentStep;

        // Nếu bước này là DELETING hoặc SHOWING_RESULT, áp dụng lại Delete
        if (currentState == DELETING || currentState == SHOWING_RESULT) {
            if (tree.search(operationValue)) {
                tree.remove(tree.root, operationValue);
                AVLTree treeReplica(tree);
                treeUndoState.push(treeReplica);
            }
        }
    } else if (currentOperation == "search" && currentStep < searchPaths.size()) {
        currentPath = searchPaths[currentStep];
        highlightNodes = searchHighlightNodes[currentStep];
        notificationMessage = searchNotifications[currentStep];
        currentState = searchStates[currentStep];
        tree = searchTreeStates[currentStep]; // Khôi phục trạng thái cây
        pathIndex = currentStep;
    } else {
        // Nếu không có dữ liệu hợp lệ, đặt về bước cuối
        currentStep = totalSteps - 1;
        return;
    }

    stateTimer = 0.0f;
    resultTimer = 0.0f;
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

                // Lưu bước hiện tại
                if (currentOperation == "insert") {
                    insertPaths.push_back(currentPath);
                    insertHighlightNodes.push_back(highlightNodes);
                    insertNotifications.push_back(notificationMessage);
                    insertStates.push_back(currentState);
                    insertTreeStates.push_back(AVLTree(tree));
                } else if (currentOperation == "delete") {
                    deletePaths.push_back(currentPath);
                    deleteHighlightNodes.push_back(highlightNodes);
                    deleteNotifications.push_back(notificationMessage);
                    deleteStates.push_back(currentState);
                    deleteTreeStates.push_back(AVLTree(tree));
                } else if (currentOperation == "search") {
                    searchPaths.push_back(currentPath);
                    searchHighlightNodes.push_back(highlightNodes);
                    searchNotifications.push_back(notificationMessage);
                    searchStates.push_back(currentState);
                    searchTreeStates.push_back(AVLTree(tree));
                }
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

                // Lưu bước hiện tại
                if (currentOperation == "insert") {
                    insertPaths.push_back(currentPath);
                    insertHighlightNodes.push_back(highlightNodes);
                    insertNotifications.push_back(notificationMessage);
                    insertStates.push_back(currentState);
                    insertTreeStates.push_back(AVLTree(tree));
                } else if (currentOperation == "delete") {
                    deletePaths.push_back(currentPath);
                    deleteHighlightNodes.push_back(highlightNodes);
                    deleteNotifications.push_back(notificationMessage);
                    deleteStates.push_back(currentState);
                    deleteTreeStates.push_back(AVLTree(tree));
                } else if (currentOperation == "search") {
                    searchPaths.push_back(currentPath);
                    searchHighlightNodes.push_back(highlightNodes);
                    searchNotifications.push_back(notificationMessage);
                    searchStates.push_back(currentState);
                    searchTreeStates.push_back(AVLTree(tree));
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

                // Lưu bước hiện tại
                insertPaths.push_back(currentPath);
                insertHighlightNodes.push_back(highlightNodes);
                insertNotifications.push_back(notificationMessage);
                insertStates.push_back(currentState);
                insertTreeStates.push_back(AVLTree(tree));
            }
            break;

        case HIGHLIGHTING_BEFORE_DELETE:
            if (stateTimer >= 0.5f) {
                notificationMessage = "Deleting " + std::to_string(operationValue);
                currentState = DELETING;
                stateTimer = 0.0f;

                // Lưu bước hiện tại
                deletePaths.push_back(currentPath);
                deleteHighlightNodes.push_back(highlightNodes);
                deleteNotifications.push_back(notificationMessage);
                deleteStates.push_back(currentState);
                deleteTreeStates.push_back(AVLTree(tree));
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

                // Lưu bước hiện tại
                deletePaths.push_back(currentPath);
                deleteHighlightNodes.push_back(highlightNodes);
                deleteNotifications.push_back(notificationMessage);
                deleteStates.push_back(currentState);
                deleteTreeStates.push_back(AVLTree(tree));
            }
            break;

        case ROTATING:
            notificationMessage = "Executing rotation";
            currentState = SHOWING_RESULT;
            resultTimer = 0.0f;

            // Lưu bước hiện tại
            if (currentOperation == "insert") {
                insertPaths.push_back(currentPath);
                insertHighlightNodes.push_back(highlightNodes);
                insertNotifications.push_back(notificationMessage);
                insertStates.push_back(currentState);
                insertTreeStates.push_back(AVLTree(tree));
            } else if (currentOperation == "delete") {
                deletePaths.push_back(currentPath);
                deleteHighlightNodes.push_back(highlightNodes);
                deleteNotifications.push_back(notificationMessage);
                deleteStates.push_back(currentState);
                deleteTreeStates.push_back(AVLTree(tree));
            }
            break;

        case SHOWING_RESULT:
            resultTimer += deltaTime;
            duration = (currentOperation == "search" || currentOperation == "delete") && !searchFound ? 4.0f : 2.0f;
            if (resultTimer >= duration) {
                pathIndex = 0;
                rotationIndex = 0;
                searchFound = false;
                highlightNodes.clear();

                // Cập nhật totalSteps chính xác
                if (currentOperation == "insert" && !insertPaths.empty()) {
                    totalSteps = insertPaths.size();
                } else if (currentOperation == "delete" && !deletePaths.empty()) {
                    totalSteps = deletePaths.size();
                } else if (currentOperation == "search" && !searchPaths.empty()) {
                    totalSteps = searchPaths.size();
                } else {
                    totalSteps = 0; // Đảm bảo totalSteps không âm hoặc không xác định
                }
                currentStep = totalSteps > 0 ? totalSteps - 1 : 0;
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

                    // Lưu bước hiện tại
                    searchPaths.push_back(currentPath);
                    searchHighlightNodes.push_back(highlightNodes);
                    searchNotifications.push_back(notificationMessage);
                    searchStates.push_back(currentState);
                    searchTreeStates.push_back(AVLTree(tree));
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

                    // Lưu bước hiện tại
                    searchPaths.push_back(currentPath);
                    searchHighlightNodes.push_back(highlightNodes);
                    searchNotifications.push_back(notificationMessage);
                    searchStates.push_back(currentState);
                    searchTreeStates.push_back(AVLTree(tree));
                }
            }
            break;

        case SEARCH_NOT_FOUND:
            if (stateTimer >= 1.0f) {
                currentState = SHOWING_RESULT;
                highlightNodes.clear();
                stateTimer = 0.0f;
                resultTimer = 0.0f;

                // Lưu bước hiện tại
                searchPaths.push_back(currentPath);
                searchHighlightNodes.push_back(highlightNodes);
                searchNotifications.push_back(notificationMessage);
                searchStates.push_back(currentState);
                searchTreeStates.push_back(AVLTree(tree));
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
        t = std::max(0.0f, std::min(t, 1.0f));
        return minValue + t * (maxValue - minValue);
    }
    return currentValue;
}