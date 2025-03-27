#include "raylib.h"
#include "LinkedList.h"
#include "Button.h"
#include "Utils.h"
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
    bool showInput = false;
    bool showInitOptions = false;
    bool showRandomInput = false;
    bool showEnterInput = false;
    bool showExternalFile = false;
    std::string inputText = "";
    std::string inputTextOld = "";
    std::string inputTextNew = "";
    int inputType = 0;
    Vector2 inputPosition = {0, 0};
    Vector2 inputPositionOld = {0, 0};
    Vector2 inputPositionNew = {0, 0};

    Rectangle initButton = {20, 20, 100, 40};
    Rectangle addButton = {130, 20, 100, 40};
    Rectangle deleteButton = {240, 20, 100, 40};
    Rectangle searchButton = {350, 20, 100, 40};
    Rectangle updateButton = {460, 20, 100, 40};
    Rectangle randomButton = {20, 70, 100, 40};
    Rectangle enterButton = {20, 120, 100, 40};
    Rectangle externalFileButton = {20, 170, 100, 40};
    Rectangle inputBox = {0, 0, 200, 40};
    Rectangle inputBoxOld = {0, 0, 100, 40};
    Rectangle inputBoxNew = {0, 0, 100, 40};
    Rectangle okButton = {0, 0, 100, 40};

    Rectangle prevButton = {screenWidth / 2 - 150, screenHeight - 100, 60, 40};
    Rectangle playButton = {screenWidth / 2 - 75, screenHeight - 100, 60, 40};
    Rectangle nextButton = {screenWidth / 2, screenHeight - 100, 60, 40};
    Rectangle skipButton = {screenWidth / 2 + 75, screenHeight - 100, 60, 40};
    Rectangle slider = {screenWidth / 2 - 150, screenHeight - 50, 300, 20};
    Rectangle scrollBar = {50, screenHeight - 100, 200, 20};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        list.UpdateScroll(screenWidth, screenHeight);
        list.UpdateAnimation();
        list.ProcessOperations();
        list.UpdateOperations();

        DrawRectangleRec(initButton, LIGHTGRAY);
        DrawText("Initialize", initButton.x + 10, initButton.y + 10, 20, BLACK);
        DrawRectangleRec(addButton, LIGHTGRAY);
        DrawText("Add", addButton.x + 30, addButton.y + 10, 20, BLACK);
        DrawRectangleRec(deleteButton, LIGHTGRAY);
        DrawText("Delete", deleteButton.x + 20, deleteButton.y + 10, 20, BLACK);
        DrawRectangleRec(searchButton, LIGHTGRAY);
        DrawText("Search", searchButton.x + 20, searchButton.y + 10, 20, BLACK);
        DrawRectangleRec(updateButton, LIGHTGRAY);
        DrawText("Update", updateButton.x + 20, updateButton.y + 10, 20, BLACK);

        DrawRectangleRec(slider, LIGHTGRAY);
        float speed = UpdateSlider(slider, 0.1f, 2.0f, list.GetSpeed());
        list.SetSpeed(speed);
        float sliderPos = (speed - 0.1f) / (2.0f - 0.1f) * slider.width;
        DrawRectangle(slider.x + sliderPos - 5, slider.y - 5, 10, 30, DARKGRAY);
        DrawText(("Speed: " + std::to_string(speed).substr(0, 4)).c_str(), slider.x, slider.y - 30, 20, BLACK);

        DrawRectangleRec(prevButton, LIGHTGRAY);
        DrawText("Prev", prevButton.x + 10, prevButton.y + 10, 20, BLACK);
        DrawRectangleRec(playButton, LIGHTGRAY);
        DrawText(list.IsPaused() ? "Play" : "Pause", playButton.x + 5, playButton.y + 10, 20, BLACK);
        DrawRectangleRec(nextButton, LIGHTGRAY);
        DrawText("Next", nextButton.x + 10, nextButton.y + 10, 20, BLACK);
        DrawRectangleRec(skipButton, LIGHTGRAY);
        DrawText("Skip", skipButton.x + 10, skipButton.y + 10, 20, BLACK);

        DrawRectangleRec(scrollBar, LIGHTGRAY);
        float scrollPos = (-list.GetScrollOffset() / (screenWidth + 200)) * scrollBar.width;
        DrawRectangle(scrollBar.x + scrollPos - 5, scrollBar.y - 5, 10, 30, DARKGRAY);

        if (!list.GetNotification().empty()) {
            DrawText(list.GetNotification().c_str(), screenWidth / 2 - MeasureText(list.GetNotification().c_str(), 20) / 2, 50, 20, RED);
        }

        if (IsButtonClicked(initButton)) {
            showInitOptions = true;
            showInput = false;
            showRandomInput = false;
            showEnterInput = false;
            showExternalFile = false;
            inputText = "";
        }
        if (IsButtonClicked(addButton)) {
            showInitOptions = false;
            showInput = true;
            inputType = 1;
            inputText = "";
            inputPosition = {addButton.x, addButton.y + addButton.height + 10};
        }
        if (IsButtonClicked(deleteButton)) {
            showInitOptions = false;
            showInput = true;
            inputType = 2;
            inputText = "";
            inputPosition = {deleteButton.x, deleteButton.y + deleteButton.height + 10};
        }
        if (IsButtonClicked(searchButton)) {
            showInitOptions = false;
            showInput = true;
            inputType = 3;
            inputText = "";
            inputPosition = {searchButton.x, searchButton.y + searchButton.height + 10};
        }
        if (IsButtonClicked(updateButton)) {
            showInitOptions = false;
            showInput = true;
            inputType = 4;
            inputTextOld = "";
            inputTextNew = "";
            inputPositionOld = {updateButton.x, updateButton.y + updateButton.height + 10};
            inputPositionNew = {updateButton.x + 150, updateButton.y + updateButton.height + 10};
        }
        if (IsButtonClicked(prevButton)) list.PreviousStep();
        if (IsButtonClicked(playButton)) list.SetPaused(!list.IsPaused());
        if (IsButtonClicked(nextButton)) list.NextStep();
        if (IsButtonClicked(skipButton)) list.SkipToEnd();

        if (showInitOptions) {
            DrawRectangleRec(randomButton, LIGHTGRAY);
            DrawText("Random", randomButton.x + 10, randomButton.y + 10, 20, BLACK);
            DrawRectangleRec(enterButton, LIGHTGRAY);
            DrawText("Enter", enterButton.x + 10, enterButton.y + 10, 20, BLACK);
            DrawRectangleRec(externalFileButton, LIGHTGRAY);
            DrawText("External File", externalFileButton.x + 10, externalFileButton.y + 10, 20, BLACK);

            if (IsButtonClicked(randomButton)) {
                showRandomInput = true;
                showEnterInput = false;
                showExternalFile = false;
                inputText = "";
                inputPosition = {randomButton.x + randomButton.width + 10, randomButton.y};
            }
            if (IsButtonClicked(enterButton)) {
                showRandomInput = false;
                showEnterInput = true;
                showExternalFile = false;
                inputText = "";
                inputPosition = {enterButton.x + enterButton.width + 10, enterButton.y};
            }
            if (IsButtonClicked(externalFileButton)) {
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
            DrawRectangleRec(okButton, LIGHTGRAY);
            DrawText("OK", okButton.x + 30, okButton.y + 10, 20, BLACK);

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

            if (IsButtonClicked(okButton)) {
                if (!inputText.empty()) {
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
                        while (ss >> value) {
                            values.push_back(value);
                        }
                        if (!values.empty()) {
                            list.EnqueueOperationWithValues(1, values);
                        } else {
                            list.ClearList();
                            list.SetNotification("No valid numbers entered.");
                        }
                    }
                    showInitOptions = false;
                    showRandomInput = false;
                    showEnterInput = false;
                    showExternalFile = false;
                    list.SetNotification("");
                }
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

        if (showInput) {
            if (inputType != 4) {
                inputBox.x = inputPosition.x;
                inputBox.y = inputPosition.y;
                okButton.x = inputPosition.x;
                okButton.y = inputPosition.y + 50;
                DrawRectangleRec(inputBox, WHITE);
                DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, BLACK);
                DrawText(inputText.c_str(), inputBox.x + 10, inputBox.y + 10, 20, BLACK);
                DrawRectangleRec(okButton, LIGHTGRAY);
                DrawText("OK", okButton.x + 30, okButton.y + 10, 20, BLACK);

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

                if (IsButtonClicked(okButton)) {
                    if (!inputText.empty()) {
                        int value = std::stoi(inputText);
                        list.EnqueueOperation(inputType + 1, value);
                        showInput = false;
                    }
                }
            } else {
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

                DrawRectangleRec(okButton, LIGHTGRAY);
                DrawText("OK", okButton.x + 30, okButton.y + 10, 20, BLACK);

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

                if (IsButtonClicked(okButton)) {
                    if (!inputTextOld.empty() && !inputTextNew.empty()) {
                        int oldValue = std::stoi(inputTextOld);
                        int newValue = std::stoi(inputTextNew);
                        list.EnqueueOperation(5, oldValue, newValue);
                        showInput = false;
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