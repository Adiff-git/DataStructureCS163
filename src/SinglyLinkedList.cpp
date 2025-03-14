#include "SinglyLinkedList.h"
#include <cstdlib>

SinglyLinkedList::SinglyLinkedList() : head(nullptr) {}
SinglyLinkedList::~SinglyLinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    for (auto state : stepStates) {
        while (state) {
            Node* temp = state;
            state = state->next;
            delete temp;
        }
    }
}

void SinglyLinkedList::ClearSteps() {
    steps.clear();
    for (auto state : stepStates) {
        while (state) {
            Node* temp = state;
            state = state->next;
            delete temp;
        }
    }
    stepStates.clear();
}

Node* SinglyLinkedList::CopyList(Node* source) {
    if (!source) return nullptr;
    Node* newHead = new Node{source->data, nullptr};
    Node* current = newHead;
    source = source->next;
    while (source) {
        current->next = new Node{source->data, nullptr};
        current = current->next;
        source = source->next;
    }
    return newHead;
}

void SinglyLinkedList::SaveStep(const std::string& description) {
    steps.push_back(description);
    stepStates.push_back(CopyList(head));
}

void SinglyLinkedList::Initialize(int size) {
    ClearSteps();
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    SaveStep("Initial state (empty)");
    for (int i = 0; i < size; i++) {
        Add(rand() % 100);
    }
}

void SinglyLinkedList::Add(int value) {
    Node* newNode = new Node{value, nullptr};
    if (!head) {
        head = newNode;
        SaveStep("Added first node: " + std::to_string(value));
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
        SaveStep("Added node: " + std::to_string(value));
    }
}

void SinglyLinkedList::Delete(int value) {
    if (!head) return;
    if (head->data == value) {
        Node* temp = head;
        head = head->next;
        delete temp;
        SaveStep("Deleted first node: " + std::to_string(value));
        return;
    }
    Node* current = head;
    while (current->next && current->next->data != value) current = current->next;
    if (current->next) {
        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted node: " + std::to_string(value));
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
    if (updated) SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue));
}

bool SinglyLinkedList::Search(int value) {
    ClearSteps();
    Node* current = head;
    int index = 0;
    while (current) {
        SaveStep("Searching at position " + std::to_string(index) + ": " + std::to_string(current->data));
        if (current->data == value) {
            SaveStep("Found " + std::to_string(value));
            return true;
        }
        current = current->next;
        index++;
    }
    SaveStep("Not found: " + std::to_string(value));
    return false;
}

void SinglyLinkedList::Draw(Font font, int x, int y, int step) {
    if (step < 0 || step >= stepStates.size()) return;
    Node* current = stepStates[step];
    int offset = 0;
    while (current) {
        DrawRectangle(x + offset, y, 50, 50, LIGHTGRAY);
        DrawTextEx(font, TextFormat("%d", current->data), {static_cast<float>(x + offset + 10), static_cast<float>(y + 10)}, 30, 1, DARKBLUE);
        if (current->next) DrawLine(x + offset + 50, y + 25, x + offset + 100, y + 25, BLACK);
        current = current->next;
        offset += 100;
    }
    DrawTextEx(font, steps[step].c_str(), {static_cast<float>(x), static_cast<float>(y + 70)}, 20, 1, BLACK);
}