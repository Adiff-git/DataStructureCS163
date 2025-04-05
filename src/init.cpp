#include "../inc/AVL.h"
#include "../raylib/src/raylib.h"
#include "../inc/Color.h"
#include "../inc/init.h"
#include "../inc/animation.h"

void initAVLProgram() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    // Initialize the window
    InitWindow(screenWidth, screenHeight, "AVL Tree Visualizer");

    // Load the custom font (Raleway-Regular)
    Font customFont = LoadFont("../src/Raleway-Regular.ttf");  // Adjusted path

    // Check if the font loaded correctly
    if (customFont.texture.id == 0) {
        TraceLog(LOG_ERROR, "Font loading failed! Check file path.");
    }
    printf("Font loaded successfully.\n");

    // Set the target frame rate
    SetTargetFPS(60);

    // Create an instance of AVLTreeVisualizer
    AVLTreeVisualizer visualizer;

    // Main game loop
    while (!WindowShouldClose()) {
        // Clear the background to a custom color (Make sure CreamyBlueBG is defined in Color.h)
        ClearBackground(CreamyBlueBG);

        // Get the deltaTime for smooth animation
        float deltaTime = GetFrameTime();

        // Handle user input (mouse clicks, key presses, etc.)
        visualizer.handleInput();

        // Update the animation based on deltaTime
        visualizer.updateAnimation(deltaTime);

        // Begin drawing the frame
        BeginDrawing();

        // Use the custom font for drawing notifications or other text
        Vector2 textPosition = { 50, 50 };  // Adjust position as needed
        // Example of drawing a notification message
        DrawTextEx(customFont, visualizer.notificationMessage.c_str(), textPosition, 24, 2, DARKGRAY);  // Example

        // Draw the AVL Tree visualizer (draw tree, notifications, etc.)
        visualizer.draw();

        // End the drawing process
        EndDrawing();
    }

    // Unload the font to free memory
    UnloadFont(customFont);

    // Close the window and clean up resources
    CloseWindow();
}
