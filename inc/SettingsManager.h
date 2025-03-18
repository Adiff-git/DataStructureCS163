#ifndef SETTINGS_MANAGER_H                          // Bảo vệ chống bao gồm nhiều lần
#define SETTINGS_MANAGER_H

extern bool soundEnabled;                           // Biến toàn cục: bật/tắt âm thanh
extern bool isLightMode;                            // Biến toàn cục: chế độ sáng/tối
extern float animationSpeed;                        // Biến toàn cục: tốc độ animation (giây/bước)

void ToggleSound();                                 // Chuyển đổi trạng thái âm thanh
void ToggleColorMode();                             // Chuyển đổi chế độ màu
void IncreaseAnimationSpeed();                      // Tăng tốc độ animation
void DecreaseAnimationSpeed();                      // Giảm tốc độ animation

#endif