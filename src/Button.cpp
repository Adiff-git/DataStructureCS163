#include "Button.h"


// Hàm kiểm tra click chuột
bool IsButtonClicked(Rectangle button) {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Hàm xử lý thanh kéo
float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, slider) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float sliderWidth = slider.width;
        float mouseX = mousePos.x - slider.x;
        float t = mouseX / sliderWidth;
        return minValue + t * (maxValue - minValue);
    }
    return currentValue;
}