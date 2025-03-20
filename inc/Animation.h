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
    virtual void ClearSteps();
    virtual void SaveStep(const std::string& description, void* state);
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
    virtual void DrawAnimation(Font font, int x, int y);
    virtual void UpdateAnimation();
    std::vector<std::string>& GetSteps() { return steps; }

    // Thêm các phương thức ảo để override
    virtual void Initialize(int param) {}
    virtual void Add(int value) {}
    virtual void Delete(int value) {}
    virtual void Update(int oldValue, int newValue) {}
    virtual bool Search(int value) { return false; }
    virtual void* CopyState() { return nullptr; } // Thêm phương thức ảo
    virtual void Draw(Font font, void* state, int x, int y) {} // Thêm phương thức ảo
    virtual void ClearStates() {} // Thêm phương thức ảo
};

#endif