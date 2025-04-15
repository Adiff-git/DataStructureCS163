#include "SLLmain.h"
#include <stdio.h>
#include <string>
#include <random>

SLLmain::SLLmain() : linkedList() {
    // Do not call InitWindow here; the window is already created by MainInterface
    shouldClose = false;
    backToMainMenu = false;
}

SLLmain::~SLLmain() {
    // Do not call CloseWindow here; the window will be managed by MainInterface
}

void SLLmain::Run() {
    // Run the Linked List visualization in the existing window
    linkedList.handleFileDrop();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    linkedList.DrawScreen();

    // Nếu nút "Back" được nhấn, đặt backToMainMenu = true
    if (linkedList.IsBackButtonClicked()) { // Sử dụng getter thay vì truy cập trực tiếp
        backToMainMenu = true;
    }

    // Nếu nhấn ESC hoặc nhấn nút "Back", quay lại màn hình chính
    if (IsKeyPressed(KEY_ESCAPE) || backToMainMenu) {
        shouldClose = true;
    }

    EndDrawing();
}

bool SLLmain::ShouldClose() {
    return shouldClose || WindowShouldClose();
}