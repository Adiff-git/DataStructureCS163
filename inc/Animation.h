#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include <vector>
#include <string>

class Animation {
protected:
    std::vector<std::string> steps;
    std::vector<void*> stepStates;
    int currentStep;
    bool isRunning;
    double lastStepTime;
    double animationSpeed;

public:
    Animation();
    virtual ~Animation();
    void ClearSteps();
    void SaveStep(const std::string& description, void* state);
    int GetTotalSteps() const { return steps.size(); }
    void ResetAnimation();
    void StartAnimation() { isRunning = true; }
    void PauseAnimation() { isRunning = false; }
    void NextStep();
    void PrevStep();
    int GetCurrentStep() const { return currentStep; }
    bool IsRunning() const { return isRunning; }
    void SetAnimationSpeed(double speed) { animationSpeed = speed; }
    double GetAnimationSpeed() const { return animationSpeed; }
    virtual void DrawAnimation(Font font, int x, int y) = 0;
    virtual void UpdateAnimation() = 0;
    std::vector<std::string>& GetSteps() { return steps; } // Thêm getter để truy cập steps
};

#endif