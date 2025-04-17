#include "../inc/AVLTreeMain.h"
#include "../inc/AVLTree.h"
#include "../inc/Button.h"
#include "tinyfiledialogs.h"
#include <sstream>
#include <cstdlib>
#include <stack>
#include <iostream>
#include <cmath>
#include "raylib.h"
#include <fstream>

std::stack<AVLTree> treeUndoState;
std::stack<AVLTree> treeRedoState;

#define NODE_RADIUS 20.0f

AVLTreeVisualizer::AVLTreeVisualizer() 
    : inputText(""),
      inputActive(false),
      handleSpace{0.0f, 0.0f, 1500.0f, 75.0f},
      inputBox{10.0f, 610.0f, 100.0f, 25.0f},
      initButtonRect{30.0f, 700.0f, 80.0f, 30.0f},
      insertButtonRect{130.0f, 700.0f, 80.0f, 30.0f},
      deleteButtonRect{230.0f, 700.0f, 80.0f, 30.0f},
      searchButtonRect{330.0f, 700.0f, 80.0f, 30.0f},
      loadFileButtonRect{430.0f, 700.0f, 80.0f, 30.0f},
      rewindButtonRect{600.0f, 700.0f, 80.0f, 30.0f},
      previousButtonRect{700.0f, 700.0f, 80.0f, 30.0f},
      playPauseButtonRect{800.0f, 700.0f, 80.0f, 30.0f},
      nextButtonRect{900.0f, 700.0f, 80.0f, 30.0f},
      fastForwardButtonRect{1000.0f, 700.0f, 80.0f, 30.0f},
      randomButtonRect{120.0f, 610.0f, 25.0f, 25.0f},
      speedBar{600.0f + 35, 670.0f, 280.0f, 20.0f},
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
      loadFileButtonMessage("Upload"),
      rewindButtonMessage("|<<"),
      previousButtonMessage("Prev"),
      playPauseButtonMessage("Pause"),
      nextButtonMessage("Next"),
      fastForwardButtonMessage(">>|"),
      randomButtonMessage("?"),
      manualStepping(false),
      stateHistoryIndex(0),
      shouldClose(false),
      backButtonClicked(false),
      showInputWindow(false),
      okButtonRect{0.0f, 0.0f, 50.0f, 30.0f},
      okButtonClicked(false),
      okButtonMessage("OK"),
      operationType(""),
      currentStepIndex(-1),
      isInitialized(false) // Initialize isInitialized to false
{
    sliderHandle = { speedBar.x + (speedBar.width - sliderHandle.width) / 2.0f, speedBar.y + 2.0f, 10.0f, 16.0f };
    backButtonTexture = LoadTexture("..\\resources\\images\\BackButton.png");
    
    initialTreeState = AVLTree();
    operationSteps.clear();
    AVLTree treeReplica(tree);
    treeUndoState.push(treeReplica);
    OperationStep initialStep = {
        "", IDLE, {}, 0, {}, "Initial null tree", 0, 0, false, 0.0f, 0.0f, "", 0, treeReplica
    };
    operationSteps.push_back(initialStep);
    currentStepIndex = 0;
}

AVLTreeVisualizer::~AVLTreeVisualizer() {
    UnloadTexture(backButtonTexture);
}

