#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>

// Hàm kiểm tra click chuột
bool IsButtonClicked(Rectangle button);

// Hàm xử lý thanh kéo
float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue);