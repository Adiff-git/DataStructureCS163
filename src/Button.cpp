#include "raylib.h"
#include "Button.h"

// Hàm vẽ button
void DrawButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char*& buttonMessage) {
    // Định nghĩa kích thước của button
    Rectangle buttonRect = { x, y, 500, 50 };  // Rectangle cho button

    // Vẽ button
    DrawRectangleRec(buttonRect, LIGHTGRAY);

    // Tính toán vị trí văn bản để căn giữa trong button
    Vector2 textSize = MeasureTextEx(font, text, 30, 1);
    Vector2 textPosition = { (buttonRect.x + buttonRect.width / 2) - textSize.x / 2, buttonRect.y + buttonRect.height / 2 - textSize.y / 2 };

    // Vẽ văn bản vào button
    DrawTextEx(font, text, textPosition, 30, 1, DARKGRAY);

    // Kiểm tra xem người dùng có click vào button không
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        buttonClicked = true;  // Cập nhật trạng thái click
        buttonMessage = text;  // Lưu thông báo khi button được nhấn
    }

    // Nếu button được nhấn, hiển thị thông báo trên màn hình
    if (buttonClicked) {
        DrawText(buttonMessage, 250, 450, 30, DARKGREEN);  // Hiển thị thông báo
    }
}