void AVLTreeVisualizer::handleInput() {
    Vector2 mousePos = GetMousePosition();

    // Handle Back button
    Rectangle backButtonBounds = { 20.0f, 20.0f, 65.0f, 65.0f };
    if (DrawBackButton(backButtonTexture, backButtonBounds, backButtonClicked)) {
        shouldClose = true;
        backButtonClicked = false;
        return;
    }

    if (showInputWindow) {
        Font defaultFont = GetFontDefault();
        const char* buttonMessage = nullptr;

        if (DrawButton("Init", initButtonRect, defaultFont, initButtonClicked, buttonMessage)) {
            showInputWindow = false;
            operationType = "";
            inputText.clear();
            notificationMessage = "Initializing: Clearing list";
            showInputWindow = true;
            operationType = "init";
        }
        if (isInitialized) {
            if (DrawButton("Insert", insertButtonRect, defaultFont, insertButtonClicked, buttonMessage)) {
                showInputWindow = false;
                operationType = "";
                inputText.clear();
                notificationMessage = "Inserting";
                showInputWindow = true;
                operationType = "insert";
            }
            if (DrawButton("Delete", deleteButtonRect, defaultFont, deleteButtonClicked, buttonMessage)) {
                showInputWindow = false;
                operationType = "";
                inputText.clear();
                notificationMessage = "Deleting";
                showInputWindow = true;
                operationType = "delete";
            }
            if (DrawButton("Search", searchButtonRect, defaultFont, searchButtonClicked, buttonMessage)) {
                showInputWindow = false;
                operationType = "";
                inputText.clear();
                notificationMessage = "Searching";
                showInputWindow = true;
                operationType = "search";
            }
        }

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

        if (DrawButton("?", randomButtonRect, defaultFont, randomButtonClicked, buttonMessage)) {
            int randomValue = (operationType == "init") ? GetRandomValue(1, 20) : GetRandomValue(1, 100);
            inputText = std::to_string(randomValue);
            inputActive = false;
        }

        if (operationType == "init") {
            loadFileButtonRect = { 30.0f, 647.0f, 80.0f, 30.0f };
            if (DrawButton("Upload", loadFileButtonRect, defaultFont, loadFileButtonClicked, buttonMessage)) {
                animateLoadFile();
                inputText.clear();
                showInputWindow = false;
                operationType = "";
                isInitialized = true;
            }
        }

        if (DrawButton("OK", okButtonRect, defaultFont, okButtonClicked, buttonMessage) && !inputText.empty()) {
            if (operationType == "init") {
                int numNodes = std::stoi(inputText);
                if (numNodes > 0) {
                    tree.~AVLTree();
                    new (&tree) AVLTree();
                    int* randomValues = LoadRandomSequence(numNodes, 1, 100);
                    for (int i = 0; i < numNodes; i++) {
                        tree.insert(tree.root, randomValues[i]);
                    }
                    initialTreeState = tree;
                    AVLTree treeReplica(tree);
                    treeUndoState.push(treeReplica);
                    UnloadRandomSequence(randomValues);
                    currentOperation = "random";
                    OperationStep step = {
                        "random", SHOWING_RESULT, {}, 0, {},
                        "Initialization complete", 0, 0, false, 0.0f, 0.0f,
                        currentOperation, rotationIndex, treeReplica
                    };
                    operationSteps.push_back(step);
                    currentStepIndex = operationSteps.size() - 1;
                    currentState = SHOWING_RESULT;
                    stateTimer = 0.0f;
                    resultTimer = 0.0f;
                    notificationMessage = "Initialization complete";
                    isInitialized = true;
                }
            } else if (operationType == "insert") {
                int insertValue = std::stoi(inputText);
                animateInsert(insertValue);
            } else if (operationType == "delete") {
                int deleteValue = std::stoi(inputText);
                animateDelete(deleteValue);
            } else if (operationType == "search") {
                int searchValue = std::stoi(inputText);
                animateSearch(searchValue);
            }
            showInputWindow = false;
            operationType = "";
            inputText.clear();
        }
        return;
    }

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
    const char* buttonMessage = nullptr;

    if (DrawButton("Init", initButtonRect, defaultFont, initButtonClicked, buttonMessage)) {
        showInputWindow = true;
        operationType = "init";
        inputText.clear();
        notificationMessage = "Initializing: Clearing list";
    }
    if (isInitialized) {
        if (DrawButton("Insert", insertButtonRect, defaultFont, insertButtonClicked, buttonMessage)) {
            showInputWindow = true;
            operationType = "insert";
            inputText.clear();
            notificationMessage = "Inserting";
        }
        if (DrawButton("Delete", deleteButtonRect, defaultFont, deleteButtonClicked, buttonMessage)) {
            showInputWindow = true;
            operationType = "delete";
            inputText.clear();
            notificationMessage = "Deleting";
        }
        if (DrawButton("Search", searchButtonRect, defaultFont, searchButtonClicked, buttonMessage)) {
            showInputWindow = true;
            operationType = "search";
            inputText.clear();
            notificationMessage = "Searching";
        }
    }

    // Animation control buttons
    if (DrawButton("|<<", rewindButtonRect, defaultFont, rewindButtonClicked, buttonMessage)) {
        if (!treeUndoState.empty()) {
            paused = true;
            playPauseButtonMessage = "Play";
            AVLTree treeReplica(tree);
            treeRedoState.push(treeReplica);
            tree = treeUndoState.top();
            treeUndoState.pop();
            notificationMessage = "Reverted to previous tree state";
            isInitialized = tree.root != nullptr;
            currentOperation = "";
            currentPath.clear();
            pathIndex = 0;
            highlightNodes.clear();
            currentState = IDLE;
            stateTimer = 0.0f;
            resultTimer = 0.0f;
            if (!operationSteps.empty()) {
                currentStepIndex = std::max(0, currentStepIndex - 1);
                while (currentStepIndex > 0 && operationSteps[currentStepIndex].currentState != SHOWING_RESULT && operationSteps[currentStepIndex].currentState != IDLE) {
                    currentStepIndex--;
                }
                tree = operationSteps[currentStepIndex].treeSnapshot;
            }
        } else {
            notificationMessage = "No previous tree state available";
        }
        inputText.clear();
        rewindButtonClicked = false;
    }

    // Step Back (Previous)
    if (paused && !operationSteps.empty() && currentStepIndex > 0 && DrawButton("Prev", previousButtonRect, defaultFont, previousButtonClicked, buttonMessage)) {
        stepBackward();
        previousButtonClicked = false;
    } else if (paused && (operationSteps.empty() || currentStepIndex <= 0)) {
        notificationMessage = "Cannot step back further";
        previousButtonClicked = false;
    }

    if (DrawButton(paused ? "Play" : "Pause", playPauseButtonRect, defaultFont, playPauseButtonClicked, buttonMessage)) {
        paused = !paused;
        playPauseButtonMessage = paused ? "Play" : "Pause";
        if (!paused) stateTimer = 0.0f;
        playPauseButtonClicked = false;
    }

    if (paused && currentStepIndex < static_cast<int>(operationSteps.size()) - 1 && DrawButton("Next", nextButtonRect, defaultFont, nextButtonClicked, buttonMessage)) {
        stepForward();
        nextButtonClicked = false;
    }

    if (DrawButton(">>|", fastForwardButtonRect, defaultFont, fastForwardButtonClicked, buttonMessage)) {
        if (currentState != IDLE || !treeRedoState.empty()) {
            paused = true;
            playPauseButtonMessage = "Play";
            if (currentState != IDLE && !currentPath.empty()) {
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
                    OperationStep step = {
                        "insert", SHOWING_RESULT, currentPath, pathIndex, highlightNodes,
                        notificationMessage, operationValue, pendingInsertValue, searchFound, 0.0f, 0.0f,
                        currentOperation, rotationIndex, tree
                    };
                    operationSteps.push_back(step);
                    currentStepIndex = operationSteps.size() - 1;
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
                    OperationStep step = {
                        "delete", SHOWING_RESULT, currentPath, pathIndex, highlightNodes,
                        notificationMessage, operationValue, pendingInsertValue, searchFound, 0.0f, 0.0f,
                        currentOperation, rotationIndex, tree
                    };
                    operationSteps.push_back(step);
                    currentStepIndex = operationSteps.size() - 1;
                    currentState = SHOWING_RESULT;
                } else if (currentOperation == "search") {
                    if (searchFound) {
                        notificationMessage = "Found " + std::to_string(operationValue);
                    } else {
                        notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";
                    }
                    OperationStep step = {
                        "search", SHOWING_RESULT, currentPath, pathIndex, highlightNodes,
                        notificationMessage, operationValue, pendingInsertValue, searchFound, 0.0f, 0.0f,
                        currentOperation, rotationIndex, tree
                    };
                    operationSteps.push_back(step);
                    currentStepIndex = operationSteps.size() - 1;
                    currentState = SHOWING_RESULT;
                }
                stateTimer = 0.0f;
                resultTimer = 0.0f;
            } else if (!treeRedoState.empty()) {
                animateNext();
            }
        } else {
            notificationMessage = "No animation to skip or redo";
        }
        fastForwardButtonClicked = false;
    }

    if (CheckCollisionPointRec(GetMousePosition(), speedBar) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        animationSpeed = UpdateSlider(speedBar, 0.05f, 5.0f, animationSpeed);
    }
}

