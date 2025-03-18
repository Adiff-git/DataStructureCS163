#include "GUI.h"
#include "Button.h"
#include "SettingsManager.h"
#include "DataStructureLogic.h" // Thêm include để định nghĩa đầy đủ DataStructureLogic
#include "raylib.h"

void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(BLUE);
    // Căn giữa tiêu đề
    Vector2 titleSize = MeasureTextEx(font, "Data Structure Visualization", 40, 1);
    float titleX = (GetScreenWidth() - titleSize.x) / 2.0f;
    DrawTextEx(font, "Data Structure Visualization", { titleX, 50.0f }, 40, 1, DARKGRAY);

    // Căn giữa các nút theo chiều ngang và dọc
    float buttonWidth = 300.0f;  // Tăng kích thước nút
    float buttonHeight = 60.0f;
    float buttonX = (GetScreenWidth() - buttonWidth) / 2.0f;  // Căn giữa ngang
    float startY = (GetScreenHeight() - (3 * buttonHeight + 2 * 80.0f)) / 2.0f;  // Căn giữa dọc, 80 là khoảng cách giữa các nút
    float buttonY = startY;

    if (DrawButton("Data Structures", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::DATA_STRUCTURES;
    }
    buttonY += buttonHeight + 80.0f;  // Khoảng cách giữa các nút
    if (DrawButton("Settings", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::SETTINGS;
    }
    buttonY += buttonHeight + 80.0f;
    if (DrawButton("Exit", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        CloseWindow();
    }
}

void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(GREEN);
    // Căn giữa tiêu đề
    Vector2 titleSize = MeasureTextEx(font, "Settings", 40, 1);
    float titleX = (GetScreenWidth() - titleSize.x) / 2.0f;
    DrawTextEx(font, "Settings", { titleX, 50.0f }, 40, 1, DARKGRAY);

    // Căn giữa các nút
    float buttonWidth = 300.0f;
    float buttonHeight = 60.0f;
    float buttonX = (GetScreenWidth() - buttonWidth) / 2.0f;
    float startY = (GetScreenHeight() - (3 * buttonHeight + 2 * 80.0f)) / 2.0f;  // 3 nút: Toggle Speed, Toggle Color, Back
    float buttonY = startY;

    // Nút Toggle Speed
    if (DrawButton("Toggle Speed", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        ToggleAnimationSpeed();  // Gọi hàm từ SettingsManager
    }
    buttonY += buttonHeight + 80.0f;

    // Nút Toggle Color Mode
    if (DrawButton("Toggle Color Mode", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        ToggleColorMode();  // Gọi hàm từ SettingsManager
    }
    buttonY += buttonHeight + 80.0f;

    // Hiển thị thông tin hiện tại
    DrawTextEx(font, TextFormat("Animation Speed: %.1f", animationSpeed), { 50.0f, 500.0f }, 30, 1, DARKGRAY);
    DrawTextEx(font, TextFormat("Color Mode: %s", isLightMode ? "Light" : "Dark"), { 50.0f, 540.0f }, 30, 1, DARKGRAY);

    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(YELLOW);
    DrawTextEx(font, "Choose a Data Structure", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float buttonX = 600.0f;
    float buttonY = 150.0f;
    if (DrawButton("Singly Linked List", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::SINGLY_LINKED_LIST;
    }
    if (DrawButton("Hash Table", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::HASH_TABLE;
    }
    if (DrawButton("AVL Tree", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::AVL_TREE;
    }
    if (DrawButton("Graph", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::GRAPH;
    }
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    float buttonX = 50.0f;
    float buttonY = 600.0f;
    if (DrawButton("Back", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        if (currentScreen == Screen::DATA_STRUCTURES || currentScreen == Screen::SETTINGS) {
            currentScreen = Screen::MAIN_MENU;
        } else {
            currentScreen = Screen::DATA_STRUCTURES;
        }
    }
}

void DrawAnimationControls(Font font, bool& buttonClicked, const char*& buttonMessage, DataStructureLogic* ds) {
    float buttonX = 800.0f;
    float buttonY = 500.0f;
    if (DrawButton("Play", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        ds->StartAnimation();
    }
    if (DrawButton("Pause", buttonX + 220.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->PauseAnimation();
    }
    if (DrawButton("Next", buttonX + 440.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->NextStep();
    }
    if (DrawButton("Prev", buttonX + 660.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->PrevStep();
    }

    float stepY = 600.0f;
    for (int i = 0; i < ds->GetTotalSteps(); i++) {
        DrawTextEx(font, ds->GetSteps()[i].c_str(), { 800.0f, stepY + i * 30.0f }, 20, 1, (i == ds->GetCurrentStep()) ? RED : DARKGRAY); // Thêm tham số spacing = 1
    }
}