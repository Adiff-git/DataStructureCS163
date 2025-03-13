#include "raylib.h"
#include "GUI.h"

// Define screen states

int main() {
    InitWindow(1820, 880, "Data Visualization Group 1");
    Vector2 pos = { 600, 50 };
    Font myFont = LoadFont("D:\\Downloads\\sproject\\DataStructure\\resources\\fonts\\Rubik-Italic-VariableFont_wght.ttf");

    Screen currentScreen = MENU;  // Initial state is the menu
    bool buttonClicked = false;
    const char* buttonMessage = "";

    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE);  // Clear screen with a bright background

        if (currentScreen == MENU) {
            DrawMenu(myFont, buttonClicked, buttonMessage, pos, currentScreen);  // Pass state to DrawMenu function
        }
        else if (currentScreen == SINGLY_LINKED_LIST) {
            ClearBackground(ORANGE);  // Change color for different screen
            DrawText("Singly Linked List Screen", 600, 200, 30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        }
        else if (currentScreen == HASH_TABLE) {
            ClearBackground(LIME);  // Another screen with different background
            DrawText("Hash Table Screen", 600, 200, 30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        } else if ( currentScreen == AVL_TREE) {
            ClearBackground(RED); // Another screen with different background
            DrawText("AVL Tree Screen", 600, 200,30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        } else if (currentScreen == GRAPH) {
            ClearBackground(PINK); // Another screen with different background
            DrawText("Graph Screen", 600, 200,30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        }
        // Add similar screens for AVL_TREE, and GRAPH

        EndDrawing();

        // Check for exit condition and change screen if necessary
        if (buttonClicked) {
            if (buttonMessage == "Singly Linked List") {
                currentScreen = SINGLY_LINKED_LIST;
            } else if (buttonMessage == "Hash Table") {
                currentScreen = HASH_TABLE;
            } else if (buttonMessage == "AVL Tree") {
                currentScreen = AVL_TREE;
            } else if (buttonMessage == "Graph") {
                currentScreen = GRAPH;
            }
            buttonClicked = false;  // Reset button click state
        }
    }

    UnloadFont(myFont);  // Unload font
    CloseWindow();  // Close window
    return 0;
}
