#include "GUI.h"
#include "Button.h"
#include "SettingsManager.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"

static SinglyLinkedList sll;
static HashTable ht(10);
static AVLTree avl;
static Graph graph(5);

static int currentStep = 0;
static bool isRunning = false;
static double lastStepTime = 0.0;

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
    if (DrawButton("Increase Animation Speed", xPos, 290, font, buttonClicked, buttonMessage)) IncreaseAnimationSpeed();
    if (DrawButton("Decrease Animation Speed", xPos, 360, font, buttonClicked, buttonMessage)) DecreaseAnimationSpeed();
    DrawTextEx(font, TextFormat("Animation Speed: %.1f s/step", animationSpeed), {static_cast<float>(xPos), 430}, 30, 1, DARKBLUE);
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
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) {
        sll.Initialize(5);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Add (42)", xPos, 220, font, buttonClicked, buttonMessage)) {
        sll.Add(42);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Delete (42)", xPos, 290, font, buttonClicked, buttonMessage)) {
        sll.Delete(42);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Update (42->99)", xPos, 360, font, buttonClicked, buttonMessage)) {
        sll.Update(42, 99);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Search (42)", xPos, 430, font, buttonClicked, buttonMessage)) {
        sll.Search(42);
        currentStep = 0;
        isRunning = false;
    }
    sll.Draw(font, 600, 400, currentStep);
    DrawAnimationControls(font, buttonClicked, buttonMessage, currentStep, sll.GetTotalSteps(), isRunning);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawHashTableScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(LIME);
    DrawTextEx(font, "Hash Table", {600, 50}, 40, 1, DARKGRAY);
    int xPos = 300;
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) {
        ht.Initialize(5);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Add (42)", xPos, 220, font, buttonClicked, buttonMessage)) {
        ht.Add(42);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Delete (42)", xPos, 290, font, buttonClicked, buttonMessage)) {
        ht.Delete(42);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Update (42->99)", xPos, 360, font, buttonClicked, buttonMessage)) {
        ht.Update(42, 99);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Search (42)", xPos, 430, font, buttonClicked, buttonMessage)) {
        ht.Search(42);
        currentStep = 0;
        isRunning = false;
    }
    ht.Draw(font, 600, 400, currentStep);
    DrawAnimationControls(font, buttonClicked, buttonMessage, currentStep, ht.GetTotalSteps(), isRunning);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawAVLTreeScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(RED);
    DrawTextEx(font, "AVL Tree", {600, 50}, 40, 1, DARKGRAY);
    int xPos = 300;
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) {
        avl.Initialize(5);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Add (42)", xPos, 220, font, buttonClicked, buttonMessage)) {
        avl.Add(42);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Delete (42)", xPos, 290, font, buttonClicked, buttonMessage)) {
        avl.Delete(42);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Update (42->99)", xPos, 360, font, buttonClicked, buttonMessage)) {
        avl.Update(42, 99);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Search (42)", xPos, 430, font, buttonClicked, buttonMessage)) {
        avl.Search(42);
        currentStep = 0;
        isRunning = false;
    }
    avl.Draw(font, 600, 400, currentStep);
    DrawAnimationControls(font, buttonClicked, buttonMessage, currentStep, avl.GetTotalSteps(), isRunning);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawGraphScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(PINK);
    DrawTextEx(font, "Graph", {600, 50}, 40, 1, DARKGRAY);
    int xPos = 300;
    if (DrawButton("Initialize", xPos, 150, font, buttonClicked, buttonMessage)) {
        graph.Initialize();
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Add Edge (0-1)", xPos, 220, font, buttonClicked, buttonMessage)) {
        graph.AddEdge(0, 1, 4);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Delete Edge (0-1)", xPos, 290, font, buttonClicked, buttonMessage)) {
        graph.DeleteEdge(0, 1);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Update Edge (0-1->5)", xPos, 360, font, buttonClicked, buttonMessage)) {
        graph.UpdateEdge(0, 1, 5);
        currentStep = 0;
        isRunning = false;
    }
    if (DrawButton("Search Edge (0-1)", xPos, 430, font, buttonClicked, buttonMessage)) {
        graph.SearchEdge(0, 1);
        currentStep = 0;
        isRunning = false;
    }
    graph.Draw(font, 600, 400, currentStep);
    DrawAnimationControls(font, buttonClicked, buttonMessage, currentStep, graph.GetTotalSteps(), isRunning);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    if (DrawSmallButton("Back", 50, 50, font, buttonClicked, buttonMessage)) currentScreen = MAIN_MENU;
}

void DrawAnimationControls(Font font, bool& buttonClicked, const char*& buttonMessage, int& currentStep, int totalSteps, bool& isRunning) {
    int xPos = 300;
    int yPos = 600;
    if (DrawButton("Previous Step", xPos, yPos, font, buttonClicked, buttonMessage)) {
        if (currentStep > 0) currentStep--;
        isRunning = false;
    }
    if (DrawButton("Next Step", xPos + 300, yPos, font, buttonClicked, buttonMessage)) {
        if (currentStep < totalSteps - 1) currentStep++;
        isRunning = false;
    }
    if (DrawButton(isRunning ? "Stop" : "Run All", xPos + 600, yPos, font, buttonClicked, buttonMessage)) {
        isRunning = !isRunning;
        if (isRunning) {
            currentStep = 0;
            lastStepTime = GetTime();
        }
    }
    DrawTextEx(font, TextFormat("Step: %d/%d", currentStep + 1, totalSteps), {static_cast<float>(xPos + 300), static_cast<float>(yPos + 70)}, 30, 1, DARKBLUE);

    if (isRunning && currentStep < totalSteps - 1) {
        double currentTime = GetTime();
        if (currentTime - lastStepTime >= animationSpeed) {
            currentStep++;
            lastStepTime = currentTime;
        }
    }
}