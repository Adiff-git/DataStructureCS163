#ifndef HASH_TABLE_MAIN_H
#define HASH_TABLE_MAIN_H

#include "raylib.h"
#include "HashTable.h"

class HashTableMain {
public:
    HashTableMain();
    ~HashTableMain();
    void Run();
    bool ShouldClose(); // Check if the visualization should close

private:
    HashTable hashTable;
    bool shouldClose; // Flag to indicate if the visualization should close
    bool backToMainMenu; // Flag to indicate if the "Back" button was clicked
};

#endif