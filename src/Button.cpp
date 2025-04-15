
#include "Button.h"

bool IsButtonClicked(Rectangle button) {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool DrawButton(const char* text, Rectangle bounds, Font font, bool& buttonClicked, const char* buttonMessage) {
    // (Mã hiện có của bạn cho hàm DrawButton)
    // Ví dụ:
    bool clicked = false;
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, bounds)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            buttonClicked = true;
            clicked = true;
        }
        DrawRectangleRec(bounds, Fade(GRAY, 0.5f));
    } else {
        DrawRectangleRec(bounds, LIGHTGRAY);
    }
    Vector2 textSize = MeasureTextEx(font, text, 20, 1);
    DrawTextEx(font, text, { bounds.x + (bounds.width - textSize.x) / 2, bounds.y + (bounds.height - textSize.y) / 2 }, 20, 1, BLACK);
    return clicked;
}

bool DrawBackButton(Texture2D backButtonTexture, Rectangle bounds, bool& buttonClicked) {
    // Kiểm tra xem texture có hợp lệ không
    if (backButtonTexture.id == 0) {
        // Nếu không tải được texture, vẽ một hình chữ nhật tạm thời để kiểm tra vị trí
        DrawRectangleRec(bounds, RED);
        DrawText("Back", bounds.x + 5, bounds.y + 15, 20, WHITE);
    } else {
        // Vẽ hình ảnh nút "Back" với tỷ lệ phù hợp
        Rectangle sourceRect = { 0, 0, (float)backButtonTexture.width, (float)backButtonTexture.height };
        Rectangle destRect = { bounds.x, bounds.y, bounds.width, bounds.height };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(backButtonTexture, sourceRect, destRect, origin, 0.0f, WHITE);
    }

    // Kiểm tra xem nút có được nhấn hay không
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, bounds)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            buttonClicked = true;
            return true;
        }
        // Tạo hiệu ứng khi di chuột qua (làm sáng nút)
        if (backButtonTexture.id != 0) {
            Rectangle sourceRect = { 0, 0, (float)backButtonTexture.width, (float)backButtonTexture.height };
            Rectangle destRect = { bounds.x, bounds.y, bounds.width, bounds.height };
            Vector2 origin = { 0, 0 };
            DrawTexturePro(backButtonTexture, sourceRect, destRect, origin, 0.0f, Fade(WHITE, 0.8f));
        }
    }
    return false;
}