#include "SinglyLinkedList.h"
#include "DataStructureLogic.h"
#include "raylib.h"
#include <string>

SinglyLinkedList::SinglyLinkedList() : head(nullptr) {}
SinglyLinkedList::~SinglyLinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    ClearStates();
}

void* SinglyLinkedList::CopyState() {
    if (!head) return nullptr;
    Node* newHead = new Node(head->data);
    Node* current = newHead;
    Node* source = head->next;
    while (source) {
        current->next = new Node(source->data);
        current = current->next;
        source = source->next;
    }
    return newHead;
}

void SinglyLinkedList::Draw(Font font, void* state, int x, int y) {
    Node* current = static_cast<Node*>(state);
    int offset = 0;
    while (current) {
        DrawRectangle(x + offset, y, 50, 50, LIGHTGRAY);
        DrawTextEx(font, TextFormat("%d", current->data), {static_cast<float>(x + offset + 10), static_cast<float>(y + 10)}, 30, 1, DARKBLUE);
        if (current->next) DrawLine(x + offset + 50, y + 25, x + offset + 100, y + 25, BLACK);
        current = current->next;
        offset += 100;
    }
}

void SinglyLinkedList::ClearStates() {
    for (auto state : stepStates) {
        Node* current = static_cast<Node*>(state);
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
}

void SinglyLinkedList::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    SaveStep("Initial state (empty)", CopyState());
    for (int i = 0; i < param; i++) Add(rand() % 100);
}

void SinglyLinkedList::Add(int value) {
    Node* newNode = new Node(value);
    if (!head) {
        head = newNode;
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
    SaveStep("Added node: " + std::to_string(value), CopyState());
}

void SinglyLinkedList::Delete(int value) {
    if (!head) return;
    if (head->data == value) {
        Node* temp = head;
        head = head->next;
        delete temp;
        SaveStep("Deleted node: " + std::to_string(value), CopyState());
        return;
    }
    Node* current = head;
    while (current->next && current->next->data != value) current = current->next;
    if (current->next) {
        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted node: " + std::to_string(value), CopyState());
    }
}

void SinglyLinkedList::Update(int oldValue, int newValue) {
    Node* current = head;
    bool updated = false;
    while (current) {
        if (current->data == oldValue) {
            current->data = newValue;
            updated = true;
        }
        current = current->next;
    }
    if (updated) SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
}

bool SinglyLinkedList::Search(int value) {
    ClearSteps();
    Node* current = head;
    int index = 0;
    while (current) {
        SaveStep("Searching at position " + std::to_string(index) + ": " + std::to_string(current->data), CopyState());
        if (current->data == value) {
            SaveStep("Found " + std::to_string(value), CopyState());
            return true;
        }
        current = current->next;
        index++;
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    return false;
}