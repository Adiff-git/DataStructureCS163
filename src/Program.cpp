#include "Program.h"
#include <stdio.h> // Để sử dụng printf

Program::Program() : hashTable(10), graph(5), buttonClicked(false), buttonMessage(nullptr), currentScreen(Screen::MAIN_MENU) {
    const int screenWidth = 1800;
    const int screenHeight = 824;
    InitWindow(screenWidth, screenHeight, "Data Structure Visualization");
    SetTargetFPS(60);
    font = LoadFont("resources/Rubik-Italic-VariableFont_wght.ttf");
}

Program::~Program() {
    UnloadFont(font); // Giải phóng font thành viên
    CloseWindow();
}

void Program::Run() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScreen) {
            case Screen::MAIN_MENU:
                DrawMainMenu(font, buttonClicked, buttonMessage, currentScreen);
                break;
            case Screen::SETTINGS:
                DrawSettingsMenu(font, buttonClicked, buttonMessage, currentScreen);
                break;
            case Screen::DATA_STRUCTURES:
                DrawDataStructuresMenu(font, buttonClicked, buttonMessage, currentScreen);
                break;
            case Screen::SINGLY_LINKED_LIST:
                singlyLinkedList.DrawScreen(font, buttonClicked, buttonMessage, currentScreen);
                break;
            case Screen::HASH_TABLE:
                hashTable.DrawScreen(font, buttonClicked, buttonMessage, currentScreen);
                break;
            case Screen::AVL_TREE:
                avlTree.DrawScreen(font, buttonClicked, buttonMessage, currentScreen);
                break;
            case Screen::GRAPH:
                graph.DrawScreen(font, buttonClicked, buttonMessage, currentScreen);
                break;
        }

        if (buttonClicked) {
            if (buttonMessage) {
                DrawTextEx(font, buttonMessage, {50.0f, 650.0f}, 30, 1, RED);
            }
            buttonClicked = false;
        }

        EndDrawing();
    }
}

