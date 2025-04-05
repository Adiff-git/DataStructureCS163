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

    struct OperationState {
        int step = 0;
        int initCount = 0, initIndex = 0;
        Node* newNode = nullptr;
        Node* current = nullptr;
        int deleteValue = 0;
        int searchValue = 0;
        bool searchResult = false;
        int updateOld = 0, updateNew = 0;
        float delayTimer = 0.0f;
        std::vector<int> initValues;
        int traversalIndex = 0;
        int addIndex = 0; // Lưu index cho Add Index
    } opState;

    std::deque<ListState> past;
    std::deque<ListState> future_continuous;
    std::deque<ListState> operationHistory;

    void Clear();
    void UpdateInitialize();
    void SkipInitialize();
    void UpdateAdd(); // Gộp tất cả Add vào đây
    void UpdateDelete();
    void UpdateSearch();
    void UpdateUpdate();
    void SkipAdd(); // Gộp tất cả Skip Add vào đây
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