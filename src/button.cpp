#include "../inc/Button.h"
#include "raylib.h"

Button::Button(float x, float y, float width, float height, const std::string& label, 
               Color bgColor, Color textColor, Font font, int fontSize)
    : rect{x, y, width, height}, label(label), bgColor(bgColor), textColor(textColor), 
      font(font), fontSize(fontSize), isPressed(false) {}

bool Button::isClicked() const {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return true;
    }
    return false;
}

void Button::draw() const {
    DrawRectangleRec(rect, bgColor);
    int textWidth = MeasureText(label.c_str(), fontSize);
    float textX = rect.x + (rect.width - textWidth) / 2.0f;
    float textY = rect.y + (rect.height - fontSize) / 2.0f;
    DrawTextEx(font, label.c_str(), {textX, textY}, fontSize, 1, textColor);
}

void Button::setPosition(float x, float y) {
    rect.x = x;
    rect.y = y;
}

void Button::setSize(float width, float height) {
    rect.width = width;
    rect.height = height;
}

void Button::setLabel(const std::string& newLabel) {
    label = newLabel;
}

void Button::setBgColor(Color newColor) {
    bgColor = newColor;
}