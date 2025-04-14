// #ifndef BUTTON_H
// #define BUTTON_H

// #include "raylib.h"

// // Hàm vẽ nút với hiệu ứng màu sắc giống GuiButton của raygui
// void DrawStyledButton(Rectangle bounds, const char* text, bool pressed);

// // Hàm kiểm tra xem nút có được nhấn hay không
// bool IsButtonClicked(Rectangle bounds);

// #endif

#pragma once

#include "raylib.h"
#include <cmath>

// Hàm kiểm tra click chuột
bool IsButtonClicked(Rectangle button);

// Hàm xử lý thanh kéo
float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue);

// Hàm vẽ nút
bool DrawButton(const char* text, Rectangle button, Font font, bool& buttonClicked, const char*& buttonMessage);