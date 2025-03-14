#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include "raylib.h"
#include <vector>

struct HashNode {
    int key;
    HashNode* next;
};

class HashTable {
private:
    std::vector<HashNode*> table;
    int size;
    int hash(int key) { return key % size; }
public:
    HashTable(int size);
    ~HashTable();
    void Initialize(int count);
    void Add(int key);
    void Delete(int key);
    void Update(int oldKey, int newKey);
    bool Search(int key);
    void Draw(Font font, int x, int y);
};

#endif