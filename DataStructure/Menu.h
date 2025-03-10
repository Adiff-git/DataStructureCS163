#ifndef MENU_H
#define MENU_H

#include "raylib.h"

// Hàm vẽ button (ô cấu trúc dữ liệu)
void DrawButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char* buttonMessage);

#endif
