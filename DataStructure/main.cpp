#include "raylib.h"

int main() {
    // Khởi tạo cửa sổ
    InitWindow(800, 600, "Background Example");

    // Nạp ảnh background từ thư mục resources/images/
    Texture2D background = LoadTexture("resources/images/SegmentTree.png");

    // Vòng lặp chính
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Vẽ background lên màn hình
        DrawTexture(background, 0, 0, WHITE);  // Vẽ từ góc trên bên trái

        // Vẽ các đối tượng khác (ví dụ, văn bản)
        DrawText("Hello, World!", 100, 100, 40, DARKGREEN);

        EndDrawing();
    }

    // Giải phóng tài nguyên
    UnloadTexture(background);  // Giải phóng ảnh background khi không sử dụng nữa
    CloseWindow();  // Đóng cửa sổ khi thoát

    return 0;
}
