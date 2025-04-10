#include "Button.h"

bool IsButtonClicked(Rectangle button) {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, slider) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float sliderWidth = slider.width;
        float mouseX = mousePos.x - slider.x;
        float t = mouseX / sliderWidth;
        return minValue + t * (maxValue - minValue);
    }
    return currentValue;
}

bool DrawButton(const char* text, Rectangle button, Font font, bool& buttonClicked, const char*& buttonMessage) {
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
        DrawRectangleRec(button, LIGHTGRAY); // Thay GRAY bằng LIGHTGRAY để khớp với giao diện gốc
    }

    DrawRectangleLines(button.x, button.y, button.width, button.height, BLACK);
    Vector2 textSize = MeasureTextEx(font, text, 20, 1); // Giảm kích thước font xuống 20 để khớp với giao diện gốc
    DrawTextEx(font, text, { button.x + button.width / 2 - textSize.x / 2, button.y + button.height / 2 - textSize.y / 2 }, 20, 1, BLACK); // Văn bản căn giữa

    return isClicked;
}