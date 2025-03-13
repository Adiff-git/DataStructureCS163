#ifndef GUI_H
#define GUI_H

#include "raylib.h"

// Enum to define screen states
enum Screen {
    MENU,
    SINGLY_LINKED_LIST,
    HASH_TABLE,
    AVL_TREE,
    GRAPH
};

// Function to draw the main menu
void DrawMenu(Font font, bool buttonClicked, const char* buttonMessage, Vector2 pos, Screen &currentScreen);

// Function to draw the "Back" button for navigating back to the main menu
void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);

#endif // GUI_H
