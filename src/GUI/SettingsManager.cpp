#include "SettingsManager.h"

bool soundEnabled = true;   // Defined here
bool isLightMode = true;    // Defined here

void ToggleSound() {
    soundEnabled = !soundEnabled;
}

void ToggleColorMode() {
    isLightMode = !isLightMode;
}
