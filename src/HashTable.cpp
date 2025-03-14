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
}

void HashTable::Initialize(int count) {
    for (int i = 0; i < count; i++) Add(rand() % 100);
}

void HashTable::Add(int key) {
    int index = hash(key);
    HashNode* newNode = new HashNode{key, nullptr};
    if (!table[index]) table[index] = newNode;
    else {
        HashNode* current = table[index];
        while (current->next) current = current->next;
        current->next = newNode;
    }
}

void HashTable::Delete(int key) {
    int index = hash(key);
    if (!table[index]) return;
    if (table[index]->key == key) {
        HashNode* temp = table[index];
        table[index] = temp->next;
        delete temp;
        return;
    }
    HashNode* current = table[index];
    while (current->next && current->next->key != key) current = current->next;
    if (current->next) {
        HashNode* temp = current->next;
        current->next = temp->next;
        delete temp;
    }
}

void HashTable::Update(int oldKey, int newKey) {
    int index = hash(oldKey);
    HashNode* current = table[index];
    while (current) {
        if (current->key == oldKey) current->key = newKey;
        current = current->next;
    }
}

bool HashTable::Search(int key) {
    int index = hash(key);
    HashNode* current = table[index];
    while (current) {
        if (current->key == key) return true;
        current = current->next;
    }
    return false;
}

void HashTable::Draw(Font font, int x, int y) {
    for (int i = 0; i < size; i++) {
        DrawTextEx(font, TextFormat("Slot %d:", i), {static_cast<float>(x), static_cast<float>(y + i * 60)}, 30, 1, DARKBLUE);
        HashNode* current = table[i];
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