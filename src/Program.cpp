#include "Program.h"
#include <stdio.h> // Để sử dụng printf
#include <string>
#include <random>
#include "raygui.h"

#define CUSTOM_STYLE_PROP_COUNT  9

static const GuiStyleProp customStyle[CUSTOM_STYLE_PROP_COUNT] = {
    { 0, 3, (int)0x838383ff },    // DEFAULT_BORDER_COLOR_FOCUSED 
    { 0, 4, (int)0xc9c9c9ff },    // DEFAULT_BASE_COLOR_FOCUSED 
    { 0, 5, (int)0x686868ff },    // DEFAULT_TEXT_COLOR_FOCUSED 
    { 0, 6, (int)0x838383ff },    // DEFAULT_BORDER_COLOR_PRESSED 
    { 0, 7, (int)0xc9c9c9ff },    // DEFAULT_BASE_COLOR_PRESSED 
    { 0, 8, (int)0x686868ff },    // DEFAULT_TEXT_COLOR_PRESSED
    { 0, 16, (int)0x00000012 },    // DEFAULT_TEXT_SIZE 
    { 0, 17, (int)0x00000001 },    // DEFAULT_TEXT_SPACING
    { 0, 20, (int)0x0000001a },    // DEFAULT_TEXT_LINE_SPACING 
};

static void GuiLoadStyleLight(void)
{
    // Load style properties provided
    // NOTE: Default properties are propagated
    for (int i = 0; i < CUSTOM_STYLE_PROP_COUNT; i++) {
        GuiSetStyle(customStyle[i].controlId, customStyle[i].propertyId, customStyle[i].propertyValue);
    }
};

Program::Program() : hashTable(1) {
    const int screenWidth = 1500;
    const int screenHeight = 786;
    InitWindow(screenWidth, screenHeight, "Data Structure Visualization");
	// GuiLoadStyle("../resources/styles/style_terminal.rgs");
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

