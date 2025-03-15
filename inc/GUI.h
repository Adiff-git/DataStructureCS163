#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include "DataStructureLogic.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"

enum class Screen {
    MAIN_MENU,
    SETTINGS,
    DATA_STRUCTURES,
    SINGLY_LINKED_LIST,
    HASH_TABLE,
    AVL_TREE,
    GRAPH
};

void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawSinglyLinkedListScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, SinglyLinkedList* sll);
void DrawHashTableScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, HashTable* ht);
void DrawAVLTreeScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, AVLTree* avl);
void DrawGraphScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, Graph* graph);
void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawAnimationControls(Font font, bool& buttonClicked, const char*& buttonMessage, DataStructureLogic* ds);

#endif