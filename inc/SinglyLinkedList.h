#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H
#include "raylib.h"
#include <vector>
#include <string>

struct Node {
    int data;
    Node* next;
};

class SinglyLinkedList {
private:
    Node* head;
    std::vector<std::string> steps;
    std::vector<Node*> stepStates; // Lưu trạng thái của danh sách tại mỗi bước
    void ClearSteps();
    void SaveStep(const std::string& description);
    Node* CopyList(Node* source);
public:
    SinglyLinkedList();
    ~SinglyLinkedList();
    void Initialize(int size);
    void Add(int value);
    void Delete(int value);
    void Update(int oldValue, int newValue);
    bool Search(int value);
    void Draw(Font font, int x, int y, int step);
    int GetTotalSteps() const { return steps.size(); }
};

#endif