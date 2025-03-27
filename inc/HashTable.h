#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "raylib.h"
#include <vector>
#include <string>
#include <tuple>

struct HashNode {
    int key;
    int value;
    HashNode* next;
    HashNode(int k, int v): key(k), value(v), next(nullptr) {}
};

class HashTable {
private:
    // hash table
    int hash(int key) { return size > 0 ? key % size : 0; }
    int size;
    std::vector<HashNode*> table;
    std::vector<HashNode*> prevTable;
    void drawTable();
    void drawPrevTable();
    void saveCurrentTable();
    
    // operation menu
    void drawOperationMenu();
    void drawInitializeOptions();
    void drawInsertOptions();
    void drawDeleteOptions();
    void drawSearchOptions();

    // node detail menu
    void drawNodeDetailMenu();
    HashNode* selectedNode;
    int selectedValue;
    Rectangle selectedNodeArea;
    
    // animation control menu
    void drawAnimationMenu();
    int curStep = 0;
    int totalStep = 0;
    float curX = 0;
    float delta = 0;
    float speed = 1;
    bool pause = false;
    int done = 0;
    bool doneStep = true;
    bool doneAnimation = false;
    
    int operation_type;
    // insert, operation type = 1
    int insertIndex;
    std::vector<std::string> insertCodes = {
        "void HashTable::Insert(int key, int value) {",
        "   int hashValue = hash(key);",
        "   if (!table[hashValue]) {",
        "       HashNode* newNode = new HashNode(key, value);",
        "       table[hashValue] = newNode;",
        "   } else {",
        "       HashNode* current = table[hashValue];",
        "       while (current->next != NULL && current -> key != key) {",
        "           current = current->next;",
        "       }",
        "       if (current->key == key) {",
        "           current->value = value;",
        "       } else {",
        "           HashNode* newNode = new HashNode(key, value);",
        "           current->next = newNode;",
        "       }",
        "   }",
        "}"
    };
    std::vector<std::string> insertDescriptions;
    std::vector<int> insertCodeIndex;
    std::vector<std::vector<std::tuple<Rectangle, Color>>> insertPaths1;
    std::vector<std::vector<std::tuple<Rectangle, Color>>> insertPaths2;
    void drawInsertDescription();
    void drawInsertAnimation();

    // delete, operation type = 2
    int deleteIndex;
    std::vector<std::string> deleteCodes = {
        " 1. void HashTable::Delete(int key) {",
        " 2.    int hashValue = hash(key);",
        " 3.    HashNode* current = table[hashValue];",
        " 4.    if (current == NULL) {",
        " 5.        return;",
        " 6.    }",
        " 7.    if (current->key == key) {",
        " 8.        table[hashValue] = current->next;",
        " 9.        delete current;",
        "10.        return;",
        "11.    }",
        "12.    while (current->next) {",
        "13.        if (current->next->key == key) {",
        "14.            HashNode* temp = current->next;",
        "15.            current->next = temp->next;",
        "16.            delete temp;",
        "17.            return;",
        "18.        }",
        "19.        current = current->next;",
        "20.    }",
        "21. }"
    };
    std::vector<std::string> deleteDescriptions;
    std::vector<int> deleteCodeIndex;
    std::vector<std::vector<std::tuple<Rectangle, Color>>> deletePaths1;
    std::vector<std::vector<std::tuple<Rectangle, Color>>> deletePaths2;
    void drawDeleteDescription();
    void drawDeleteAnimation();

    std::vector<std::string> searchCodes = {
        " 1. HashNode* HashTable::Search(int key) {",
        " 2.    int hashValue = hash(key);",
        " 3.    HashNode* current = table[hashValue];",
        " 4.    while (current) {",
        " 5.        if (current->key == key) {",
        " 6.            return current;",
        " 7.        }",
        " 8.        current = current->next;",
        " 9.    }",
        "10.    return NULL;",
        "11. }"
    };
    std::vector<std::string> searchDescriptions;
    std::vector<int> searchCodeIndex;
    std::vector<std::vector<std::tuple<Rectangle, Color>>> searchPaths1;
    std::vector<std::vector<std::tuple<Rectangle, Color>>> searchPaths2;
    void drawSearchDescription();
    void drawSearchAnimation();

public:
    HashTable(int size);
    ~HashTable();
    void Init(int m, int n);
    void Insert(int key, int value);
    void Delete(int key);
    HashNode* Search(int key);
    void DrawScreen();
};

#endif