#ifndef SLL_ANIMATION_H
#define SLL_ANIMATION_H

#include "Animation.h"
#include "raylib.h"
#include <vector>
#include <string>

struct SLLNodeState {
    int data;
    Vector2 position;
    float alpha;
    bool highlight;
    Vector2 targetPos;
};

class SLLAnimation : public Animation {
private:
    std::vector<std::vector<SLLNodeState>> stepStates;
    std::vector<SLLNodeState> currentNodes;
    float stepDuration;
    Vector2 curPos;

public:
    SLLAnimation();
    void ClearSteps() override;
    void SaveStep(const std::string& description, void* state) override;
    void AddStep(const std::string& description, const std::vector<SLLNodeState>& nodes, int highlightIndex = -1, bool isDeleting = false);
    void UpdateAnimation() override;
    void DrawAnimation(Font font, int x, int y) override;
    std::vector<SLLNodeState> CopyNodes(void* state);
};

#endif