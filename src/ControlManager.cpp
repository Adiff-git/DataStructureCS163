#include "ControlManager.h"
#include "Button.h"

ControlManager::ControlManager(LinkedList& linkedList, int screenWidth, int screenHeight, Font font)
    : list(linkedList),
      prevButton({static_cast<float>(screenWidth) / 2.0f - 150.0f, static_cast<float>(screenHeight) - 100.0f, 60.0f, 40.0f}),
      playButton({static_cast<float>(screenWidth) / 2.0f - 75.0f, static_cast<float>(screenHeight) - 100.0f, 60.0f, 40.0f}),
      nextButton({static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) - 100.0f, 60.0f, 40.0f}),
      skipButton({static_cast<float>(screenWidth) / 2.0f + 75.0f, static_cast<float>(screenHeight) - 100.0f, 60.0f, 40.0f}),
      speedSlider({static_cast<float>(screenWidth) / 2.0f - 150.0f, static_cast<float>(screenHeight) - 50.0f, 300.0f, 20.0f}),
      font(font),
      buttonClicked(false),
      buttonMessage("") {
}

void ControlManager::Update() {
    // Handle button interactions
    if (DrawButton("Prev", prevButton, font, buttonClicked, buttonMessage)) {
        list.PreviousStep();
    }
    if (DrawButton(list.IsPaused() ? "Play" : "Pause", playButton, font, buttonClicked, buttonMessage)) {
        list.SetPaused(!list.IsPaused());
    }
    if (DrawButton("Next", nextButton, font, buttonClicked, buttonMessage)) {
        list.NextStep();
    }
    if (DrawButton("Skip", skipButton, font, buttonClicked, buttonMessage)) {
        list.SkipToEnd();
    }

    // Update speed slider
    float speed = UpdateSlider(speedSlider, 0.1f, 2.0f, list.GetSpeed());
    list.SetSpeed(speed);
}

void ControlManager::Draw() {
    // Draw buttons
    DrawButton("Prev", prevButton, font, buttonClicked, buttonMessage);
    DrawButton(list.IsPaused() ? "Play" : "Pause", playButton, font, buttonClicked, buttonMessage);
    DrawButton("Next", nextButton, font, buttonClicked, buttonMessage);
    DrawButton("Skip", skipButton, font, buttonClicked, buttonMessage);

    // Draw speed slider
    DrawRectangleRec(speedSlider, LIGHTGRAY);
    float speed = list.GetSpeed();
    float sliderPos = (speed - 0.1f) / (2.0f - 0.1f) * speedSlider.width;
    DrawRectangle(speedSlider.x + sliderPos - 5.0f, speedSlider.y - 5.0f, 10.0f, 30.0f, DARKGRAY);
    DrawText(("Speed: " + std::to_string(speed).substr(0, 4)).c_str(), speedSlider.x, speedSlider.y - 30.0f, 20, BLACK);
}

bool ControlManager::IsPrevPressed() const {
    return IsButtonClicked(prevButton);
}

bool ControlManager::IsPlayPressed() const {
    return IsButtonClicked(playButton);
}

bool ControlManager::IsNextPressed() const {
    return IsButtonClicked(nextButton);
}

bool ControlManager::IsSkipPressed() const {
    return IsButtonClicked(skipButton);
}

float ControlManager::UpdateSpeedSlider() {
    return UpdateSlider(speedSlider, 0.1f, 2.0f, list.GetSpeed());
}