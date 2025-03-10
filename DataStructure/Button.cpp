// Button.cpp
#include "raylib.h"
#include "Button.h"

void InitButton(Button& button, float x, float y, float width, float height, const char* text) {
    button.rect = { x, y, width, height };
    button.text = text;
    button.isClicked = false;
}

void DrawButton(Button& button) {
    // Vẽ button trên giao diện
    if (button.isClicked) {
        DrawRectangleRec(button.rect, GREEN);  // Màu khi nhấn
    }
    else {
        DrawRectangleRec(button.rect, BLUE);   // Màu bình thường
    }
    DrawText(button.text, button.rect.x + 10, button.rect.y + 10, 20, WHITE);
}

bool CheckButtonClick(Button& button) {
    if (CheckCollisionPointRec(GetMousePosition(), button.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        button.isClicked = true;
        return true;
    }
    button.isClicked = false;
    return false;
}