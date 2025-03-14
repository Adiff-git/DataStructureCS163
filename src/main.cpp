#include "raylib.h"
#include "GUI.h"
#include "SettingsManager.h"

int main() {
    InitWindow(screenWidth, screenHeight, "Data Visualization Group 1");
    SetTargetFPS(60);

    Font myFont = LoadFont("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\Rubik-Italic-VariableFont_wght.ttf");
    Screen currentScreen = MAIN_MENU;
    bool buttonClicked = false;
    const char* buttonMessage = "";
    Vector2 pos = {350, 50};

    while (!WindowShouldClose()) {
        BeginDrawing();
        if (isLightMode) ClearBackground(WHITE);
        else ClearBackground(BLACK);

        switch (currentScreen) {
            case MAIN_MENU:
                DrawMainMenu(myFont, buttonClicked, buttonMessage, pos, currentScreen);
                break;
            case SETTINGS:
                DrawSettingsMenu(myFont, buttonClicked, buttonMessage, currentScreen);
                break;
            case DATA_STRUCTURES:
                DrawDataStructuresMenu(myFont, buttonClicked, buttonMessage, currentScreen);
                break;
            case SINGLY_LINKED_LIST:
                DrawSinglyLinkedListScreen(myFont, buttonClicked, buttonMessage, currentScreen);
                break;
            case HASH_TABLE:
                DrawHashTableScreen(myFont, buttonClicked, buttonMessage, currentScreen);
                break;
            case AVL_TREE:
                DrawAVLTreeScreen(myFont, buttonClicked, buttonMessage, currentScreen);
                break;
            case GRAPH:
                DrawGraphScreen(myFont, buttonClicked, buttonMessage, currentScreen);
                break;
        }

        EndDrawing();
    }

    UnloadFont(myFont);
    CloseWindow();
    return 0;
}