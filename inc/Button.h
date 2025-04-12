#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

bool DrawButton(const char* text, Rectangle button, Font font, bool& buttonClicked, const char*& buttonMessage);
// Hàm kiểm tra xem nút có được nhấn hay không
bool IsButtonClicked(Rectangle bounds);

#endif