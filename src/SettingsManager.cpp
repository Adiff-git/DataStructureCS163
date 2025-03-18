#include "SettingsManager.h"

bool isLightMode = true;                        // Biến toàn cục để chọn chế độ sáng/tối, mặc định sáng
float animationSpeed = 1.0f;                    // Biến toàn cục cho tốc độ animation, mặc định 1 giây/bước

void ToggleColorMode() { isLightMode = !isLightMode; } // Chuyển đổi chế độ màu
void ToggleAnimationSpeed() {                   // Chuyển đổi tốc độ animation giữa các giá trị cố định
    if (animationSpeed == 0.5f) animationSpeed = 1.0f;
    else if (animationSpeed == 1.0f) animationSpeed = 1.5f;
    else animationSpeed = 0.5f;
}