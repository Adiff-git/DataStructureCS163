#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include "raylib.h"
#include <vector>
#include <string>

struct HashNode {
    int key;
    HashNode* next;
};

class HashTable {
private:
    std::vector<HashNode*> table;
    int size;
    std::vector<std::string> steps;
    std::vector<std::vector<HashNode*>> stepStates;
    int hash(int key) { return key % size; }
    void ClearSteps();
    void SaveStep(const std::string& description);
    HashNode* CopyList(HashNode* source);
public:
    HashTable(int size);
    ~HashTable();
    void Initialize(int count);
    void Add(int key);
    void Delete(int key);
    void Update(int oldKey, int newKey);
    bool Search(int key);
    void Draw(Font font, int x, int y, int step);
    int GetTotalSteps() const { return steps.size(); }
};

#endif