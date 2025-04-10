#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"
#include <string>

class Button {
public:
    Rectangle rect;
    std::string label;
    Color bgColor;
    Color textColor;
    Font font;
    int fontSize;
    bool isPressed;

    Button(float x, float y, float width, float height, const std::string& label, 
           Color bgColor, Color textColor, Font font, int fontSize);

    bool isClicked() const;
    void draw() const;
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setLabel(const std::string& newLabel);
    void setBgColor(Color newColor);
};

#endif // BUTTON_H