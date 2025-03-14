#include "SettingsManager.h"

bool soundEnabled = true;
bool isLightMode = true;

void ToggleSound() { soundEnabled = !soundEnabled; }
void ToggleColorMode() { isLightMode = !isLightMode; }