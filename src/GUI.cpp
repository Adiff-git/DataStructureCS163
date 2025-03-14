#include "GUI.h"
#include "Button.h"
#include "SettingsManager.h"
#include "LinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"

static SinglyLinkedList sll;
static HashTable ht(10); // Kích thước mặc định 10
static AVLTree avl;
static Graph graph(5); // 5 đỉnh mặc định

void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Vector2 pos, Screen& currentScreen) {
    DrawTextEx(font, "Data Visualization Group 1", pos, 80, 1, RED);
    int xPos = (screenWidth - 800) / 2;
    if (DrawButton("Start", xPos, 150, font, buttonClicked, buttonMessage)) currentScreen = DATA_STRUCTURES;
    if (DrawButton("Settings", xPos, 220, font, buttonClicked, buttonMessage)) currentScreen = SETTINGS;
    if (DrawButton("End", xPos, 290, font, buttonClicked, buttonMessage)) CloseWindow();
}

void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    int xPos = (screenWidth - 800) / 2;
    if (DrawButton(soundEnabled ? "Disable Sound" : "Enable Sound", xPos, 150, font, buttonClicked, buttonMessage)) ToggleSound();
    if (DrawButton(isLightMode ? "Switch to Dark Mode" : "Switch to Light Mode", xPos, 220, font, buttonClicked, buttonMessage)) ToggleColorMode();
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    int xPos = (screenWidth - 800) / 2;
    if (DrawButton("Singly Linked List", xPos, 150, font, buttonClicked, buttonMessage)) currentScreen = SINGLY_LINKED_LIST;
    if (DrawButton("Hash Table", xPos, 220, font, buttonClicked, buttonMessage)) currentScreen = HASH_TABLE;
    if (DrawButton("AVL Tree", xPos, 290, font, buttonClicked, buttonMessage)) currentScreen = AVL_TREE;
    if (DrawButton("Graph", xPos, 360, font, buttonClicked, buttonMessage)) currentScreen = GRAPH;
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawSinglyLinkedListScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(ORANGE);
    DrawTextEx(font, "Singly Linked List", {600, 50}, 40, 1, DARKGRAY);
    int xPos = 300;
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) sll.Initialize(5);
    if (DrawButton("Add (42)", xPos, 220, font, buttonClicked, buttonMessage)) sll.Add(42);
    if (DrawButton("Delete (42)", xPos, 290, font, buttonClicked, buttonMessage)) sll.Delete(42);
    if (DrawButton("Update (42->99)", xPos, 360, font, buttonClicked, buttonMessage)) sll.Update(42, 99);
    if (DrawButton("Search (42)", xPos, 430, font, buttonClicked, buttonMessage)) sll.Search(42);
    sll.Draw(font, 600, 400);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawHashTableScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(LIME);
    DrawTextEx(font, "Hash Table", {600, 50}, 40, 1, DARKGRAY);
    int xPos = 300;
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) ht.Initialize(5);
    if (DrawButton("Add (42)", xPos, 220, font, buttonClicked, buttonMessage)) ht.Add(42);
    if (DrawButton("Delete (42)", xPos, 290, font, buttonClicked, buttonMessage)) ht.Delete(42);
    if (DrawButton("Update (42->99)", xPos, 360, font, buttonClicked, buttonMessage)) ht.Update(42, 99);
    if (DrawButton("Search (42)", xPos, 430, font, buttonClicked, buttonMessage)) ht.Search(42);
    ht.Draw(font, 600, 400);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawAVLTreeScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(RED);
    DrawTextEx(font, "AVL Tree", {600, 50}, 40, 1, DARKGRAY);
    int xPos = 300;
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) avl.Initialize(5);
    if (DrawButton("Add (42)", xPos, 220, font, buttonClicked, buttonMessage)) avl.Add(42);
    if (DrawButton("Delete (42)", xPos, 290, font, buttonClicked, buttonMessage)) avl.Delete(42);
    if (DrawButton("Update (42->99)", xPos, 360, font, buttonClicked, buttonMessage)) avl.Update(42, 99);
    if (DrawButton("Search (42)", xPos, 430, font, buttonClicked, buttonMessage)) avl.Search(42);
    avl.Draw(font, 600, 400);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawGraphScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(PINK);
    DrawTextEx(font, "Graph", {600, 50}, 40, 1, DARKGRAY);
    int xPos = 300;
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) graph.Initialize();
    if (DrawButton("Add Edge (0-1)", xPos, 220, font, buttonClicked, buttonMessage)) graph.AddEdge(0, 1, 4);
    if (DrawButton("Delete Edge (0-1)", xPos, 290, font, buttonClicked, buttonMessage)) graph.DeleteEdge(0, 1);
    if (DrawButton("Update Edge (0-1->5)", xPos, 360, font, buttonClicked, buttonMessage)) graph.UpdateEdge(0, 1, 5);
    if (DrawButton("Search Edge (0-1)", xPos, 430, font, buttonClicked, buttonMessage)) graph.SearchEdge(0, 1);
    graph.Draw(font, 600, 400);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    if (DrawSmallButton("Back", 50, 50, font, buttonClicked, buttonMessage)) currentScreen = MAIN_MENU;
}