void AVLTreeVisualizer::stepBackward() {
    if (!paused || operationSteps.empty() || currentStepIndex <= 0) {
        notificationMessage = "Cannot step back further";
        return;
    }

    // Store current tree state for redo
    AVLTree treeReplica(tree);
    treeRedoState.push(treeReplica);

    // Decrement step index
    currentStepIndex--;
    const OperationStep& step = operationSteps[currentStepIndex];
    currentOperation = step.operation;
    currentState = step.currentState;
    currentPath = step.currentPath;
    pathIndex = step.pathIndex;
    highlightNodes = step.highlightNodes;
    notificationMessage = step.notificationMessage;
    operationValue = step.operationValue;
    pendingInsertValue = step.pendingInsertValue;
    searchFound = step.searchFound;
    stateTimer = step.stateTimer;
    resultTimer = step.resultTimer;
    rotationIndex = step.rotationIndex;

    // Restore tree state from snapshot
    tree = step.treeSnapshot;
    isInitialized = tree.root != nullptr;

    // Update notification based on context
    if (currentState == TRAVERSING || currentState == SEARCHING) {
        if (pathIndex < currentPath.size()) {
            notificationMessage = "Stepped back: Traversing " + std::to_string(currentPath[pathIndex]->data);
        } else {
            notificationMessage = "Stepped back: Traversing complete";
        }
    } else if (currentState == SHOWING_RESULT || currentState == IDLE) {
        notificationMessage = "Stepped back to: " + step.notificationMessage;
    } else {
        notificationMessage = "Stepped back: " + step.notificationMessage;
    }

    inputText.clear();
}
void AVLTreeVisualizer::stepForward() {
    if (!paused || currentState == SHOWING_RESULT || currentState == IDLE) {
        return;
    }

    if (currentStepIndex < static_cast<int>(operationSteps.size()) - 1) {
        currentStepIndex++;
        const OperationStep& step = operationSteps[currentStepIndex];

        currentOperation = step.operation;
        currentState = step.currentState;
        currentPath = step.currentPath;
        pathIndex = step.pathIndex;
        highlightNodes = step.highlightNodes;
        notificationMessage = step.notificationMessage;
        operationValue = step.operationValue;
        pendingInsertValue = step.pendingInsertValue;
        searchFound = step.searchFound;
        stateTimer = step.stateTimer;
        resultTimer = step.resultTimer;
    }
}

