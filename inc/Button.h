#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

bool DrawButton(const char* text, Rectangle bounds, Font font, bool& buttonClicked, const char* buttonMessage);
bool IsButtonClicked(Rectangle bounds);

// Hàm mới để vẽ nút "Back" sử dụng hình ảnh
bool DrawBackButton(Texture2D backButtonTexture, Rectangle bounds, bool& buttonClicked);

#endif