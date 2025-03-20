#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include "Animation.h"
#include "SLLAnimation.h"
#include "Pseudocode.h"
#include "Screen.h"
#include <vector>
#include <string>

class SinglyLinkedList : public Animation {
public:
    struct Node {
        int data;
        Node* next;
        Node(int val) : data(val), next(nullptr) {}
    };

private:
    Node* head;
    SLLAnimation sllAnimation;
    Pseudocode pseudocode;
    std::vector<std::string> currentSteps;

protected:
    void* CopyState() override;
    void Draw(Font font, void* state, int x, int y) override;
    void ClearStates() override;

public:
    SinglyLinkedList();
    ~SinglyLinkedList();
    void Initialize(int param) override;
    void Add(int value) override;
    void Delete(int value) override;
    void Update(int oldValue, int newValue) override;
    bool Search(int value) override;
    void DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
    void UpdateAnimation() override;
    void DrawAnimation(Font font, int x, int y) override;
    void DrawDirect(Font font, int x, int y); // Thêm phương thức vẽ trực tiếp
};

#endif