#include "Button.h"

bool DrawButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char*& buttonMessage) {
    Rectangle buttonRect = {(float)x, (float)y, 800, 50};
    Color buttonColor = LIGHTGRAY;
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) buttonColor = YELLOW;
    DrawRectangleRec(buttonRect, buttonColor);
    Vector2 textSize = MeasureTextEx(font, text, 30, 1);
    Vector2 textPosition = {buttonRect.x + buttonRect.width / 2 - textSize.x / 2, buttonRect.y + buttonRect.height / 2 - textSize.y / 2};
    DrawTextEx(font, text, textPosition, 30, 1, DARKBLUE);
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        buttonClicked = true;
        buttonMessage = text;
        return true;
    }
    return false;
}

bool DrawSmallButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char*& buttonMessage) {
    Rectangle buttonRect = {(float)x, (float)y, 50, 50};
    Color buttonColor = LIGHTGRAY;
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) buttonColor = YELLOW;
    DrawRectangleRec(buttonRect, buttonColor);
    Vector2 textSize = MeasureTextEx(font, text, 20, 1);
    Vector2 textPosition = {buttonRect.x + buttonRect.width / 2 - textSize.x / 2, buttonRect.y + buttonRect.height / 2 - textSize.y / 2};
    DrawTextEx(font, text, textPosition, 20, 1, DARKBLUE);
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        buttonClicked = true;
        buttonMessage = text;
        return true;
    }
    return false;
}