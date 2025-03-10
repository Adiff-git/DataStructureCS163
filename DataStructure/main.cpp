#include "raylib.h"

int main() {
    // Khởi tạo cửa sổ
    InitWindow(800, 600, "Font Example");

    // Nạp font từ thư mục resources/fonts/
    Font myFont = LoadFont("resources/fonts/Rubik-VariableFont_wght.ttf");

    // Khởi tạo vị trí cho văn bản
    Vector2 position = { 100, 100 }; // Khai báo vị trí (Vector2)

    // Vòng lặp chính
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Vẽ văn bản với font đã nạp
        DrawTextEx(myFont, "Hello, World!", position, 40, 2, DARKGREEN);

        EndDrawing();
    }

    // Giải phóng tài nguyên
    UnloadFont(myFont);
    CloseWindow();  // Đóng cửa sổ khi thoát

    return 0;
}
