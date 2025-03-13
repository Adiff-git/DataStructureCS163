#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

// Function to draw a button and detect clicks
bool DrawButton(const char* text, int x, int y, Font font, bool& buttonClicked, const char*& buttonMessage);

#endif // BUTTON_H
