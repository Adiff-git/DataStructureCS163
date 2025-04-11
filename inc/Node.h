#pragma once
#include "raylib.h"
#include <vector>
#include <string>

struct Node {
    int value;
    Node* next;
    Vector2 position;
    bool isActive;
    float alpha;
    Vector2 targetPosition;
    float animationProgress;
    int animationType; // 0: none, 1: highlight, 2: drop, 3: fade, 4: realign, 5: arrow
    Node(int val) : value(val), next(nullptr), position({0, 0}), isActive(false), alpha(1.0f), 
                    targetPosition({0, 0}), animationProgress(0.0f), animationType(0) {}
};

// Define OperationState before ListState to avoid dependency issues
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
    int addIndex = 0; // For Add Index
};

struct ListState {
    std::vector<int> values;
    std::vector<Vector2> positions;
    std::vector<bool> isActive;
    std::vector<float> alphas;
    std::vector<Vector2> targetPositions;
    std::vector<float> animationProgresses;
    std::vector<int> animationTypes;
    int activeFunction;  // Explicitly specify type
    int activeLine;      // Explicitly specify type
    OperationState opState;
};

struct Operation {
    int type; // 1: Init, 2: Add, 3: Delete, 4: Search, 5: Update
    int value1;
    int value2;
    Operation(int t, int v1, int v2 = 0) : type(t), value1(v1), value2(v2) {}
};

struct OperationStep {
    int step;
    int activeLine;
    Node* current;
    Node* newNode;
    int deleteValue;
    int searchValue;
    bool searchResult;
    int updateOld;
    int updateNew;
    int initCount;
    int initIndex;
    int traversalIndex;
    float delayTimer;
    std::vector<int> initValues;
    std::vector<Node*> nodesState;
};