#include "../inc/AVL.h"
#include "../raylib/src/raylib.h"
#include "../inc/Color.h"
#include "../inc/init.h"
#include "../inc/animation.h"

void initAVLProgram() {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "AVL Tree Visualizer");
    SetTargetFPS(60);

    AVLTreeVisualizer visualizer;

    while (!WindowShouldClose()) {
        ClearBackground(CreamyBlueBG);
        float deltaTime = GetFrameTime();
        visualizer.handleInput();
        visualizer.updateAnimation(deltaTime);
        BeginDrawing();
        visualizer.draw();
        EndDrawing();
    }

    CloseWindow();
}