void AVLTreeVisualizer::updateAnimation(float deltaTime) {
    if (currentState == IDLE || paused) {
        return;
    }

    stateTimer += deltaTime * (animationSpeed * 5.0f);

    float duration = 0.0f;

    switch (currentState) {
        case TRAVERSING: {
            if (stateTimer >= 0.5f && pathIndex < currentPath.size()) {
                Node* currentNode = currentPath[pathIndex];
                highlightNodes.insert(currentNode);
                notificationMessage = "Traverse: " + std::to_string(currentNode->data);
                pathIndex++;
                stateTimer = 0.0f;

                AVLTree treeSnapshot(tree);
                OperationStep step = {
                    currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                    notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                    currentOperation, rotationIndex, treeSnapshot
                };
                operationSteps.push_back(step);
                currentStepIndex = operationSteps.size() - 1;
            }
            if (pathIndex >= currentPath.size()) {
                if (currentOperation == "insert") {
                    currentState = INSERTING;
                    notificationMessage = "Inserting " + std::to_string(pendingInsertValue);

                    AVLTree treeSnapshot(tree);
                    OperationStep step = {
                        currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                        notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                        currentOperation, rotationIndex, treeSnapshot
                    };
                    operationSteps.push_back(step);
                    currentStepIndex = operationSteps.size() - 1;
                } else if (currentOperation == "delete") {
                    if (!tree.search(operationValue)) {
                        notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";
                        stateTimer = 0.0f;
                        currentState = SHOWING_RESULT;

                        AVLTree treeSnapshot(tree);
                        OperationStep step = {
                            currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                            notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                            currentOperation, rotationIndex, treeSnapshot
                        };
                        operationSteps.push_back(step);
                        currentStepIndex = operationSteps.size() - 1;
                    } else {
                        currentState = HIGHLIGHTING_BEFORE_DELETE;
                        notificationMessage = "Deleting " + std::to_string(operationValue);

                        AVLTree treeSnapshot(tree);
                        OperationStep step = {
                            currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                            notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                            currentOperation, rotationIndex, treeSnapshot
                        };
                        operationSteps.push_back(step);
                        currentStepIndex = operationSteps.size() - 1;
                    }
                } else if (currentOperation == "search") {
                    currentState = SEARCHING;
                    stateTimer = 0.0f;

                    AVLTree treeSnapshot(tree);
                    OperationStep step = {
                        currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                        notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                        currentOperation, rotationIndex, treeSnapshot
                    };
                    operationSteps.push_back(step);
                    currentStepIndex = operationSteps.size() - 1;
                }
            }
            break;
        }

        case INSERTING: {
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

                AVLTree treeSnapshot(tree);
                OperationStep step = {
                    currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                    notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                    currentOperation, rotationIndex, treeSnapshot
                };
                operationSteps.push_back(step);
                currentStepIndex = operationSteps.size() - 1;
            }
            break;
        }

        case HIGHLIGHTING_BEFORE_DELETE: {
            if (stateTimer >= 0.5f) {
                notificationMessage = "Deleting " + std::to_string(operationValue);
                currentState = DELETING;
                stateTimer = 0.0f;

                AVLTree treeSnapshot(tree);
                OperationStep step = {
                    currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                    notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                    currentOperation, rotationIndex, treeSnapshot
                };
                operationSteps.push_back(step);
                currentStepIndex = operationSteps.size() - 1;
            }
            break;
        }

        case DELETING: {
            if (stateTimer >= 0.5f) {
                tree.remove(tree.root, operationValue);
                AVLTree treeReplica(tree);
                treeUndoState.push(treeReplica);
                notificationMessage = "Node " + std::to_string(operationValue) + " Deleted";
                currentState = SHOWING_RESULT;
                resultTimer = 0.0f;
                stateTimer = 0.0f;

                AVLTree treeSnapshot(tree);
                OperationStep step = {
                    currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                    notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                    currentOperation, rotationIndex, treeSnapshot
                };
                operationSteps.push_back(step);
                currentStepIndex = operationSteps.size() - 1;
            }
            break;
        }

        case ROTATING: {
            notificationMessage = "Executing rotation";
            currentState = SHOWING_RESULT;
            resultTimer = 0.0f;

            AVLTree treeSnapshot(tree);
            OperationStep step = {
                currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                currentOperation, rotationIndex, treeSnapshot
            };
            operationSteps.push_back(step);
            currentStepIndex = operationSteps.size() - 1;
            break;
        }

        case SHOWING_RESULT: {
            resultTimer += deltaTime;
            duration = (currentOperation == "search" || currentOperation == "delete") && !searchFound ? 4.0f : 2.0f;
            if (resultTimer >= duration) {
                currentState = IDLE;
                currentPath.clear();
                pathIndex = 0;
                rotationIndex = 0;
                searchFound = false;
                highlightNodes.clear();

                AVLTree treeSnapshot(tree);
                OperationStep step = {
                    currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                    notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                    currentOperation, rotationIndex, treeSnapshot
                };
                operationSteps.push_back(step);
                currentStepIndex = operationSteps.size() - 1;
            }
            break;
        }

        case SEARCHING: {
            if (stateTimer >= 0.5f) {
                if (pathIndex < currentPath.size()) {
                    Node* currentNode = currentPath[pathIndex];
                    highlightNodes.insert(currentNode);
                    notificationMessage = "Searching: " + std::to_string(currentNode->data);
                    pathIndex++;
                    stateTimer = 0.0f;

                    AVLTree treeSnapshot(tree);
                    OperationStep step = {
                        currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                        notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                        currentOperation, rotationIndex, treeSnapshot
                    };
                    operationSteps.push_back(step);
                    currentStepIndex = operationSteps.size() - 1;
                } else {
                    if (searchFound) {
                        notificationMessage = "Found " + std::to_string(operationValue);
                        currentState = SHOWING_RESULT;
                        stateTimer = 0.0f;
                        resultTimer = 0.0f;

                        AVLTree treeSnapshot(tree);
                        OperationStep step = {
                            currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                            notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                            currentOperation, rotationIndex, treeSnapshot
                        };
                        operationSteps.push_back(step);
                        currentStepIndex = operationSteps.size() - 1;
                    } else {
                        notificationMessage = "Node " + std::to_string(operationValue) + " Not Found";
                        currentState = SEARCH_NOT_FOUND;
                        stateTimer = 0.0f;

                        AVLTree treeSnapshot(tree);
                        OperationStep step = {
                            currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                            notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                            currentOperation, rotationIndex, treeSnapshot
                        };
                        operationSteps.push_back(step);
                        currentStepIndex = operationSteps.size() - 1;
                    }
                }
            }
            break;
        }

        case SEARCH_NOT_FOUND: {
            if (stateTimer >= 1.0f) {
                currentState = SHOWING_RESULT;
                highlightNodes.clear();
                stateTimer = 0.0f;
                resultTimer = 0.0f;

                AVLTree treeSnapshot(tree);
                OperationStep step = {
                    currentOperation, currentState, currentPath, pathIndex, highlightNodes,
                    notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
                    currentOperation, rotationIndex, treeSnapshot
                };
                operationSteps.push_back(step);
                currentStepIndex = operationSteps.size() - 1;
            }
            break;
        }
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
    int pseudoCodeTopY = 100;
    int maxLineWidth = 0;
    for (const auto& l : lines) {
        int lineWidth = MeasureText(l.c_str(), 20);
        if (lineWidth > maxLineWidth) maxLineWidth = lineWidth;
    }
    float pseudoCodeRightEdge = 10.0f + static_cast<float>(maxLineWidth) + 10.0f;
    float offset = pseudoCodeRightEdge + 400.0f;

    int titleSize = MeasureText("AVL Tree Visualizer", 30);
    float titleX = offset;
    float titleY = 37.0f;
    DrawText("AVL Tree Visualizer", titleX, titleY, 30, BLACK);
    int speedLabelSize = MeasureText("Speed", 20);
    Rectangle speedLabelBgRect = { speedBar.x - 35.0f, speedBar.y - 20.0f - 5.0f, static_cast<float>(speedLabelSize) + 10.0f, 30.0f };
    DrawRectangleRec(speedLabelBgRect, Fade(LIGHTGRAY, 0.8f));
    DrawText("Speed", speedBar.x - 30.0f, speedBar.y - 20.0f, 20, BLACK);
    char* speedText = (char*)malloc(20);
    sprintf(speedText, "x%.2f", animationSpeed);
    DrawText(speedText, speedBar.x + speedBar.width + 10.0f, speedBar.y, 20, BLACK);
    free(speedText);

    DrawRectangleRec(speedBar, LIGHTGRAY);
    float middleX = speedBar.x + speedBar.width / 2.0f;
    DrawLineEx({middleX, speedBar.y - 5.0f}, {middleX, speedBar.y + speedBar.height + 5.0f}, 2.0f, BLACK);
    DrawRectangleRec(sliderHandle, DARKGRAY);

    if (showInputWindow) {
        inputBox = { 30.0f, 620.0f, 50.0f, 25.0f };
        randomButtonRect = { 80.0f, 620.0f, 25.0f, 25.0f };
        okButtonRect = { 350.0f, 655.0f, 40.0f, 30.0f };
        loadFileButtonRect = { 30.0f, 647.0f, 80.0f, 30.0f };

        std::string label = operationType == "init" ? "N: " : "V: ";
        DrawText(label.c_str(), 10.0f, 625.0f, 20, BLACK);
        DrawRectangleRec(inputBox, LIGHTGRAY);
        DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);
        DrawText(inputText.c_str(), inputBox.x + 5.0f, inputBox.y + 5.0f, 20, BLACK);

        Font defaultFont = GetFontDefault();
        const char* buttonMessage = nullptr;
        DrawButton("?", randomButtonRect, defaultFont, randomButtonClicked, buttonMessage);
        DrawButton("OK", okButtonRect, defaultFont, okButtonClicked, buttonMessage);

        if (operationType == "init") {
            DrawButton("Upload", loadFileButtonRect, defaultFont, loadFileButtonClicked, buttonMessage);
        }
    }

    Font defaultFont = GetFontDefault();
    const char* buttonMessage = nullptr;
    DrawButton("Init", initButtonRect, defaultFont, initButtonClicked, buttonMessage);
    if (isInitialized) {
        DrawButton("Insert", insertButtonRect, defaultFont, insertButtonClicked, buttonMessage);
        DrawButton("Delete", deleteButtonRect, defaultFont, deleteButtonClicked, buttonMessage);
        DrawButton("Search", searchButtonRect, defaultFont, searchButtonClicked, buttonMessage);
    }
    DrawButton("|<<", rewindButtonRect, defaultFont, rewindButtonClicked, buttonMessage);
    DrawButton("Prev", previousButtonRect, defaultFont, previousButtonClicked, buttonMessage);
    DrawButton(paused ? "Play" : "Pause", playPauseButtonRect, defaultFont, playPauseButtonClicked, buttonMessage);
    DrawButton("Next", nextButtonRect, defaultFont, nextButtonClicked, buttonMessage);
    DrawButton(">>|", fastForwardButtonRect, defaultFont, fastForwardButtonClicked, buttonMessage);

    Rectangle backButtonBounds = { 20.0f, 20.0f, 65.0f, 65.0f };
    DrawBackButton(backButtonTexture, backButtonBounds, backButtonClicked);

    if (tree.root) {
        drawTree(tree.root, offset, 120.0f, 200.0f, highlightNodes);
    }
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
        DrawText(displayMessage.c_str(), xPos, yPos, 20, operationType == "init" ? RED : BLACK);
    }
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
    if (IsKeyPressed(KEY_ESCAPE)) {
        shouldClose = true;
    }
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
        OperationStep step = {
            "loadfile", SHOWING_RESULT, {}, 0, {}, notificationMessage, 0, 0, false, 0.0f, 0.0f
        };
        operationSteps.push_back(step);
        currentStepIndex = operationSteps.size() - 1;
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        return;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        notificationMessage = "Failed to open file: " + std::string(filePath);
        currentOperation = "loadfile";
        OperationStep step = {
            "loadfile", SHOWING_RESULT, {}, 0, {}, notificationMessage, 0, 0, false, 0.0f, 0.0f
        };
        operationSteps.push_back(step);
        currentStepIndex = operationSteps.size() - 1;
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
        initialTreeState = tree;
        AVLTree treeReplica(tree);
        treeUndoState.push(treeReplica);
        notificationMessage = "Loaded " + std::to_string(numberCount) + " numbers from " + std::string(filePath);
    } else if (!hasContent) {
        initialTreeState = tree;
        notificationMessage = "File is empty: " + std::string(filePath);
    } else {
        initialTreeState = tree;
        notificationMessage = "No valid numbers found in " + std::string(filePath);
    }

    currentOperation = "loadfile";
    currentPath.clear();
    pathIndex = 0;
    OperationStep step = {
        "loadfile", SHOWING_RESULT, {}, 0, {}, notificationMessage, 0, 0, false, 0.0f, 0.0f
    };
    operationSteps.push_back(step);
    currentStepIndex = operationSteps.size() - 1;
    currentState = SHOWING_RESULT;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
}
void AVLTreeVisualizer::animatePrevious() {
    if (!treeUndoState.empty()) {
        // If in the middle of an animation, undo the current command
        if (currentState != IDLE && !paused) {
            while (!treeUndoState.empty() && currentStepIndex > 0) {
                AVLTree treeReplica(tree);
                treeRedoState.push(treeReplica);
                tree = treeUndoState.top();
                treeUndoState.pop();

                // Go back to the last SHOWING_RESULT or IDLE state
                currentStepIndex--;
                while (currentStepIndex > 0 && operationSteps[currentStepIndex].currentState != SHOWING_RESULT && operationSteps[currentStepIndex].currentState != IDLE) {
                    currentStepIndex--;
                }
                if (operationSteps[currentStepIndex].currentState == SHOWING_RESULT || operationSteps[currentStepIndex].currentState == IDLE) {
                    break;
                }
            }
        } else {
            // Not in the middle of an animation, undo the previous command
            AVLTree treeReplica(tree);
            treeRedoState.push(treeReplica);
            tree = treeUndoState.top();
            treeUndoState.pop();

            if (!operationSteps.empty()) {
                currentStepIndex = std::max(0, currentStepIndex - 1);
                while (currentStepIndex > 0 && operationSteps[currentStepIndex].currentState != SHOWING_RESULT && operationSteps[currentStepIndex].currentState != IDLE) {
                    currentStepIndex--;
                }
            }
        }

        notificationMessage = "Reverted to previous tree state";
        isInitialized = tree.root != nullptr;
        currentOperation = "";
        currentPath.clear();
        pathIndex = 0;
        highlightNodes.clear();
        currentState = IDLE;
        stateTimer = 0.0f;
        resultTimer = 0.0f;
    } else {
        notificationMessage = "No previous tree state available";
    }
}

