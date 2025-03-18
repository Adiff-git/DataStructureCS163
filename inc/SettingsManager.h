#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

extern bool isLightMode;                            // Biến toàn cục: chế độ sáng/tối
extern float animationSpeed;                        // Biến toàn cục: tốc độ animation (giây/bước)

void ToggleColorMode();                             // Chuyển đổi chế độ màu
void ToggleAnimationSpeed();                        // Chuyển đổi tốc độ animation

#endif