#include "Button.h"
#include "raylib.h"

bool DrawButton(const char* text, float x, float y, Font font, bool& buttonClicked, const char*& buttonMessage) {
    Rectangle button = { x, y, 200.0f, 50.0f };
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), button);
    bool isClicked = false;

    if (isHovered) {
        DrawRectangleRec(button, DARKGRAY);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isClicked = true;
            buttonClicked = true;
            buttonMessage = text;
        }
    } else {
        DrawRectangleRec(button, GRAY);
    }

    DrawRectangleLines(x, y, 200.0f, 50.0f, BLACK);
    Vector2 textSize = MeasureTextEx(font, text, 30, 1);
    DrawTextEx(font, text, { x + 100.0f - textSize.x / 2, y + 10.0f }, 30, 1, WHITE);

    return isClicked;
}