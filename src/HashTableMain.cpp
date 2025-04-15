#include "HashTableMain.h"
#include <stdio.h>
#include <string>
#include <random>

HashTableMain::HashTableMain() : hashTable(1) {
    shouldClose = false;
    backToMainMenu = false; // Khởi tạo biến
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

    // Nếu nút "Back" được nhấn, đặt backToMainMenu = true
    if (hashTable.IsBackButtonClicked()) { // Sử dụng getter thay vì truy cập trực tiếp
        backToMainMenu = true;
    }

    // Nếu nhấn ESC hoặc nhấn nút "Back", quay lại màn hình chính
    if (IsKeyPressed(KEY_ESCAPE) || backToMainMenu) {
        shouldClose = true;
    }

    EndDrawing();
}

bool HashTableMain::ShouldClose() {
    return shouldClose || WindowShouldClose();
}