#include "raylib.h"
#include "GUI.h"
#include "Button.h"

void DrawMenu(Font font, bool buttonClicked, const char* buttonMessage, Vector2 pos, Screen &currentScreen) {
    DrawTextEx(font, "Data Visualization Group 1", pos, 80, 1, YELLOW);

    // Draw buttons with colorful backgrounds and different layouts
    if (DrawButton("Singly Linked List", 600, 150, font, buttonClicked, buttonMessage)) {
        currentScreen = SINGLY_LINKED_LIST;
    }
    if (DrawButton("Hash Table", 600, 220, font, buttonClicked, buttonMessage)) {
        currentScreen = HASH_TABLE;
    }
    if (DrawButton("AVL Tree", 600, 290, font, buttonClicked, buttonMessage)) {
        currentScreen = AVL_TREE;
    }
    if (DrawButton("Graph", 600, 360, font, buttonClicked, buttonMessage)) {
        currentScreen = GRAPH;
    }
}

void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    if (DrawButton("Back", 600, 600, font, buttonClicked, buttonMessage)) {
        currentScreen = MENU;  // Go back to the menu
    }
}
