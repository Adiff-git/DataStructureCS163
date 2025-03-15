#include "Animation.h"
#include "raylib.h"

Animation::Animation() : currentStep(0), isRunning(false), lastStepTime(0.0), animationSpeed(1.0) {}
Animation::~Animation() {
    ClearSteps();
}

void Animation::ClearSteps() {
    steps.clear();
    for (auto state : stepStates) {
        // Không giải phóng bộ nhớ ở đây, để lớp con xử lý
    }
    stepStates.clear();
    currentStep = 0;
    isRunning = false;
}

void Animation::SaveStep(const std::string& description, void* state) {
    steps.push_back(description);
    stepStates.push_back(state);
}

void Animation::ResetAnimation() {
    currentStep = 0;
    isRunning = false;
    lastStepTime = GetTime();
}

void Animation::NextStep() {
    if (currentStep < static_cast<int>(steps.size()) - 1) currentStep++;
    isRunning = false;
}

void Animation::PrevStep() {
    if (currentStep > 0) currentStep--;
    isRunning = false;
}

void Animation::UpdateAnimation() {
    if (isRunning && currentStep < static_cast<int>(steps.size()) - 1) {
        double currentTime = GetTime();
        if (currentTime - lastStepTime >= animationSpeed) {
            currentStep++;
            lastStepTime = currentTime;
        }
    }
}