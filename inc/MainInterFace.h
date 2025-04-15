#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include "raylib.h"
#include "SLLmain.h"
#include "HashTableMain.h"
#include "AVLTreeMain.h"
#include "GraphMain.h" // Thêm header cho Graph

class MainInterface {
public:
    MainInterface();
    ~MainInterface();
    void Run();

private:
    enum class ScreenState {
        MAIN_MENU,
        HASH_TABLE,
        LINKED_LIST,
        AVL_TREE,
        GRAPH
    };

    ScreenState currentState;
    SLLmain* linkedListProgram; // Pointer to manage the Linked List program
    HashTableMain* hashTableProgram; // Pointer to manage the Hash Table program
    AVLTreeVisualizer* avlTreeProgram; // Pointer to manage the AVL Tree program
    GraphMain* graphProgram; // Pointer to manage the Graph program

    Texture2D hashTableButton;
    Texture2D linkedListButton;
    Texture2D avlTreeButton;
    Texture2D graphButton;
    Texture2D backButtonTexture; // Texture cho nút Back

    Rectangle hashTableRect;
    Rectangle linkedListRect;
    Rectangle avlTreeRect;
    Rectangle graphRect;

    void LoadTextures();
    void UnloadTextures();
    void DrawInterface();
    void DrawPlaceholderScreen(const char* title);
    void HandleButtonClicks();
};

#endif