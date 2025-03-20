#include "SettingsManager.h"
#include "DataStructureLogic.h"

bool isLightMode = true;
float animationSpeed = 1.0f;

void ToggleColorMode() { isLightMode = !isLightMode; }

void ToggleAnimationSpeed() {
    if (animationSpeed == 0.5f) animationSpeed = 1.0f;
    else if (animationSpeed == 1.0f) animationSpeed = 1.5f;
    else animationSpeed = 0.5f;
}

// Hàm phụ để áp dụng tốc độ cho DataStructureLogic nếu cần
void ApplyAnimationSpeed(DataStructureLogic* ds) {
    if (ds) ds->SetAnimationSpeed(animationSpeed);
}