// #include "Button.h"
// #include "raylib.h"

// static const Color BUTTON_DEFAULT_COLOR = { 200, 200, 200, 255 };  // Màu xám mặc định
// static const Color BUTTON_HOVER_COLOR = { 240, 240, 240, 255 };    // Sáng hơn khi hover
// static const Color BUTTON_PRESSED_COLOR = { 150, 150, 150, 255 };  // Tối hơn khi nhấn
// static const Color BUTTON_BORDER_COLOR = { 100, 100, 100, 255 };   // Viền
// static const Color TEXT_COLOR = { 50, 50, 50, 255 };               // Màu chữ

// void DrawStyledButton(Rectangle bounds, const char* text, bool pressed) {
//     Vector2 mousePoint = GetMousePosition();
//     bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
//     bool isPressed = pressed || (isHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON));

//     Color buttonColor;
//     if (isPressed) {
//         buttonColor = BUTTON_PRESSED_COLOR;  // Khi nhấn
//     } else if (isHovered) {
//         buttonColor = BUTTON_HOVER_COLOR;    // Khi hover
//     } else {
//         buttonColor = BUTTON_DEFAULT_COLOR;  // Trạng thái bình thường
//     }

//     // Vẽ nền nút
//     DrawRectangleRec(bounds, buttonColor);

//     // Vẽ viền
//     DrawRectangleLinesEx(bounds, 1, BUTTON_BORDER_COLOR);

//     // Tính toán vị trí chữ để căn giữa
//     int fontSize = 20;
//     Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
//     Vector2 textPos = {
//         bounds.x + (bounds.width - textSize.x) / 2,
//         bounds.y + (bounds.height - textSize.y) / 2
//     };

//     // Vẽ chữ
//     DrawTextEx(GetFontDefault(), text, textPos, fontSize, 1, TEXT_COLOR);
// }

// bool IsButtonClicked(Rectangle bounds) {
//     Vector2 mousePoint = GetMousePosition();
//     bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
//     return isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
// }

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