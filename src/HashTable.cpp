#include "HashTable.h"

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
    for (auto& state : stepStates) {
        for (auto node : state) {
            while (node) {
                HashNode* temp = node;
                node = node->next;
                delete temp;
            }
        }
    }
}

void HashTable::ClearSteps() {
    steps.clear();
    for (auto& state : stepStates) {
        for (auto node : state) {
            while (node) {
                HashNode* temp = node;
                node = node->next;
                delete temp;
            }
        }
    }
    stepStates.clear();
}

HashNode* HashTable::CopyList(HashNode* source) {
    if (!source) return nullptr;
    HashNode* newHead = new HashNode{source->key, nullptr};
    HashNode* current = newHead;
    source = source->next;
    while (source) {
        current->next = new HashNode{source->key, nullptr};
        current = current->next;
        source = source->next;
    }
    return newHead;
}

void HashTable::SaveStep(const std::string& description) {
    steps.push_back(description);
    std::vector<HashNode*> state;
    for (int i = 0; i < size; i++) {
        state.push_back(CopyList(table[i]));
    }
    stepStates.push_back(state);
}

void HashTable::Initialize(int count) {
    ClearSteps();
    for (int i = 0; i < size; i++) {
        while (table[i]) {
            HashNode* temp = table[i];
            table[i] = temp->next;
            delete temp;
        }
    }
    SaveStep("Initial state (empty)");
    for (int i = 0; i < count; i++) Add(rand() % 100);
}

void HashTable::Add(int key) {
    int index = hash(key);
    HashNode* newNode = new HashNode{key, nullptr};
    if (!table[index]) {
        table[index] = newNode;
        SaveStep("Added " + std::to_string(key) + " to slot " + std::to_string(index));
    } else {
        HashNode* current = table[index];
        while (current->next) current = current->next;
        current->next = newNode;
        SaveStep("Added " + std::to_string(key) + " to slot " + std::to_string(index));
    }
}

void HashTable::Delete(int key) {
    int index = hash(key);
    if (!table[index]) return;
    if (table[index]->key == key) {
        HashNode* temp = table[index];
        table[index] = temp->next;
        delete temp;
        SaveStep("Deleted " + std::to_string(key) + " from slot " + std::to_string(index));
        return;
    }
    HashNode* current = table[index];
    while (current->next && current->next->key != key) current = current->next;
    if (current->next) {
        HashNode* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted " + std::to_string(key) + " from slot " + std::to_string(index));
    }
}

void HashTable::Update(int oldKey, int newKey) {
    int index = hash(oldKey);
    HashNode* current = table[index];
    bool updated = false;
    while (current) {
        if (current->key == oldKey) {
            current->key = newKey;
            updated = true;
        }
        current = current->next;
    }
    if (updated) SaveStep("Updated " + std::to_string(oldKey) + " to " + std::to_string(newKey));
}

bool HashTable::Search(int key) {
    ClearSteps();
    int index = hash(key);
    SaveStep("Searching in slot " + std::to_string(index));
    HashNode* current = table[index];
    while (current) {
        SaveStep("Checking value: " + std::to_string(current->key));
        if (current->key == key) {
            SaveStep("Found " + std::to_string(key));
            return true;
        }
        current = current->next;
    }
    SaveStep("Not found: " + std::to_string(key));
    return false;
}

void HashTable::Draw(Font font, int x, int y, int step) {
    if (step < 0 || step >= stepStates.size()) return;
    auto& state = stepStates[step];
    for (int i = 0; i < size; i++) {
        DrawTextEx(font, TextFormat("Slot %d:", i), {static_cast<float>(x), static_cast<float>(y + i * 60)}, 30, 1, DARKBLUE);
        HashNode* current = state[i];
        int offset = 100;
        while (current) {
            DrawRectangle(x + offset, y + i * 60, 50, 50, LIGHTGRAY);
            DrawTextEx(font, TextFormat("%d", current->key), {static_cast<float>(x + offset + 10), static_cast<float>(y + i * 60 + 10)}, 30, 1, DARKBLUE);
            if (current->next) DrawLine(x + offset + 50, y + i * 60 + 25, x + offset + 100, y + i * 60 + 25, BLACK);
            current = current->next;
            offset += 100;
        }
    }
    DrawTextEx(font, steps[step].c_str(), {static_cast<float>(x), static_cast<float>(y + size * 60 + 20)}, 20, 1, BLACK);
}