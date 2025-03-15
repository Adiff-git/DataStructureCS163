#include "HashTable.h"
#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>

HashTable::HashTable(int size) : size(size) {
    table.resize(size, nullptr);
}

HashTable::~HashTable() {
    for (int i = 0; i < size; i++) {
        HashNode* current = table[i];
        while (current) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
    ClearStates();
}

void* HashTable::CopyState() {
    std::vector<HashNode*>* state = new std::vector<HashNode*>;
    state->resize(size, nullptr);
    for (int i = 0; i < size; i++) {
        HashNode* newHead = nullptr;
        HashNode* current = nullptr;
        HashNode* source = table[i];
        while (source) {
            HashNode* newNode = new HashNode(source->key);
            if (!newHead) {
                newHead = newNode;
                current = newHead;
            } else {
                current->next = newNode;
                current = current->next;
            }
            source = source->next;
        }
        (*state)[i] = newHead;
    }
    return state;
}

void HashTable::Draw(Font font, void* state, int x, int y) {
    std::vector<HashNode*>* tableState = static_cast<std::vector<HashNode*>*>(state);
    for (int i = 0; i < size; i++) {
        DrawTextEx(font, TextFormat("Slot %d:", i), {static_cast<float>(x), static_cast<float>(y + i * 60)}, 30, 1, DARKBLUE);
        HashNode* current = (*tableState)[i];
        int offset = 100;
        while (current) {
            DrawRectangle(x + offset, y + i * 60, 50, 50, LIGHTGRAY);
            DrawTextEx(font, TextFormat("%d", current->key), {static_cast<float>(x + offset + 10), static_cast<float>(y + i * 60 + 10)}, 30, 1, DARKBLUE);
            if (current->next) DrawLine(x + offset + 50, y + i * 60 + 25, x + offset + 100, y + i * 60 + 25, BLACK);
            current = current->next;
            offset += 100;
        }
    }
}

void HashTable::ClearStates() {
    for (auto state : stepStates) {
        std::vector<HashNode*>* tableState = static_cast<std::vector<HashNode*>*>(state);
        for (auto node : *tableState) {
            while (node) {
                HashNode* temp = node;
                node = node->next;
                delete temp;
            }
        }
        delete tableState;
    }
}

void HashTable::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    for (int i = 0; i < size; i++) {
        while (table[i]) {
            HashNode* temp = table[i];
            table[i] = temp->next;
            delete temp;
        }
    }
    SaveStep("Initial state (empty)", CopyState());
    for (int i = 0; i < param; i++) Add(rand() % 100);
}

void HashTable::Add(int value) {
    int index = hash(value);
    HashNode* newNode = new HashNode(value);
    if (!table[index]) {
        table[index] = newNode;
    } else {
        HashNode* current = table[index];
        while (current->next) current = current->next;
        current->next = newNode;
    }
    SaveStep("Added " + std::to_string(value) + " to slot " + std::to_string(index), CopyState());
}

void HashTable::Delete(int value) {
    int index = hash(value);
    if (!table[index]) return;
    if (table[index]->key == value) {
        HashNode* temp = table[index];
        table[index] = temp->next;
        delete temp;
        SaveStep("Deleted " + std::to_string(value) + " from slot " + std::to_string(index), CopyState());
        return;
    }
    HashNode* current = table[index];
    while (current->next && current->next->key != value) current = current->next;
    if (current->next) {
        HashNode* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted " + std::to_string(value) + " from slot " + std::to_string(index), CopyState());
    }
}

void HashTable::Update(int oldValue, int newValue) {
    int index = hash(oldValue);
    HashNode* current = table[index];
    bool updated = false;
    while (current) {
        if (current->key == oldValue) {
            current->key = newValue;
            updated = true;
        }
        current = current->next;
    }
    if (updated) SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
}

bool HashTable::Search(int value) {
    ClearSteps();
    int index = hash(value);
    SaveStep("Searching in slot " + std::to_string(index), CopyState());
    HashNode* current = table[index];
    while (current) {
        SaveStep("Checking value: " + std::to_string(current->key), CopyState());
        if (current->key == value) {
            SaveStep("Found " + std::to_string(value), CopyState());
            return true;
        }
        current = current->next;
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    return false;
}