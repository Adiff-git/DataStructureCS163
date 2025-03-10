// GUI.cpp
#include "raylib.h"
#include "GUI.h"

void InitializeWindow(int width, int height, const char* title) {
    InitWindow(width, height, title);
    SetTargetFPS(60);
}

void DrawUI() {
    // Vẽ các thành phần UI như buttons, text boxes, v.v.
    DrawText("Press 'A' to Add Node", 10, 10, 20, WHITE);
}

void HandleInput() {
    if (IsKeyPressed(KEY_A)) {
        // Xử lý khi người dùng nhấn 'A'
    }
}
