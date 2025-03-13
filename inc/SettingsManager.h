#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H
#include "raylib.h"
extern bool soundEnabled;  // Declared as external
extern bool isLightMode;   // Declared as external

void ToggleSound();
void ToggleColorMode();

#endif // SETTINGS_MANAGER_H
