#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include "Screen.h" // Include Screen.h thay vì các file khác
#include <string>

class DataStructureLogic; // Forward declaration thay vì include các file

void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
void DrawAnimationControls(Font font, bool& buttonClicked, const char*& buttonMessage, DataStructureLogic* ds);

#endif