void AVLTreeVisualizer::animateNext() {
    if (!treeRedoState.empty()) {
        AVLTree treeReplica(tree);
        treeUndoState.push(treeReplica);
        tree = treeRedoState.top();
        treeRedoState.pop();

        notificationMessage = "Advanced to next tree state";
        currentOperation = "";
        currentPath.clear();
        pathIndex = 0;
        highlightNodes.clear();
        currentState = IDLE;
        stateTimer = 0.0f;
        resultTimer = 0.0f;

        // Adjust step index to the last step of the next operation
        if (currentStepIndex < static_cast<int>(operationSteps.size()) - 1) {
            currentStepIndex++;
            while (currentStepIndex < static_cast<int>(operationSteps.size()) - 1 &&
                   operationSteps[currentStepIndex].currentState != SHOWING_RESULT &&
                   operationSteps[currentStepIndex].currentState != IDLE) {
                currentStepIndex++;
            }
            const OperationStep& step = operationSteps[currentStepIndex];
            currentOperation = step.operation;
            currentState = step.currentState;
            currentPath = step.currentPath;
            pathIndex = step.pathIndex;
            highlightNodes = step.highlightNodes;
            notificationMessage = step.notificationMessage;
            operationValue = step.operationValue;
            pendingInsertValue = step.pendingInsertValue;
            searchFound = step.searchFound;
            stateTimer = step.stateTimer;
            resultTimer = step.resultTimer;
            rotationIndex = step.rotationIndex;
        }
    } else {
        notificationMessage = "No further tree states to redo";
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

    if (CheckCollisionPointRec(mousePos, slider) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        draggingSlider = true;
    }

    if (draggingSlider) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float mouseX = mousePos.x;
            float t = (mouseX - slider.x) / (slider.width - sliderHandle.width);
            t = std::max(0.0f, std::min(t, 1.0f));

            // Map t to animationSpeed: t=0.5 (middle) -> 0.5f, t=0 -> 0.05f, t=1 -> 5.0f
            if (t <= 0.5f) {
                // Left half: 0.05f to 0.5f
                animationSpeed = 0.05f + (t / 0.5f) * (0.5f - 0.05f);
            } else {
                // Right half: 0.5f to 5.0f
                animationSpeed = 0.5f + ((t - 0.5f) / 0.5f) * (5.0f - 0.5f);
            }

            sliderHandle.x = slider.x + t * (slider.width - sliderHandle.width);
            sliderHandle.y = slider.y + 2.0f; // Keep handle vertically centered
        } else {
            draggingSlider = false;
        }
    }

    return animationSpeed;
}

void AVLTreeVisualizer::animateInsert(int value) {
    currentOperation = "insert";
    operationValue = value;
    pendingInsertValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    notificationMessage = "Inserting " + std::to_string(value);
    currentState = TRAVERSING;
    highlightNodes.clear();

    // Store initial tree snapshot
    AVLTree treeSnapshot(tree);
    OperationStep step = {
        currentOperation, currentState, currentPath, pathIndex, highlightNodes,
        notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
        currentOperation, rotationIndex, treeSnapshot
    };
    operationSteps.push_back(step);
    currentStepIndex = operationSteps.size() - 1;
}
void AVLTreeVisualizer::animateDelete(int value) {
    currentOperation = "delete";
    operationValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    notificationMessage = "Deleting " + std::to_string(value);
    currentState = TRAVERSING;
    highlightNodes.clear();

    // Store initial tree snapshot
    AVLTree treeSnapshot(tree);
    OperationStep step = {
        "delete", currentState, currentPath, pathIndex, highlightNodes,
        notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
        currentOperation, rotationIndex, treeSnapshot
    };
    operationSteps.push_back(step);
    currentStepIndex = operationSteps.size() - 1;
}
void AVLTreeVisualizer::animateSearch(int value) {
    currentOperation = "search";
    operationValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    searchFound = tree.search(value);
    highlightNodes.clear();
    notificationMessage = "Searching " + std::to_string(value);
    currentState = TRAVERSING;

    // Store initial tree snapshot
    AVLTree treeSnapshot(tree);
    OperationStep step = {
        "search", currentState, currentPath, pathIndex, highlightNodes,
        notificationMessage, operationValue, pendingInsertValue, searchFound, stateTimer, resultTimer,
        currentOperation, rotationIndex, treeSnapshot
    };
    operationSteps.push_back(step);
    currentStepIndex = operationSteps.size() - 1;
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