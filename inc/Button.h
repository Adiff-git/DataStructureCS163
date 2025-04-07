#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

// Hàm vẽ nút với hiệu ứng màu sắc giống GuiButton của raygui
void DrawStyledButton(Rectangle bounds, const char* text, bool pressed);

// Hàm kiểm tra xem nút có được nhấn hay không
bool IsButtonClicked(Rectangle bounds);

#endif