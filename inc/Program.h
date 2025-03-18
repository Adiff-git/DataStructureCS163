#ifndef PROGRAM_H
#define PROGRAM_H

#include "raylib.h"
#include "GUI.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"

class Program {
public:
    Program();
    ~Program();
    void Run();

private:
    Font font;
    SinglyLinkedList singlyLinkedList;
    HashTable hashTable;
    AVLTree avlTree;
    Graph graph;
    bool buttonClicked;
    const char* buttonMessage;
    Screen currentScreen;
};

#endif