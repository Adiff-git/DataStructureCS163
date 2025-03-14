#include "LinkedList.h"
#include <cstdlib> // For rand

SinglyLinkedList::SinglyLinkedList() : head(nullptr) {}
SinglyLinkedList::~SinglyLinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

void SinglyLinkedList::Initialize(int size) {
    for (int i = 0; i < size; i++) Add(rand() % 100);
}

void SinglyLinkedList::Add(int value) {
    Node* newNode = new Node{value, nullptr};
    if (!head) head = newNode;
    else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

void SinglyLinkedList::Delete(int value) {
    if (!head) return;
    if (head->data == value) {
        Node* temp = head;
        head = head->next;
        delete temp;
        return;
    }
    Node* current = head;
    while (current->next && current->next->data != value) current = current->next;
    if (current->next) {
        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
    }
}

void SinglyLinkedList::Update(int oldValue, int newValue) {
    Node* current = head;
    while (current) {
        if (current->data == oldValue) current->data = newValue;
        current = current->next;
    }
}

bool SinglyLinkedList::Search(int value) {
    Node* current = head;
    while (current) {
        if (current->data == value) return true;
        current = current->next;
    }
    return false;
}

void SinglyLinkedList::Draw(Font font, int x, int y) {
    Node* current = head;
    int offset = 0;
    while (current) {
        DrawRectangle(x + offset, y, 50, 50, LIGHTGRAY);
        DrawTextEx(font, TextFormat("%d", current->data), {static_cast<float>(x + offset + 10), static_cast<float>(y + 10)}, 30, 1, DARKBLUE);
        if (current->next) DrawLine(x + offset + 50, y + 25, x + offset + 100, y + 25, BLACK);
        current = current->next;
        offset += 100;
    }
}