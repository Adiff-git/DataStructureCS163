#pragma once

#include "raylib.h"
#include <cmath>

// Hàm kiểm tra click chuột
bool IsButtonClicked(Rectangle button);

// Hàm xử lý thanh kéo
float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue);

// Hàm vẽ nút
bool DrawButton(const char* text, Rectangle button, Font font, bool& buttonClicked, const char*& buttonMessage);