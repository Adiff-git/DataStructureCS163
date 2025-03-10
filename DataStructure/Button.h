// Button.h
#ifndef BUTTON_H
#define BUTTON_H

struct Button {
    Rectangle rect;
    const char* text;
    bool isClicked;
};

void InitButton(Button& button, float x, float y, float width, float height, const char* text);
void DrawButton(Button& button);
bool CheckButtonClick(Button& button);

#endif


