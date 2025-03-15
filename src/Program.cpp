#include "Program.h"
#include "GUI.h"
#include "raylib.h"
#include "SettingsManager.h"

Program::Program() : currentScreen(Screen::MAIN_MENU), buttonClicked(false), buttonMessage(""), ht(10), graph(5) {
    InitWindow(1400, 800, "Data Structure Visualization");
    SetTargetFPS(60);
    font = LoadFont("resources/Rubik-Italic-VariableFont_wght.ttf");
}

Program::~Program() {
    UnloadFont(font);
    CloseWindow();
}

void Program::Run() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        sll.SetAnimationSpeed(animationSpeed);
        ht.SetAnimationSpeed(animationSpeed);
        avl.SetAnimationSpeed(animationSpeed);
        graph.SetAnimationSpeed(animationSpeed);

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
                DrawSinglyLinkedListScreen(font, buttonClicked, buttonMessage, currentScreen, &sll);
                sll.UpdateAnimation();
                break;
            case Screen::HASH_TABLE:
                DrawHashTableScreen(font, buttonClicked, buttonMessage, currentScreen, &ht);
                ht.UpdateAnimation();
                break;
            case Screen::AVL_TREE:
                DrawAVLTreeScreen(font, buttonClicked, buttonMessage, currentScreen, &avl);
                avl.UpdateAnimation();
                break;
            case Screen::GRAPH:
                DrawGraphScreen(font, buttonClicked, buttonMessage, currentScreen, &graph);
                graph.UpdateAnimation();
                break;
        }
        if (buttonClicked) {
            buttonClicked = false;
        }
        EndDrawing();
    }
}