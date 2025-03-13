#include "raylib.h"
#include "Button.h"

bool DrawButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char*& buttonMessage) {
    Rectangle buttonRect = { static_cast<float>(x), static_cast<float>(y), 800.0f, 50.0f };

    Color buttonColor = LIGHTGRAY;
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
        buttonColor = DARKGRAY;  // Change color when mouse hovers
    }

    DrawRectangleRec(buttonRect, buttonColor);
    Vector2 textSize = MeasureTextEx(font, text, 30, 1);
    Vector2 textPosition = { (buttonRect.x + buttonRect.width / 2) - textSize.x / 2, 
                             buttonRect.y + buttonRect.height / 2 - textSize.y / 2 };
    DrawTextEx(font, text, textPosition, 30, 1, DARKBLUE);  // Changed text color to DARKBLUE

    if (CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        buttonClicked = true;
        buttonMessage = text;
        return true;  // Return true when button is clicked
    }
    return false;  // Return false if not clicked
}
