#ifndef PROGRAM_H
#define PROGRAM_H

#include "GUI.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"
#include "raylib.h"

class Program {
private:
    Screen currentScreen;
    Font font;
    bool buttonClicked;
    const char* buttonMessage;
    SinglyLinkedList sll;
    HashTable ht;
    AVLTree avl;
    Graph graph;

public:
    Program();
    ~Program();
    void Run();
};

#endif