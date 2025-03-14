#ifndef GUI_H
#define GUI_H
#include "raylib.h"

const int screenWidth = 1440;
const int screenHeight = 810;

enum Screen {
    MAIN_MENU, SETTINGS, DATA_STRUCTURES, SINGLY_LINKED_LIST, HASH_TABLE, AVL_TREE, GRAPH
};

void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Vector2 pos, Screen& currentScreen);
void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawSinglyLinkedListScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawHashTableScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawAVLTreeScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawGraphScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);

#endif