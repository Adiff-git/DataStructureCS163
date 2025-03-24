#ifndef BUTTON_H                                     // Bảo vệ chống bao gồm nhiều lần
#define BUTTON_H

#include "raylib.h"

bool DrawButton(const char* text, float x, float y, Font font, bool& buttonClicked, const char*& buttonMessage); // Vẽ nút và xử lý sự kiện

#endif