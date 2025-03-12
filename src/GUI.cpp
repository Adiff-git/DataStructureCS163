#include "raylib.h"
#include "GUI.h"
#include "Button.h"

// Hàm vẽ giao diện chính
void DrawMenu(Font font, bool buttonClicked, const char* buttonMessage) {
    // Vẽ tiêu đề
    DrawText("Data Visualization Group 1", 250, 50, 40, DARKBLUE);

    // Vẽ các button
    DrawButton("Singly Linked List", 150, 150, font, buttonClicked, buttonMessage);
    DrawButton("Hash Table", 150, 220, font, buttonClicked, buttonMessage);
    DrawButton("AVL Tree", 150, 290, font, buttonClicked, buttonMessage);
    DrawButton("Graph", 150, 360, font, buttonClicked, buttonMessage);
}
