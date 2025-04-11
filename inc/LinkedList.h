#pragma once
#include "raylib.h"
#include "Node.h"
#include <vector>
#include <string>
#include <queue>
#include <deque>

class LinkedList {
private:
    Node* head;
    const int NODE_SIZE = 60;
    const int SPACING = 100;
    bool isPaused;
    bool skipStep;
    float speed;
    int activeFunction;
    int activeLine;
    float scrollOffset;
    std::string notification;
    int screenWidth;
    int screenHeight;
    Node* foundNode;
    std::queue<Operation> operationQueue;

    OperationState opState;

    std::vector<ListState> operationStates; // Replaced past and future_continuous
    int currentStepIndex; // Track the current step
    std::deque<ListState> operationHistory;

    void Clear();
    void UpdateInitialize();
    void SkipInitialize();
    void UpdateAdd();
    void UpdateDelete();
    void UpdateSearch();
    void UpdateUpdate();
    void SkipAdd();
    void SkipDelete();
    void SkipSearch();
    void SkipUpdate();
    void DrawNode(Node* node, Vector2 adjustedPos);
    void DrawArrow(Vector2 start, Vector2 end);
    void RealignNodes();
    void RealignNodesImmediate();
    void SaveState();
    void LoadState(const ListState& state);
    void ResetOperation();
    float Lerp(float start, float end, float t);

public:
    LinkedList(int width, int height);
    std::string GetNotification() const;
    void SetNotification(const std::string& msg);
    void ClearList();
    void EnqueueOperation(int type, int value1, int value2 = 0);
    void EnqueueOperationWithValues(int type, const std::vector<int>& values);
    void ProcessOperations();
    void UpdateOperations();
    void UpdateAnimation();
    void Draw();
    void DrawCodeBlock();
    void LoadFromFile(const char* filename);
    void PreviousStep();
    void NextStep();
    void SkipToEnd();
    void SetPaused(bool paused);
    bool IsPaused() const;
    void SkipCurrentStep();
    void SetSpeed(float newSpeed);
    float GetSpeed() const;
    void UpdateScroll(float screenWidth, float screenHeight);
    float GetScrollOffset() const;
};