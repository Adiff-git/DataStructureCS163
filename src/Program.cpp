#include "Program.h"
#include <stdio.h> // Để sử dụng printf
#include <string>
#include <random>
#include "raygui.h"
Program::Program() : hashTable(1) {
    const int screenWidth = 1300;
    const int screenHeight = 786;
    InitWindow(screenWidth, screenHeight, "Data Structure Visualization");
    // GuiLoadStyle("../resources/styles/style_jungle.rgs");
    SetTargetFPS(60);
}

Program::~Program() {
    CloseWindow();
}

void Program::Run() {
    while (!WindowShouldClose()) {
		hashTable.handleFileDrop(); 
        BeginDrawing();
        ClearBackground(RAYWHITE);
        hashTable.DrawScreen();
        EndDrawing();
    }
}

