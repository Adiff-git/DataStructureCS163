#include "../inc/AVLTreeMain.h"

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "AVL Tree Visualizer");
    SetTargetFPS(60);

    AVLTreeVisualizer visualizer;

    while (!WindowShouldClose() && !visualizer.shouldClose) {
        visualizer.handleInput();
        visualizer.updateAnimation(GetFrameTime());
        BeginDrawing();
        visualizer.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}