#include "raylib.h"
#include "GUI.h"
#include "Button.h"
#include "SettingsManager.h"  // Include the SettingsManager for handling sound and color mode

// Function to draw the main menu
void DrawMainMenu(Font font, bool buttonClicked, const char* buttonMessage, Vector2 pos, Screen &currentScreen) {
    DrawTextEx(font, "Data Visualization Group 1", pos, 80, 1, RED);

    // Calculate button positions based on the screen width and height for centering
    int buttonWidth = 800;
    int buttonHeight = 50;
    int xPos = (screenWidth - buttonWidth) / 2;  // Center the buttons horizontally

    // Vertical positions for main menu buttons
    int yPos1 = 150;
    int yPos2 = 220;
    int yPos3 = 290;

    // Draw buttons for the Main Menu
    if (DrawButton("Start", xPos, yPos1, font, buttonClicked, buttonMessage)) {
        currentScreen = DATA_STRUCTURES;  // Go to the data structure selection screen
    }
    if (DrawButton("Settings", xPos, yPos2, font, buttonClicked, buttonMessage)) {
        currentScreen = SETTINGS;  // Go to Settings screen
    }
    if (DrawButton("End", xPos, yPos3, font, buttonClicked, buttonMessage)) {
        CloseWindow();  // Close the application
    }
}

// Function to draw the settings menu
void DrawSettingsMenu(Font font, bool buttonClicked, const char* buttonMessage, Screen &currentScreen) {
    int buttonWidth = 800;
    int buttonHeight = 50;
    int xPos = (screenWidth - buttonWidth) / 2;  // Center the buttons horizontally

    // Vertical positions for settings buttons
    int yPos1 = 150;
    int yPos2 = 220;

    // Draw the settings menu
    if (DrawButton(soundEnabled ? "Disable Sound" : "Enable Sound", xPos, yPos1, font, buttonClicked, buttonMessage)) {
        ToggleSound();  // Toggle the sound setting
    }
    if (DrawButton(isLightMode ? "Switch to Dark Mode" : "Switch to Light Mode", xPos, yPos2, font, buttonClicked, buttonMessage)) {
        ToggleColorMode();  // Toggle the color mode
    }

    // Add a back button
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

// Function to draw the back button
void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    if (DrawSmallButton("Back", 50, 50, font, buttonClicked, buttonMessage)) {
        currentScreen = MAIN_MENU;  // Go back to the main menu
    }
}

// Function to draw the data structure menu
void DrawDataStructuresMenu(Font font, bool buttonClicked, const char* buttonMessage, Screen &currentScreen) {
    // Calculate button positions based on the screen width and height for centering
    int buttonWidth = 800;
    int buttonHeight = 50;
    int xPos = (screenWidth - buttonWidth) / 2;  // Center the buttons horizontally

    // Vertical positions for buttons
    int yPos1 = 150;
    int yPos2 = 220;
    int yPos3 = 290;
    int yPos4 = 360;

    // Draw buttons for the Data Structures menu
    if (DrawButton("Singly Linked List", xPos, yPos1, font, buttonClicked, buttonMessage)) {
        currentScreen = SINGLY_LINKED_LIST;
    }
    if (DrawButton("Hash Table", xPos, yPos2, font, buttonClicked, buttonMessage)) {
        currentScreen = HASH_TABLE;
    }
    if (DrawButton("AVL Tree", xPos, yPos3, font, buttonClicked, buttonMessage)) {
        currentScreen = AVL_TREE;
    }
    if (DrawButton("Graph", xPos, yPos4, font, buttonClicked, buttonMessage)) {
        currentScreen = GRAPH;
    }

    // Add a back button
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}
