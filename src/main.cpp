#include "raylib.h"
#include "GUI.h"
#include "SettingsManager.h"  // Include the SettingsManager for sound and color mode

int main() {
    // Initialize window
    InitWindow(screenWidth, screenHeight, "Data Visualization Group 1");

    // Load font
    Font myFont = LoadFont("D:\\Downloads\\sproject\\DataStructure\\resources\\fonts\\Rubik-Italic-VariableFont_wght.ttf");

    // Set initial screen state
    Screen currentScreen = MAIN_MENU;
    bool buttonClicked = false;
    const char* buttonMessage = "";

    // Define initial position for the title
    Vector2 pos = { 350, 50 };

    // Main game loop
    while (!WindowShouldClose()) {
        // Start drawing
        BeginDrawing();

        // Apply color based on Light/Dark mode
        if (isLightMode) {
            ClearBackground(WHITE);  // Light background
        } else {
            ClearBackground(BLACK);  // Dark background
        }

        // Draw different screens based on the current state
        if (currentScreen == MAIN_MENU) {
            DrawMainMenu(myFont, buttonClicked, buttonMessage, pos, currentScreen);  // Draw the main menu
        } else if (currentScreen == SETTINGS) {
            DrawSettingsMenu(myFont, buttonClicked, buttonMessage, currentScreen);  // Draw the settings menu
        } else if (currentScreen == DATA_STRUCTURES) {
            DrawDataStructuresMenu(myFont, buttonClicked, buttonMessage, currentScreen);  // Draw the data structure selection menu
        } else if (currentScreen == SINGLY_LINKED_LIST) {
            ClearBackground(ORANGE);
            DrawText("Singly Linked List Screen", 600, 200, 30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        } else if (currentScreen == HASH_TABLE) {
            ClearBackground(LIME);
            DrawText("Hash Table Screen", 600, 200, 30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        } else if (currentScreen == AVL_TREE) {
            ClearBackground(RED);
            DrawText("AVL Tree Screen", 600, 200, 30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        } else if (currentScreen == GRAPH) {
            ClearBackground(PINK);
            DrawText("Graph Screen", 600, 200, 30, DARKGRAY);
            DrawBackButton(myFont, buttonClicked, buttonMessage, currentScreen);
        }

        // End drawing
        EndDrawing();

        // Check for button click and change screen if necessary
        if (buttonClicked) {
            if (buttonMessage == "Start") {
                currentScreen = DATA_STRUCTURES;  // Go to the data structure selection screen
            } else if (buttonMessage == "Settings") {
                currentScreen = SETTINGS;  // Go to Settings screen
            } else if (buttonMessage == "End") {
                CloseWindow();  // Close the application
            }
            buttonClicked = false;  // Reset button click state
        }
    }

    // Clean up resources
    UnloadFont(myFont);  // Unload font
    CloseWindow();  // Close window
    return 0;
}
