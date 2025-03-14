#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H
#include "raylib.h"

struct Node {
    int data;
    Node* next;
};

class SinglyLinkedList {
private:
    Node* head;
public:
    SinglyLinkedList();
    ~SinglyLinkedList();
    void Initialize(int size);
    void Add(int value);
    void Delete(int value);
    void Update(int oldValue, int newValue);
    bool Search(int value);
    void Draw(Font font, int x, int y);
};

#endif