#ifndef BUTTON_H
#define BUTTON_H
#include "raylib.h"

bool DrawButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char*& buttonMessage);
bool DrawSmallButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char*& buttonMessage);

#endif