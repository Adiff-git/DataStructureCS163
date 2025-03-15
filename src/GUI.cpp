#include "GUI.h"
#include "Button.h"
#include "SettingsManager.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"
#include "raylib.h"

static SinglyLinkedList sll;
static HashTable ht(10);
static AVLTree avl;
static Graph graph(5);

void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(BLUE);
    DrawTextEx(font, "Data Structure Visualization", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float buttonX = 600.0f;
    float buttonY = 200.0f;
    if (DrawButton("Data Structures", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::DATA_STRUCTURES;
    }
    if (DrawButton("Settings", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::SETTINGS;
    }
    if (DrawButton("Exit", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        CloseWindow();
    }
}

void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(GREEN);
    DrawTextEx(font, "Settings", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Increase Speed", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        animationSpeed -= 0.1;
        if (animationSpeed < 0.1) animationSpeed = 0.1;
    }
    if (DrawButton("Decrease Speed", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        animationSpeed += 0.1;
        if (animationSpeed > 2.0) animationSpeed = 2.0;
    }

    DrawTextEx(font, TextFormat("Animation Speed: %.1f", animationSpeed), { 50.0f, 300.0f }, 30, 1, DARKGRAY);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(YELLOW);
    DrawTextEx(font, "Choose a Data Structure", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float buttonX = 600.0f;
    float buttonY = 150.0f;
    if (DrawButton("Singly Linked List", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::SINGLY_LINKED_LIST;
    }
    if (DrawButton("Hash Table", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::HASH_TABLE;
    }
    if (DrawButton("AVL Tree", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::AVL_TREE;
    }
    if (DrawButton("Graph", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::GRAPH;
    }
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawSinglyLinkedListScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, SinglyLinkedList* sll) {
    ClearBackground(ORANGE);
    DrawTextEx(font, "Singly Linked List", { 600.0f, 50.0f }, 40, 1, DARKGRAY);
    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize (5)", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        sll->Initialize(5);
        sll->ResetAnimation();
    }
    if (DrawButton("Add (42)", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        sll->Add(42);
        sll->ResetAnimation();
    }
    if (DrawButton("Delete (42)", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        sll->Delete(42);
        sll->ResetAnimation();
    }
    if (DrawButton("Update (42->99)", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        sll->Update(42, 99);
        sll->ResetAnimation();
    }
    if (DrawButton("Search (42)", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        sll->Search(42);
        sll->ResetAnimation();
    }
    sll->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, sll);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawHashTableScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, HashTable* ht) {
    ClearBackground(LIME);
    DrawTextEx(font, "Hash Table", { 600.0f, 50.0f }, 40, 1, DARKGRAY);
    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize (5)", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        ht->Initialize(5);
        ht->ResetAnimation();
    }
    if (DrawButton("Add (42)", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        ht->Add(42);
        ht->ResetAnimation();
    }
    if (DrawButton("Delete (42)", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        ht->Delete(42);
        ht->ResetAnimation();
    }
    if (DrawButton("Update (42->99)", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        ht->Update(42, 99);
        ht->ResetAnimation();
    }
    if (DrawButton("Search (42)", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        ht->Search(42);
        ht->ResetAnimation();
    }
    ht->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, ht);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawAVLTreeScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, AVLTree* avl) {
    ClearBackground(RED);
    DrawTextEx(font, "AVL Tree", { 600.0f, 50.0f }, 40, 1, DARKGRAY);
    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize (5)", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        avl->Initialize(5);
        avl->ResetAnimation();
    }
    if (DrawButton("Add (42)", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        avl->Add(42);
        avl->ResetAnimation();
    }
    if (DrawButton("Delete (42)", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        avl->Delete(42);
        avl->ResetAnimation();
    }
    if (DrawButton("Update (42->99)", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        avl->Update(42, 99);
        avl->ResetAnimation();
    }
    if (DrawButton("Search (42)", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        avl->Search(42);
        avl->ResetAnimation();
    }
    avl->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, avl);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawGraphScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, Graph* graph) {
    ClearBackground(PINK);
    DrawTextEx(font, "Graph", { 600.0f, 50.0f }, 40, 1, DARKGRAY);
    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        graph->Initialize(0);
        graph->ResetAnimation();
    }
    if (DrawButton("Add Edge (0-1)", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        graph->AddEdge(0, 1, 4);
        graph->ResetAnimation();
    }
    if (DrawButton("Delete Vertex (1)", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        graph->Delete(1);
        graph->ResetAnimation();
    }
    if (DrawButton("Update Vertex (1->5)", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        graph->Update(1, 5);
        graph->ResetAnimation();
    }
    if (DrawButton("Search Vertex (1)", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        graph->Search(1);
        graph->ResetAnimation();
    }
    graph->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, graph);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    float buttonX = 50.0f;
    float buttonY = 600.0f;
    if (DrawButton("Back", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        if (currentScreen == Screen::DATA_STRUCTURES || currentScreen == Screen::SETTINGS) {
            currentScreen = Screen::MAIN_MENU;
        } else {
            currentScreen = Screen::DATA_STRUCTURES;
        }
    }
}

void DrawAnimationControls(Font font, bool& buttonClicked, const char*& buttonMessage, DataStructureLogic* ds) {
    float buttonX = 800.0f;
    float buttonY = 500.0f;
    if (DrawButton("Play", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        ds->StartAnimation();
    }
    if (DrawButton("Pause", buttonX + 220.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->PauseAnimation();
    }
    if (DrawButton("Next", buttonX + 440.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->NextStep();
    }
    if (DrawButton("Prev", buttonX + 660.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->PrevStep();
    }

    float stepY = 600.0f;
    for (int i = 0; i < ds->GetTotalSteps(); i++) {
        DrawTextEx(font, ds->GetSteps()[i].c_str(), { 800.0f, stepY + i * 30.0f }, 20, 1, (i == ds->GetCurrentStep()) ? RED : DARKGRAY);
    }
}