#include "Utils.h"

Vector2 Vector2NormalizeCustom(Vector2 v) {
    float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length == 0.0f) return {0.0f, 0.0f};
    return {v.x / length, v.y / length};
}

Vector2 Vector2SubtractCustom(Vector2 v1, Vector2 v2) {
    return {v1.x - v2.x, v1.y - v2.y};
}

float EaseInOutQuad(float t) {
    return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}