#include "raylib.h"
#include "LinkedList.h"
#include "Button.h"
#include "Utils.h"
#include "ControlManager.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Singly Linked List Visualization");
    SetTargetFPS(60);

    LinkedList list(screenWidth, screenHeight);
    list.SetPaused(true);  // Start with the operation paused
    ControlManager control(list, screenWidth, screenHeight, GetFontDefault());

    bool showInput = false;
    bool showInitOptions = false;
    bool showRandomInput = false;
    bool showEnterInput = false;
    bool showExternalFile = false;
    bool showAddOptions = false;
    bool showAddHeadInput = false;
    bool showAddTailInput = false;
    bool showAddIndexInput = false;
    std::string inputText = "";
    std::string inputTextOld = "";
    std::string inputTextNew = "";
    int inputType = 0;
    Vector2 inputPosition = {0.0f, 0.0f};
    Vector2 inputPositionOld = {0.0f, 0.0f};
    Vector2 inputPositionNew = {0.0f, 0.0f};

    bool buttonClicked = false;
    const char* buttonMessage = "";
    Font font = GetFontDefault();

    Rectangle initButton = {20, 20, 100, 40};
    Rectangle addButton = {130, 20, 100, 40};
    Rectangle deleteButton = {240, 20, 100, 40};
    Rectangle searchButton = {350, 20, 100, 40};
    Rectangle updateButton = {460, 20, 100, 40};
    Rectangle randomButton = {20, 70, 100, 40};
    Rectangle enterButton = {20, 120, 100, 40};
    Rectangle externalFileButton = {20, 170, 100, 40};
    Rectangle addHeadButton = {130, 70, 100, 40};
    Rectangle addIndexButton = {130, 120, 100, 40};
    Rectangle addTailButton = {130, 170, 100, 40};
    Rectangle inputBox = {0, 0, 200, 40};
    Rectangle inputBoxOld = {0, 0, 100, 40};
    Rectangle inputBoxNew = {0, 0, 100, 40};
    Rectangle okButton = {0, 0, 100, 40};
    Rectangle scrollBar = {50, screenHeight - 100, 200, 20};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        list.UpdateScroll(screenWidth, screenHeight);
        list.UpdateAnimation();
        list.ProcessOperations();
        list.UpdateOperations();

        control.Update();
        control.Draw();

        if (DrawButton("Initialize", initButton, font, buttonClicked, buttonMessage)) {
            showInitOptions = true;
            showAddOptions = false;
            showInput = false;
            showRandomInput = false;
            showEnterInput = false;
            showExternalFile = false;
            showAddHeadInput = false;
            showAddIndexInput = false;
            showAddTailInput = false;
            inputText = "";
        }
        if (DrawButton("Add", addButton, font, buttonClicked, buttonMessage)) {
            showInitOptions = false;
            showAddOptions = true;
            showInput = false;
            showRandomInput = false;
            showEnterInput = false;
            showExternalFile = false;
            showAddHeadInput = false;
            showAddIndexInput = false;
            showAddTailInput = false;
            inputText = "";
        }
        if (DrawButton("Delete", deleteButton, font, buttonClicked, buttonMessage)) {
            showAddOptions = false;
            showInput = true;
            inputType = 2;
            inputText = "";
            inputPosition = {deleteButton.x, deleteButton.y + deleteButton.height + 10};
        }
        if (DrawButton("Search", searchButton, font, buttonClicked, buttonMessage)) {
            showAddOptions = false;
            showInput = true;
            inputType = 3;
            inputText = "";
            inputPosition = {searchButton.x, searchButton.y + searchButton.height + 10};
        }
        if (DrawButton("Update", updateButton, font, buttonClicked, buttonMessage)) {
            showAddOptions = false;
            showInput = true;
            inputType = 4;
            inputTextOld = "";
            inputTextNew = "";
            inputPositionOld = {updateButton.x, updateButton.y + updateButton.height + 10};
            inputPositionNew = {updateButton.x + 150, updateButton.y + updateButton.height + 10};
        }

        DrawRectangleRec(scrollBar, LIGHTGRAY);
        float scrollPos = (-list.GetScrollOffset() / (screenWidth + 200)) * scrollBar.width;
        DrawRectangle(scrollBar.x + scrollPos - 5, scrollBar.y - 5, 10, 30, DARKGRAY);

        if (!list.GetNotification().empty()) {
            DrawText(list.GetNotification().c_str(), screenWidth / 2 - MeasureText(list.GetNotification().c_str(), 20) / 2, 50, 20, RED);
        }

        if (showAddOptions) {
            if (DrawButton("Head", addHeadButton, font, buttonClicked, buttonMessage)) {
                showAddHeadInput = true;
                showAddIndexInput = false;
                showAddTailInput = false;
                inputText = "";
                inputPosition = {addHeadButton.x + addHeadButton.width + 10, addHeadButton.y};
            }
            if (DrawButton("Index", addIndexButton, font, buttonClicked, buttonMessage)) {
                showAddHeadInput = false;
                showAddIndexInput = true;
                showAddTailInput = false;
                inputTextOld = "";
                inputTextNew = "";
                inputPositionOld = {addIndexButton.x + addIndexButton.width + 10, addIndexButton.y};
                inputPositionNew = {addIndexButton.x + addIndexButton.width + 120, addIndexButton.y};
            }
            if (DrawButton("Tail", addTailButton, font, buttonClicked, buttonMessage)) {
                showAddHeadInput = false;
                showAddIndexInput = false;
                showAddTailInput = true;
                inputText = "";
                inputPosition = {addTailButton.x + addTailButton.width + 10, addTailButton.y};
            }
        }

        if (showAddHeadInput) {
            inputBox.x = inputPosition.x;
            inputBox.y = inputPosition.y;
            okButton.x = inputPosition.x;
            okButton.y = inputPosition.y + 50;
            DrawRectangleRec(inputBox, WHITE);
            DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, BLACK);
            DrawText("Number:", inputBox.x - 80, inputBox.y + 10, 20, BLACK);
            DrawText(inputText.c_str(), inputBox.x + 10, inputBox.y + 10, 20, BLACK);
            if (DrawButton("OK", okButton, font, buttonClicked, buttonMessage) && !inputText.empty()) {
                int value = std::stoi(inputText);
                list.EnqueueOperation(2, value);
                showAddOptions = false;
                showAddHeadInput = false;
            }

            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48 && key <= 57) && inputText.length() < 10) {
                    inputText += (char)key;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
                inputText.pop_back();
            }
        }

        if (showAddTailInput) {
            inputBox.x = inputPosition.x;
            inputBox.y = inputPosition.y;
            okButton.x = inputPosition.x;
            okButton.y = inputPosition.y + 50;
            DrawRectangleRec(inputBox, WHITE);
            DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, BLACK);
            DrawText("Number:", inputBox.x - 80, inputBox.y + 10, 20, BLACK);
            DrawText(inputText.c_str(), inputBox.x + 10, inputBox.y + 10, 20, BLACK);
            if (DrawButton("OK", okButton, font, buttonClicked, buttonMessage) && !inputText.empty()) {
                int value = std::stoi(inputText);
                list.EnqueueOperation(4, value);
                showAddOptions = false;
                showAddTailInput = false;
            }

            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48 && key <= 57) && inputText.length() < 10) {
                    inputText += (char)key;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
                inputText.pop_back();
            }
        }

        if (showAddIndexInput) {
            inputBoxOld.x = inputPositionOld.x;
            inputBoxOld.y = inputPositionOld.y;
            inputBoxNew.x = inputPositionNew.x;
            inputBoxNew.y = inputPositionNew.y;
            okButton.x = inputPositionOld.x;
            okButton.y = inputPositionOld.y + 50;

            DrawRectangleRec(inputBoxOld, WHITE);
            DrawRectangleLines(inputBoxOld.x, inputBoxOld.y, inputBoxOld.width, inputBoxOld.height, BLACK);
            DrawText("Index:", inputBoxOld.x - 60, inputBoxOld.y + 10, 20, BLACK);
            DrawText(inputTextOld.c_str(), inputBoxOld.x + 10, inputBoxOld.y + 10, 20, BLACK);

            DrawRectangleRec(inputBoxNew, WHITE);
            DrawRectangleLines(inputBoxNew.x, inputBoxNew.y, inputBoxNew.width, inputBoxNew.height, BLACK);
            DrawText("Number:", inputBoxNew.x - 80, inputBoxNew.y + 10, 20, BLACK);
            DrawText(inputTextNew.c_str(), inputBoxNew.x + 10, inputBoxNew.y + 10, 20, BLACK);

            if (DrawButton("OK", okButton, font, buttonClicked, buttonMessage) && !inputTextOld.empty() && !inputTextNew.empty()) {
                int index = std::stoi(inputTextOld);
                int value = std::stoi(inputTextNew);
                list.EnqueueOperation(3, index, value);
                showAddOptions = false;
                showAddIndexInput = false;
            }

            if (CheckCollisionPointRec(GetMousePosition(), inputBoxOld)) {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 48 && key <= 57) && inputTextOld.length() < 10) {
                        inputTextOld += (char)key;
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && !inputTextOld.empty()) {
                    inputTextOld.pop_back();
                }
            }

            if (CheckCollisionPointRec(GetMousePosition(), inputBoxNew)) {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 48 && key <= 57) && inputTextNew.length() < 10) {
                        inputTextNew += (char)key;
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && !inputTextNew.empty()) {
                    inputTextNew.pop_back();
                }
            }
        }

        if (showInitOptions) {
            if (DrawButton("Random", randomButton, font, buttonClicked, buttonMessage)) {
                showRandomInput = true;
                showEnterInput = false;
                showExternalFile = false;
                inputText = "";
                inputPosition = {randomButton.x + randomButton.width + 10, randomButton.y};
            }
            if (DrawButton("Enter", enterButton, font, buttonClicked, buttonMessage)) {
                showRandomInput = false;
                showEnterInput = true;
                showExternalFile = false;
                inputText = "";
                inputPosition = {enterButton.x + enterButton.width + 10, enterButton.y};
            }
            if (DrawButton("External File", externalFileButton, font, buttonClicked, buttonMessage)) {
                showRandomInput = false;
                showEnterInput = false;
                showExternalFile = true;
                list.SetNotification("Please drag and drop a text file to initialize.");
            }
        }

        if (showRandomInput || showEnterInput) {
            inputBox.x = inputPosition.x;
            inputBox.y = inputPosition.y;
            okButton.x = inputPosition.x;
            okButton.y = inputPosition.y + 50;
            DrawRectangleRec(inputBox, WHITE);
            DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, BLACK);
            DrawText(inputText.c_str(), inputBox.x + 10, inputBox.y + 10, 20, BLACK);
            if (DrawButton("OK", okButton, font, buttonClicked, buttonMessage) && !inputText.empty()) {
                if (showRandomInput) {
                    int count = std::stoi(inputText);
                    if (count > 0) {
                        list.EnqueueOperation(1, count);
                    } else {
                        list.ClearList();
                    }
                } else if (showEnterInput) {
                    std::vector<int> values;
                    std::stringstream ss(inputText);
                    int value;
                    bool hasValidNumber = false;
                    while (ss >> value) {
                        values.push_back(value);
                        hasValidNumber = true;
                    }
                    if (!hasValidNumber) {
                        list.ClearList();
                        list.SetNotification("No valid numbers entered. Please enter numbers separated by spaces (e.g., 12 32 43).");
                    } else {
                        list.EnqueueOperationWithValues(1, values);
                    }
                }
                showInitOptions = false;
                showRandomInput = false;
                showEnterInput = false;
                showExternalFile = false;
                list.SetNotification("");
            }

            int key = GetCharPressed();
            while (key > 0) {
                if (showRandomInput && (key >= 48 && key <= 57) && inputText.length() < 10) {
                    inputText += (char)key;
                } else if (showEnterInput && ((key >= 48 && key <= 57) || key == 32) && inputText.length() < 50) {
                    inputText += (char)key;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
                inputText.pop_back();
            }
        }

        if (showExternalFile && IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0) {
                list.LoadFromFile(droppedFiles.paths[0]);
            }
            UnloadDroppedFiles(droppedFiles);
            showInitOptions = false;
            showRandomInput = false;
            showEnterInput = false;
            showExternalFile = false;
            list.SetNotification("");
        }

        if (showInput && inputType != 1) {
            if (inputType == 2 || inputType == 3) {
                inputBox.x = inputPosition.x;
                inputBox.y = inputPosition.y;
                okButton.x = inputPosition.x;
                okButton.y = inputPosition.y + 50;
                DrawRectangleRec(inputBox, WHITE);
                DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, BLACK);
                DrawText("Value:", inputBox.x - 60, inputBox.y + 10, 20, BLACK);
                DrawText(inputText.c_str(), inputBox.x + 10, inputBox.y + 10, 20, BLACK);
                if (DrawButton("OK", okButton, font, buttonClicked, buttonMessage) && !inputText.empty()) {
                    int value = std::stoi(inputText);
                    if (inputType == 2) {
                        list.EnqueueOperation(5, value);
                    } else {
                        list.EnqueueOperation(6, value);
                    }
                    showInput = false;
                }

                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 48 && key <= 57) && inputText.length() < 10) {
                        inputText += (char)key;
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
                    inputText.pop_back();
                }
            } else if (inputType == 4) {
                inputBoxOld.x = inputPositionOld.x;
                inputBoxOld.y = inputPositionOld.y;
                inputBoxNew.x = inputPositionNew.x;
                inputBoxNew.y = inputPositionNew.y;
                okButton.x = inputPositionOld.x;
                okButton.y = inputPositionOld.y + 50;

                DrawRectangleRec(inputBoxOld, WHITE);
                DrawRectangleLines(inputBoxOld.x, inputBoxOld.y, inputBoxOld.width, inputBoxOld.height, BLACK);
                DrawText("Old:", inputBoxOld.x - 50, inputBoxOld.y + 10, 20, BLACK);
                DrawText(inputTextOld.c_str(), inputBoxOld.x + 10, inputBoxOld.y + 10, 20, BLACK);

                DrawRectangleRec(inputBoxNew, WHITE);
                DrawRectangleLines(inputBoxNew.x, inputBoxNew.y, inputBoxNew.width, inputBoxNew.height, BLACK);
                DrawText("New:", inputBoxNew.x - 50, inputBoxNew.y + 10, 20, BLACK);
                DrawText(inputTextNew.c_str(), inputBoxNew.x + 10, inputBoxNew.y + 10, 20, BLACK);

                if (DrawButton("OK", okButton, font, buttonClicked, buttonMessage) && !inputTextOld.empty() && !inputTextNew.empty()) {
                    int oldValue = std::stoi(inputTextOld);
                    int newValue = std::stoi(inputTextNew);
                    list.EnqueueOperation(7, oldValue, newValue);
                    showInput = false;
                }

                if (CheckCollisionPointRec(GetMousePosition(), inputBoxOld)) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= 48 && key <= 57) && inputTextOld.length() < 10) {
                            inputTextOld += (char)key;
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && !inputTextOld.empty()) {
                        inputTextOld.pop_back();
                    }
                }

                if (CheckCollisionPointRec(GetMousePosition(), inputBoxNew)) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= 48 && key <= 57) && inputTextNew.length() < 10) {
                            inputTextNew += (char)key;
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && !inputTextNew.empty()) {
                        inputTextNew.pop_back();
                    }
                }
            }
        }

        list.Draw();
        list.DrawCodeBlock();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}