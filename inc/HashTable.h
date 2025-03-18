#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "DataStructureLogic.h"
#include "raylib.h"
#include "Screen.h" // Include Screen.h
#include <vector>
#include <string>

class HashTable : public DataStructureLogic {
private:
    struct HashNode {
        int key;
        HashNode* next;
        HashNode(int k) : key(k), next(nullptr) {}
    };
    std::vector<HashNode*> table;
    int size;
    int hash(int key) { return key % size; }
    void* CopyState() override;
    void Draw(Font font, void* state, int x, int y) override;
    void ClearStates() override;

public:
    HashTable(int size);
    ~HashTable() override;
    void Initialize(int param) override;
    void Add(int value) override;
    void Delete(int value) override;
    void Update(int oldValue, int newValue) override;
    bool Search(int value) override;
    void DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
};

#endif