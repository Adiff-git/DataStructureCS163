#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include "DataStructureLogic.h"
#include "raylib.h"
#include <string>

class SinglyLinkedList : public DataStructureLogic {
private:
    struct Node {
        int data;
        Node* next;
        Node(int d) : data(d), next(nullptr) {}
    };
    Node* head;
    void* CopyState() override;
    void Draw(Font font, void* state, int x, int y) override;
    void ClearStates() override;

public:
    SinglyLinkedList();
    ~SinglyLinkedList() override;
    void Initialize(int param) override;
    void Add(int value) override;
    void Delete(int value) override;
    void Update(int oldValue, int newValue) override;
    bool Search(int value) override;
};

#endif