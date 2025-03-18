#include "SettingsManager.h"

bool soundEnabled = true;                       // Biến toàn cục để bật/tắt âm thanh, mặc định bật
bool isLightMode = true;                        // Biến toàn cục để chọn chế độ sáng/tối, mặc định sáng
float animationSpeed = 1.0f;                    // Biến toàn cục cho tốc độ animation, mặc định 1 giây/bước

void ToggleSound() { soundEnabled = !soundEnabled; } // Chuyển đổi trạng thái âm thanh
void ToggleColorMode() { isLightMode = !isLightMode; } // Chuyển đổi chế độ màu
void IncreaseAnimationSpeed() {                 // Tăng tốc độ animation
    if (animationSpeed > 0.2f) animationSpeed -= 0.2f; // Giảm thời gian mỗi bước nếu chưa đạt giới hạn
}
void DecreaseAnimationSpeed() {                 // Giảm tốc độ animation
    if (animationSpeed < 2.0f) animationSpeed += 0.2f; // Tăng thời gian mỗi bước nếu chưa đạt giới hạn
}