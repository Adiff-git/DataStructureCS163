#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

bool DrawButton(const char* text, float x, float y, Font font, bool& buttonClicked, const char*& buttonMessage);

#endif