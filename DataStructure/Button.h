#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

// Hàm vẽ button
void DrawButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char* buttonMessage);

#endif
