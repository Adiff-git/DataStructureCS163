#include "SettingsManager.h"

bool soundEnabled = true;
bool isLightMode = true;
float animationSpeed = 1.0f; // Mặc định 1 giây mỗi bước

void ToggleSound() { soundEnabled = !soundEnabled; }
void ToggleColorMode() { isLightMode = !isLightMode; }
void IncreaseAnimationSpeed() { 
    if (animationSpeed > 0.2f) animationSpeed -= 0.2f; // Tăng tốc (giảm thời gian mỗi bước)
}
void DecreaseAnimationSpeed() { 
    if (animationSpeed < 2.0f) animationSpeed += 0.2f; // Giảm tốc (tăng thời gian mỗi bước)
}