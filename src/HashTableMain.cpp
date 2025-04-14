#include "HashTableMain.h"
#include <stdio.h>
#include <string>
#include <random>

HashTableMain::HashTableMain() : hashTable(1) {
    // Do not call InitWindow here; the window is already created by MainInterface
    shouldClose = false;
}

HashTableMain::~HashTableMain() {
    // Do not call CloseWindow here; the window will be managed by MainInterface
}

void HashTableMain::Run() {
    // Run the Hash Table visualization in the existing window
    hashTable.handleFileDrop();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    hashTable.DrawScreen();

    // Add a way to return to the main menu (e.g., pressing ESC)
    if (IsKeyPressed(KEY_ESCAPE)) {
        shouldClose = true;
    }

    EndDrawing();
}

bool HashTableMain::ShouldClose() {
    return shouldClose || WindowShouldClose();
}