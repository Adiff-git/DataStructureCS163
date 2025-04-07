#include "Button.h"
#include "raylib.h"

static const Color BUTTON_DEFAULT_COLOR = { 200, 200, 200, 255 };  // Màu xám mặc định
static const Color BUTTON_HOVER_COLOR = { 240, 240, 240, 255 };    // Sáng hơn khi hover
static const Color BUTTON_PRESSED_COLOR = { 150, 150, 150, 255 };  // Tối hơn khi nhấn
static const Color BUTTON_BORDER_COLOR = { 100, 100, 100, 255 };   // Viền
static const Color TEXT_COLOR = { 50, 50, 50, 255 };               // Màu chữ

void DrawStyledButton(Rectangle bounds, const char* text, bool pressed) {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
    bool isPressed = pressed || (isHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON));

    Color buttonColor;
    if (isPressed) {
        buttonColor = BUTTON_PRESSED_COLOR;  // Khi nhấn
    } else if (isHovered) {
        buttonColor = BUTTON_HOVER_COLOR;    // Khi hover
    } else {
        buttonColor = BUTTON_DEFAULT_COLOR;  // Trạng thái bình thường
    }

    // Vẽ nền nút
    DrawRectangleRec(bounds, buttonColor);

    // Vẽ viền
    DrawRectangleLinesEx(bounds, 1, BUTTON_BORDER_COLOR);

    // Tính toán vị trí chữ để căn giữa
    int fontSize = 20;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2,
        bounds.y + (bounds.height - textSize.y) / 2
    };

    // Vẽ chữ
    DrawTextEx(GetFontDefault(), text, textPos, fontSize, 1, TEXT_COLOR);
}

bool IsButtonClicked(Rectangle bounds) {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
    return isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}