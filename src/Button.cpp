#include "Button.h"
#include "raylib.h"

bool DrawButton(const char* text, float x, float y, Font font, bool& buttonClicked, const char*& buttonMessage) { // Vẽ nút và xử lý sự kiện
    Rectangle button = { x, y, 200.0f, 50.0f }; // Tạo hình chữ nhật cho nút
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), button); // Kiểm tra chuột có trên nút không
    bool isClicked = false;                     // Biến kiểm tra nút có được nhấn không

    if (isHovered) {                            // Nếu chuột di chuột qua nút
        DrawRectangleRec(button, DARKGRAY);     // Vẽ nút màu xám đậm
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // Nếu nhấn chuột trái
            isClicked = true;                   // Đánh dấu nút được nhấn
            buttonClicked = true;               // Cập nhật biến ngoài
            buttonMessage = text;               // Lưu thông điệp của nút
        }
    } else {                                    // Nếu chuột không trên nút
        DrawRectangleRec(button, GRAY);         // Vẽ nút màu xám nhạt
    }

    DrawRectangleLines(x, y, 200.0f, 50.0f, BLACK); // Vẽ viền nút
    Vector2 textSize = MeasureTextEx(font, text, 30, 1); // Đo kích thước văn bản
    DrawTextEx(font, text, { x + 100.0f - textSize.x / 2, y + 10.0f }, 30, 1, WHITE); // Vẽ văn bản ở giữa nút

    return isClicked;                           // Trả về trạng thái nhấn nút
}