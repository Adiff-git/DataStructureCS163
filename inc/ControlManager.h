#pragma once
#include "raylib.h"
#include "LinkedList.h"

class ControlManager {
private:
    LinkedList& list; // Reference to the LinkedList for state control
    Rectangle prevButton;
    Rectangle playButton;
    Rectangle nextButton;
    Rectangle skipButton;
    Rectangle speedSlider;
    Font font;
    bool buttonClicked;
    const char* buttonMessage;

public:
    ControlManager(LinkedList& linkedList, int screenWidth, int screenHeight, Font font);
    void Update();
    void Draw();
    bool IsPrevPressed() const;
    bool IsPlayPressed() const;
    bool IsNextPressed() const;
    bool IsSkipPressed() const;
    float UpdateSpeedSlider();
};