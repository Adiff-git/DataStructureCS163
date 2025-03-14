#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

extern bool soundEnabled;
extern bool isLightMode;
extern float animationSpeed; // Tốc độ animation (giây mỗi bước, nhỏ hơn = nhanh hơn)

void ToggleSound();
void ToggleColorMode();
void IncreaseAnimationSpeed();
void DecreaseAnimationSpeed();

#endif