#include "SLLAnimation.h"
#include "SinglyLinkedList.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

float Lerp(float start, float end, float t) {
    return start + t * (end - start);
}

SLLAnimation::SLLAnimation() : Animation(), stepDuration(0.5f), curPos{300.0f, 200.0f} {
    currentStep = -1;
}

void SLLAnimation::ClearSteps() {
    steps.clear();
    stepStates.clear();
    currentNodes.clear();
    currentStep = -1;
    isRunning = false;
    std::cout << "ClearSteps: Steps and nodes cleared" << std::endl;
}

void SLLAnimation::SaveStep(const std::string& description, void* state) {
    std::vector<SLLNodeState> nodes = CopyNodes(state);
    AddStep(description, nodes);
}

void SLLAnimation::AddStep(const std::string& description, const std::vector<SLLNodeState>& nodes, int highlightIndex, bool isDeleting) {
    steps.push_back(description);
    std::vector<SLLNodeState> adjustedNodes = nodes;
    for (size_t i = 0; i < adjustedNodes.size(); i++) {
        if (i == highlightIndex) {
            adjustedNodes[i].highlight = true;
            if (isDeleting) {
                adjustedNodes[i].targetPos.y = 300.0f; // Trượt xuống khi xóa
            }
        }
    }
    stepStates.push_back(adjustedNodes);
    if (currentStep == -1) {
        currentStep = 0;
        currentNodes = adjustedNodes; // Đảm bảo currentNodes được cập nhật ngay từ bước đầu tiên
    }
    std::cout << "AddStep: " << description << ", Nodes: " << adjustedNodes.size() << ", Total steps: " << steps.size() << std::endl;
}

std::vector<SLLNodeState> SLLAnimation::CopyNodes(void* state) {
    std::vector<SLLNodeState> nodes;
    SinglyLinkedList::Node* current = static_cast<SinglyLinkedList::Node*>(state);
    int x = 300;
    while (current) {
        SLLNodeState node;
        node.data = current->data;
        node.position = { static_cast<float>(x), 200.0f };
        node.alpha = 1.0f;
        node.highlight = false;
        node.targetPos = node.position;
        nodes.push_back(node);
        current = current->next;
        x += 100;
    }
    std::cout << "CopyNodes: " << nodes.size() << " nodes copied" << std::endl;
    return nodes;
}

void SLLAnimation::UpdateAnimation() {
    // Cập nhật currentNodes ngay cả khi không chạy hoạt hình
    if (steps.empty()) {
        std::cout << "UpdateAnimation: Steps empty" << std::endl;
        return;
    }

    // Đảm bảo currentNodes luôn được cập nhật từ stepStates
    if (currentStep >= 0 && currentStep < stepStates.size()) {
        currentNodes = stepStates[currentStep];
        std::cout << "UpdateAnimation: Updated currentNodes with " << currentNodes.size() << " nodes at step " << currentStep << std::endl;
    }

    if (!isRunning) {
        std::cout << "UpdateAnimation: Animation not running" << std::endl;
        return;
    }

    double currentTime = GetTime();
    float elapsed = static_cast<float>(currentTime - lastStepTime);
    float t = elapsed / animationSpeed;

    if (steps[currentStep].find("Checking node") != std::string::npos && currentStep + 1 < steps.size()) {
        if (currentStep < currentNodes.size()) {
            Vector2 targetPos = currentNodes[currentStep].position;
            curPos.x = Lerp(curPos.x, targetPos.x, 0.1f);
            curPos.y = Lerp(curPos.y, targetPos.y, 0.1f);
        }
    }

    if (elapsed >= animationSpeed) {
        currentStep++;
        if (currentStep >= static_cast<int>(steps.size())) {
            currentStep = steps.size() - 1;
            isRunning = false;
            std::cout << "UpdateAnimation: Animation finished, currentStep = " << currentStep << std::endl;
        } else {
            currentNodes = stepStates[currentStep]; // Cập nhật currentNodes khi chuyển bước
            lastStepTime = currentTime;
            std::cout << "UpdateAnimation: Moved to step " << currentStep << ", Nodes: " << currentNodes.size() << std::endl;
        }
        return;
    }

    if (currentStep > 0) {
        auto& prevNodes = stepStates[currentStep - 1];
        auto& nextNodes = stepStates[currentStep];
        currentNodes.resize(std::max(prevNodes.size(), nextNodes.size()));
        for (size_t i = 0; i < currentNodes.size(); i++) {
            if (i < prevNodes.size() && i < nextNodes.size()) {
                currentNodes[i].position.x = Lerp(prevNodes[i].position.x, nextNodes[i].position.x, t);
                currentNodes[i].position.y = Lerp(prevNodes[i].position.y, nextNodes[i].position.y, t);
                currentNodes[i].alpha = Lerp(prevNodes[i].alpha, nextNodes[i].alpha, t);
                currentNodes[i].data = nextNodes[i].data;
                currentNodes[i].highlight = nextNodes[i].highlight;
            } else if (i < prevNodes.size()) {
                currentNodes[i].position.y = Lerp(prevNodes[i].position.y, prevNodes[i].targetPos.y, t);
                currentNodes[i].alpha = Lerp(prevNodes[i].alpha, 0.0f, t);
            } else {
                currentNodes[i].position.y = Lerp(-100.0f, nextNodes[i].targetPos.y, t);
                currentNodes[i].alpha = Lerp(0.0f, nextNodes[i].alpha, t);
                currentNodes[i].data = nextNodes[i].data;
                currentNodes[i].highlight = nextNodes[i].highlight;
            }
        }
    }

    if (steps[currentStep].find("Found") != std::string::npos) {
        for (auto& node : currentNodes) {
            if (node.highlight) {
                node.alpha = (sinf(GetTime() * 5.0f) + 1.0f) / 2.0f;
            }
        }
    }
}

void SLLAnimation::DrawAnimation(Font font, int x, int y) {
    std::cout << "DrawAnimation: Drawing " << currentNodes.size() << " nodes, currentStep = " << currentStep << std::endl;

    // Vẽ các node từ currentNodes
    for (size_t i = 0; i < currentNodes.size(); i++) {
        Color color = { 200, 200, 200, static_cast<unsigned char>(currentNodes[i].alpha * 255) };
        if (currentNodes[i].highlight) {
            DrawRectangle(currentNodes[i].position.x - 5, currentNodes[i].position.y - 5, 60, 60, YELLOW);
        }
        DrawRectangle(currentNodes[i].position.x, currentNodes[i].position.y, 50, 50, color);
        DrawTextEx(font, TextFormat("%d", currentNodes[i].data),
                   { currentNodes[i].position.x + 10, currentNodes[i].position.y + 10 }, 30, 1, DARKBLUE);
        if (i < currentNodes.size() - 1) {
            DrawLine(currentNodes[i].position.x + 50, currentNodes[i].position.y + 25,
                     currentNodes[i + 1].position.x, currentNodes[i + 1].position.y + 25, BLACK);
        }
    }

    // Vẽ con trỏ tìm kiếm
    if (currentStep >= 0 && steps[currentStep].find("Checking node") != std::string::npos) {
        DrawTextEx(font, "cur", { curPos.x - 10, curPos.y - 40 }, 20, 1, ORANGE);
        DrawLineV(curPos, { curPos.x, curPos.y - 20 }, ORANGE);
    }

    // Vẽ mô tả bước
    if (currentStep >= 0 && currentStep < steps.size()) {
        DrawTextEx(font, steps[currentStep].c_str(), { static_cast<float>(x), 150.0f }, 20, 1, DARKGRAY);
    }
}