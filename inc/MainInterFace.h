#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include "raylib.h"
#include "SLLmain.h"
#include "HashTableMain.h"
#include "AVLTreeMain.h" // Thêm header cho AVL Tree

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

    Texture2D hashTableButton;
    Texture2D linkedListButton;
    Texture2D avlTreeButton;
    Texture2D graphButton;

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