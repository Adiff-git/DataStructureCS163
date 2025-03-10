#include "raylib.h"
#include "GUI.h"
#include "Menu.h"

int main() {
    // Khởi tạo cửa sổ
    InitWindow(800, 600, "Data Visualization Group 1");

    // Nạp font từ thư mục resources/fonts/
    Font myFont = LoadFont("resources/fonts/myFont.ttf");

    // Biến để kiểm tra trạng thái click button
    bool buttonClicked = false;
    const char* buttonMessage = "";

    // Vòng lặp chính
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);  // Xóa màn hình

        // Vẽ menu
        DrawMenu(myFont, buttonClicked, buttonMessage);  // Vẽ giao diện và truyền trạng thái click

        EndDrawing();
    }

    // Giải phóng tài nguyên
    UnloadFont(myFont);  // Giải phóng font khi không sử dụng nữa
    CloseWindow();  // Đóng cửa sổ khi thoát

    return 0;
}
