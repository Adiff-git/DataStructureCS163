#include "Pseudocode.h"

void Pseudocode::SetSteps(const std::vector<std::string>& newSteps) {
    steps = newSteps;
}

void Pseudocode::Draw(Font font, int currentStep) {
    float x = 1400.0f; // Góc phải trên
    float y = 50.0f;   // Góc phải trên
    DrawRectangle(x - 10, y - 10, 300, steps.size() * 30 + 20, LIGHTGRAY);
    DrawRectangleLines(x - 10, y - 10, 300, steps.size() * 30 + 20, BLACK);
    for (size_t i = 0; i < steps.size(); i++) {
        Color color = (i == currentStep) ? RED : DARKGRAY;
        DrawTextEx(font, steps[i].c_str(), { x, y + i * 30 }, 20, 1, color);
    }
}