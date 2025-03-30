#include "Program.h"
#include <stdio.h> // Để sử dụng printf
#include <string>
#include <random>
#include "raygui.h"

#define CUSTOM_STYLE_PROP_COUNT  2

static const GuiStyleProp customStyle[CUSTOM_STYLE_PROP_COUNT] = {
    { 0, 16, (int)0x00000012 },    // DEFAULT_TEXT_SIZE 
    { 0, 20, (int)0x0000001a },    // DEFAULT_TEXT_LINE_SPACING 
};

static void GuiLoadStyleLight(void)
{
    // Load style properties provided
    // NOTE: Default properties are propagated
    for (int i = 0; i < CUSTOM_STYLE_PROP_COUNT; i++)
    {
        GuiSetStyle(customStyle[i].controlId, customStyle[i].propertyId, customStyle[i].propertyValue);
    }
};

Program::Program() : hashTable(1) {
    const int screenWidth = 1500;
    const int screenHeight = 786;
    InitWindow(screenWidth, screenHeight, "Data Structure Visualization");
	//GuiLoadStyle("../resources/styles/style_jungle.rgs");
	GuiLoadStyleLight();
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

