#include "raylib.h"
#include "Screen.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Data Structure Visualization");

    Screen screen;

    // Set up transition callback
    screen.SetTransitionCallback([&screen](ScreenType type) {
        screen.SetupScreen(type);
    });

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Handle input and render the current screen
        screen.HandleInput();
        screen.Render();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
