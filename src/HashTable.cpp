#include "HashTable.h"
#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

char* ftc(float v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << v / 2;
    std::string str = ss.str();
    char* cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}

HashTable::HashTable(int size) : size(size), selectedNode(nullptr), selectedValue(0), selectedNodeArea({0, 0, 0, 0}) {
    table.resize(size, nullptr);
    saveCurrentTable();
	pathfile[0] = '\0';      // Initialize empty file path
    fileLoaded = false;      // No file loaded initially
    showUploadPrompt = false;//
}

HashTable::~HashTable() {
    for (auto& slot : table) {
        while (slot) {
            HashNode* temp = slot;
            slot = slot->next;
            delete temp;
        }
    }
    for (auto& slot: prevTable) {
        while (slot) {
            HashNode* temp = slot;
            slot = slot->next;
            delete temp;
        }
    }
    if (selectedNode) {
        selectedNode = nullptr;
    }
}

void HashTable::saveCurrentTable() {
    for (auto& slot: prevTable) {
        while (slot) {
            HashNode* temp = slot;
            slot = slot->next;
            delete temp;
        }
    }
    prevTable.clear();
    prevTable.resize(size, nullptr);
    for (int i = 0; i < size; i++) {
        HashNode* current = table[i];
        if (current) {
            prevTable[i] = new HashNode(current->key, current->value);
            HashNode* newCurrent = prevTable[i];
            while (current->next) {
                newCurrent->next = new HashNode(current->next->key, current->next->value);
                newCurrent = newCurrent->next;
                current = current->next;
            }
        }
    }
}

// Data structure area
void HashTable::drawPrevTable() {
    for (int i = 0; i < prevTable.size(); i++) {
        GuiButton(Rectangle{ 300, 20.0f + 30 * i, 60, 20 }, TextFormat("Key: %d", i));
        HashNode* current = prevTable[i];
        if (!current) {
            GuiDrawRectangle({ 360.0f, 20.0f + 30 * i + 10, 10, 2 }, 2, BLACK, BLACK);
            if (GuiButton(Rectangle{ 370.0f, 20.0f + 30 * i, 50, 20 }, "NULL")) {
                selectedNode = nullptr;
            }
        }
        int idx = 0;
        while (current) {
            GuiDrawRectangle({ 370.0f + 60 * idx - 10, 20.0f + 30 * i + 10, 10, 2 }, 2, BLACK, BLACK);
            if (GuiButton(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * i, 50, 20 }, TextFormat("%d | %d", current->key, current->value))) {
                selectedNode = current;
                selectedValue = current->value;
                selectedNodeArea = { 370.0f + 60 * idx, 20.0f + 30 * i, 50, 20 };
            }
            current = current->next;
            idx++;
        }
    }
}

void HashTable::drawTable() {
    for (int i = 0; i < size; i++) {
        GuiButton(Rectangle{ 300, 20.0f + 30 * i, 60, 20 }, TextFormat("Key: %d", i));
        HashNode* current = table[i];
        if (!current) {
            GuiDrawRectangle({ 360.0f, 20.0f + 30 * i + 10, 10, 2 }, 2, BLACK, BLACK);
            if (GuiButton(Rectangle{ 370.0f, 20.0f + 30 * i, 50, 20 }, "NULL")) {
                selectedNode = nullptr;
            }
        }
        int idx = 0;
        while (current) {
            GuiDrawRectangle({ 370.0f + 60 * idx - 10, 20.0f + 30 * i + 10, 10, 2 }, 2, BLACK, BLACK);
            if (GuiButton(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * i, 50, 20 }, TextFormat("%d | %d", current->key, current->value))) {
                selectedNode = current;
                selectedValue = current->value;
                selectedNodeArea = { 370.0f + 60 * idx, 20.0f + 30 * i, 50, 20 };
            }
            current = current->next;
            idx++;
        }
    }
}

// Operation
void HashTable::Init(int m, int n) {
    saveCurrentTable();
    initDescriptions.clear();
    initCodeIndex.clear();
    initPaths1.clear();
    initPaths2.clear();

    initDescriptions.push_back("Begin initialization");
    initCodeIndex.push_back(0);
    initPaths1.push_back({});
    initPaths2.push_back({});

    for (int i = 0; i < m; i++) {
        initDescriptions.push_back("Initializing slot " + std::to_string(i));
        initCodeIndex.push_back(1);
        initPaths1.push_back({});
        initPaths2.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * i, 60, 20 }, YELLOW) });
    }

    table.clear();
    table.resize(m, nullptr);
    size = m;

    for (int i = 0; i < n; i++) {
        int key = rand() % 100;
        int value = rand() % 100;
        Insert(key, value);
    }

    initDescriptions.push_back("Initialization complete");
    initCodeIndex.push_back(2);
    initPaths1.push_back({});
    initPaths2.push_back({});

    // Reset animation state variables
    curStep = 0;
    done = 0;
    delta = 0;
    doneAnimation = false;
    totalStep = initCodeIndex.size();
    operation_type = 4; // New operation type for initialization
}
void HashTable::Insert(int key, int value) {
    saveCurrentTable();
    insertDescriptions.clear();
    insertCodeIndex.clear();
    insertPaths1.clear();
    insertPaths2.clear();
    insertDescriptions.push_back("Begin inserting (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
    insertCodeIndex.push_back(0);
    insertPaths1.push_back({});
    insertPaths2.push_back({});
    // step 0: compute hash value
    int hashValue = hash(key);
    insertIndex = hashValue;
    insertDescriptions.push_back("Computing hash value: " + std::to_string(hashValue));
    insertCodeIndex.push_back(1);
    insertPaths1.push_back({});
    insertPaths2.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });

    //
    if (!table[hashValue]) {
        // table index is null
        insertDescriptions.push_back("Checking collision condition: No collision.");
        insertCodeIndex.push_back(2);
        insertPaths1.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
        insertPaths2.push_back({ 
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
         });

        // create node
        HashNode* newNode = new HashNode(key, value);
        insertDescriptions.push_back("Creating node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
        insertCodeIndex.push_back(3);
        insertPaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
        });
        insertPaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f + 60, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
        });
        // insert node
        table[hashValue] = newNode;
        insertDescriptions.push_back("Inserting node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
        insertCodeIndex.push_back(4);
        insertPaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, WHITE),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f + 60, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
        });
        insertPaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f - 40, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f - 40, 20.0f + 30 * hashValue - 5, 130, 2 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f + 60 + 30, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN)
        });
    } else {
        // table index is not null
        insertDescriptions.push_back("Checking collision condition: Collision detected.");
        insertCodeIndex.push_back(2);
        insertPaths1.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
        insertPaths2.push_back({ 
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, RED),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, RED)
         });
        // begin traversal
        HashNode* current = table[hashValue];
        insertDescriptions.push_back("Begin traversal at slot " + std::to_string(hashValue));
        insertCodeIndex.push_back(6);
        insertPaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
        });
        insertPaths2.push_back({ 
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, BLUE)
         });
       // check the node
int idx = 0;
while (current->next != NULL && current->key != key) {
    insertDescriptions.push_back("Checking condition: Key not found");
    insertCodeIndex.push_back(7);
    insertPaths1.push_back({
        std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
        // std::make_tuple(Rectangle{ 370.0f + 60 * idx - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
    });
    insertPaths2.push_back({ 
        std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, RED) // Highlight current node as RED (key mismatch)
    });
    current = current->next;
    idx++; // Increment idx to move to the next node position
    insertDescriptions.push_back("Move to the next node");
    insertCodeIndex.push_back(8);
    insertPaths1.push_back({
        std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
        // std::make_tuple(Rectangle{ 370.0f + 60 * idx - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, RED),
        std::make_tuple(Rectangle{ 370.0f + 60 * (idx - 1), 20.0f + 30 * hashValue, 50, 20 }, BLUE)
    });
    insertPaths2.push_back({ 
        std::make_tuple(Rectangle{ 370.0f + 60 * idx - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
        std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE) // Highlight next node as BLUE
    });
}
        if (current->key == key) {
            insertDescriptions.push_back("Checking condition: Found key" + std::to_string(key));
            insertCodeIndex.push_back(10);
            insertPaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            });
            insertPaths2.push_back({ 
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            current->value = value;
            insertDescriptions.push_back("Updating value to " + std::to_string(value));
            insertCodeIndex.push_back(11);
            insertPaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            });
            insertPaths2.push_back({ 
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
        } else {
            HashNode* newNode = new HashNode(key, value);
            insertDescriptions.push_back("Creating node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
            insertCodeIndex.push_back(13);
            insertPaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            insertPaths2.push_back({
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            //
            current->next = newNode;
            insertDescriptions.push_back("Inserting node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
            insertCodeIndex.push_back(14);
            insertPaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, GREEN),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            insertPaths2.push_back({
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, GREEN)
            });
        }
    }
    insertDescriptions.push_back("Done!");
    insertCodeIndex.push_back(17);
    insertPaths1.push_back({});
    insertPaths2.push_back({});
    
    curStep = 0;
    totalStep = insertCodeIndex.size();
    operation_type = 1;
}

void HashTable::Delete(int key) {
    saveCurrentTable();
    deleteDescriptions.clear();
    deleteCodeIndex.clear();
    deletePaths1.clear();
    deletePaths2.clear();
    deleteDescriptions.push_back("Begin deleting key: " + std::to_string(key));
    deleteCodeIndex.push_back(0);
    deletePaths1.push_back({});
    deletePaths2.push_back({});
    //
    int hashValue = hash(key);
    deleteDescriptions.push_back("Computing hash value: " + std::to_string(hashValue));
    deleteCodeIndex.push_back(1);
    deletePaths1.push_back({});
    deletePaths2.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
    //
    HashNode* current = table[hashValue];
    deleteDescriptions.push_back("Begin traversal at slot " + std::to_string(hashValue));
    deleteCodeIndex.push_back(2);
    deletePaths1.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
    deletePaths2.push_back({
        std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
        std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, BLUE)
    });
    //
    if (current == NULL) {
        deleteDescriptions.push_back("Slot NULL: " + std::to_string(hashValue));
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
        deletePaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, RED)
        });
        //
        deleteDescriptions.push_back("Done!");
        deleteCodeIndex.push_back(4);
        deletePaths1.push_back({});
        deletePaths2.push_back({});
        //
        curStep = 0;
        totalStep = deleteCodeIndex.size();
        operation_type = 2;
        return;
    }
    //
    if (current->key == key) {
        deleteDescriptions.push_back("Checking key: Found key " + std::to_string(key));
        deleteCodeIndex.push_back(6);
        deletePaths1.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
        deletePaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
        });
        //
        table[hashValue] = current->next;
        deleteDescriptions.push_back("Updating next pointer to next node");
        deleteCodeIndex.push_back(7);
        deletePaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
        });
        deletePaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, WHITE),
            std::make_tuple(Rectangle{ 370.0f - 40, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f - 40, 20.0f + 30 * hashValue - 5, 130, 2 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f + 60 + 30, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
        });
        //
        delete current;
        deleteDescriptions.push_back("Deleted key: " + std::to_string(key));
        deleteCodeIndex.push_back(8);
        deletePaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, WHITE),
            std::make_tuple(Rectangle{ 370.0f - 40, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f - 40, 20.0f + 30 * hashValue - 5, 130, 2 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f + 60 + 30, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
        });
        deletePaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, WHITE),
            std::make_tuple(Rectangle{ 370.0f + 60 - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, WHITE),
        });
        //
        deleteDescriptions.push_back("Done!");
        deleteCodeIndex.push_back(9);
        deletePaths1.push_back({});
        deletePaths2.push_back({});
        //
        curStep = 0;
        totalStep = deleteCodeIndex.size();
        operation_type = 2;
        return;
    }
    deleteDescriptions.push_back("Checking key: Key not found");
    deleteCodeIndex.push_back(6);
    deletePaths1.push_back({
        std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW)
    });
    deletePaths2.push_back({
        std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
        std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, RED)
    });
    //
    int idx = 0;
    while (current->next) {
        deleteDescriptions.push_back("Checking next node. Next node exists");
        deleteCodeIndex.push_back(11);
        deletePaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE)
        });
        deletePaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, GREEN),
            std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, GREEN)
        });
        //
        if (current->next->key == key) {
            deleteDescriptions.push_back("Checking next node: Found key " + std::to_string(key));
            deleteCodeIndex.push_back(12);
            deletePaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
            });
            deletePaths2.push_back({
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            //
            HashNode* temp = current->next;
            deleteDescriptions.push_back("Store next node to temp");
            deleteCodeIndex.push_back(13);
            deletePaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            deletePaths2.push_back({});
            //
            current->next = temp->next;
            deleteDescriptions.push_back("Updating next pointer to next next node");
            deleteCodeIndex.push_back(14);
            deletePaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            deletePaths2.push_back({
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, WHITE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 40, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 40, 20.0f + 30 * hashValue - 5, 130, 2 }, GREEN),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 2) + 30, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
            });
            //
            delete temp;
            deleteDescriptions.push_back("Deleting key: " + std::to_string(key));
            deleteCodeIndex.push_back(15);
            deletePaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, GREEN),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, WHITE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 40, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 40, 20.0f + 30 * hashValue - 5, 130, 2 }, GREEN),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 2) + 30, 20.0f + 30 * hashValue - 5, 2, 5 }, GREEN)
            });
            deletePaths2.push_back({
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, WHITE),
                std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 2) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, WHITE)
            });
            deleteDescriptions.push_back("Done!");
            deleteCodeIndex.push_back(16);
            deletePaths1.push_back({});
            deletePaths2.push_back({});
            //
            curStep = 0;
            totalStep = deleteCodeIndex.size();
            operation_type = 2;
            return;
        }
        deleteDescriptions.push_back("Checking next node: Key not found!");
        deleteCodeIndex.push_back(12);
        deletePaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE)
        });
        deletePaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, RED)
        });

        //
        current = current->next;
        deleteDescriptions.push_back("Moving to next node.");
        deleteCodeIndex.push_back(18);
        deletePaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f + 60 * (idx), 20.0f + 30 * hashValue, 50, 20 }, BLUE),
            std::make_tuple(Rectangle{ 370.0f + 60 * (idx) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE)
        });
        deletePaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f + 60 * (++idx), 20.0f + 30 * hashValue, 50, 20 }, BLUE)
        });
    }
    deleteDescriptions.push_back("Checking next node. Next node does not exist");
    deleteCodeIndex.push_back(11);
    deletePaths1.push_back({
        std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
        std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE)
    });
    deletePaths2.push_back({
        std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, RED),
        std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, RED)
    });
    //
    deleteDescriptions.push_back("Done!");
    deleteCodeIndex.push_back(20);
    deletePaths1.push_back({});
    deletePaths2.push_back({});
    //
    curStep = 0;
    totalStep = deleteCodeIndex.size();
    operation_type = 2;
    pause = false;
}

HashNode* HashTable::Search(int key) {
    saveCurrentTable();
    searchDescriptions.clear();
    searchCodeIndex.clear();
    searchPaths1.clear();
    searchPaths2.clear();
    searchDescriptions.push_back("Begin searching key: " + std::to_string(key));
    searchCodeIndex.push_back(0);
    searchPaths1.push_back({});
    searchPaths2.push_back({});
    //
    int hashValue = hash(key);
    searchDescriptions.push_back("Computing hash value: " + std::to_string(hashValue));
    searchCodeIndex.push_back(1);
    searchPaths1.push_back({});
    searchPaths2.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
    //
    HashNode* current = table[hashValue];
    searchDescriptions.push_back("Begin traversal at slot " + std::to_string(hashValue));
    searchCodeIndex.push_back(2);
    searchPaths1.push_back({ std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW) });
    searchPaths2.push_back({
        std::make_tuple(Rectangle{ 370.0f - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
        std::make_tuple(Rectangle{ 370.0f, 20.0f + 30 * hashValue, 50, 20 }, BLUE)
    });
    int idx = 0;
    while (current) {
        searchDescriptions.push_back("Checking node NULL: " + std::to_string(hashValue) + ". Not NULL.");
        searchCodeIndex.push_back(3);
        searchPaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW)
        });
        searchPaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
        });
        if (current->key == key) {
            searchDescriptions.push_back("Checking key: Found key " + std::to_string(key));
            searchCodeIndex.push_back(4);
            searchPaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW)
            });
            searchPaths2.push_back({
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            //
            searchDescriptions.push_back("Done: Found key " + std::to_string(key));
            searchCodeIndex.push_back(5);
            searchPaths1.push_back({
                std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
                std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, GREEN)
            });
            //
            curStep = 0;
            totalStep = searchCodeIndex.size();
            operation_type = 3;
            return current;
        }
        searchDescriptions.push_back("Checking key: Key not found!");
        searchCodeIndex.push_back(4);
        searchPaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW)
        });
        searchPaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, RED)
        });
        current = current->next;
        searchDescriptions.push_back("Moving to next node.");
        searchCodeIndex.push_back(7);
        searchPaths1.push_back({
            std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
            std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, BLUE)
        });
        searchPaths2.push_back({
            std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1) - 10, 20.0f + 30 * hashValue + 10, 10, 2 }, BLUE),
            std::make_tuple(Rectangle{ 370.0f + 60 * (idx + 1), 20.0f + 30 * hashValue, 50, 20 }, BLUE)
        });
        idx += 1;
    }
    searchDescriptions.push_back("Checking node NULL: " + std::to_string(hashValue) + ". NULL.");
    searchCodeIndex.push_back(3);
    searchPaths1.push_back({
        std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW)
    });
    searchPaths2.push_back({
        std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, RED)
    });
    searchDescriptions.push_back("Not found: " + std::to_string(hashValue) + ". Return NULL.");
    searchCodeIndex.push_back(9);
    searchPaths1.push_back({
        std::make_tuple(Rectangle{ 300, 20.0f + 30 * hashValue, 60, 20 }, YELLOW),
        std::make_tuple(Rectangle{ 370.0f + 60 * idx, 20.0f + 30 * hashValue, 50, 20 }, RED)
    });
    searchPaths2.push_back({});
    //
    curStep = 0;
    totalStep = searchCodeIndex.size();
    operation_type = 3;
    return NULL;
}
void HashTable::drawInitializeAnimation() {
    if (pause) {
        delta = 0;
        return;
    }

    // Ensure curStep does not exceed the size of the table
    if (curStep >= size) {
        doneAnimation = true;
        return;
    }

    // Draw the table incrementally up to the current step
    for (int i = 0; i <= curStep; i++) {
        GuiButton(Rectangle{ 300, 20.0f + 30 * i, 60, 20 }, TextFormat("Key: %d", i));
        HashNode* current = table[i];
        int idx = 0;

        // Draw nodes incrementally for the current slot
        while (current && (i < curStep || idx <= done)) {
            Rectangle nodeBounds = { 370.0f + 60 * idx, 20.0f + 30 * i, 50, 20 };

            // Highlight the current node being initialized in green
		        if (i == curStep && idx == done) {
            GuiButton(nodeBounds, TextFormat("%d | %d", current->key, current->value));
            GuiDrawRectangle(nodeBounds, 2, GREEN, GuiFade(GREEN, 0.5f));
        } else {
            GuiButton(nodeBounds, TextFormat("%d | %d", current->key, current->value));
            GuiDrawRectangle(nodeBounds, 2, GRAY, GuiFade(GRAY, 0.2f));
        }
            current = current->next;
            idx++;
        }
    }

    // Progress to the next node or slot after a delay
    delta += speed;
    if (delta >= 100) { // Adjust this value to control the delay
        HashNode* current = table[curStep];
        int idx = 0;
        while (current && idx < done) {
            current = current->next;
            idx++;
        }

        if (current && idx == done) {
            done++;
        } else {
            curStep++;
            done = 0;
        }
        delta = 0;
    }

    //// Draw progress bar
    //float progress = (float)(curStep) / size;
    ////GuiProgressBar(Rectangle{ 20, 480, 200, 20 }, NULL, NULL, &progress, 0.0f, 1.0f);
}
// Option Items
void HashTable::drawInitializeOptions() {
    static int mValue = 0;
    static int nValue = 0;
    static bool mInputEnabled = false;
    static bool nInputEnabled = false;

    GuiLabel(Rectangle{ 105, 200, 30, 20 }, "M: ");
    if (GuiSpinner(Rectangle{ 120, 200, 80, 20 }, NULL, &mValue, 0, 20, mInputEnabled)) {
        mInputEnabled = !mInputEnabled;
    }
    if (GuiButton(Rectangle{ 205, 200, 20, 20 }, "#193#")) {
        mValue = GetRandomValue(1, 10);
    }

    GuiLabel(Rectangle{ 105, 230, 30, 20 }, "N: ");
    if (GuiSpinner(Rectangle{ 120, 230, 80, 20 }, NULL, &nValue, 0, 100, nInputEnabled)) {
        nInputEnabled = !nInputEnabled;
    }
    if (GuiButton(Rectangle{ 205, 230, 20, 20 }, "#193#")) {
        nValue = GetRandomValue(0, 100);
    }

    if (GuiButton(Rectangle{ 130, 260, 60, 20 }, "Upload")) {
		showUploadPrompt = true;
        fileLoaded = false;
        TraceLog(LOG_INFO, "Upload button clicked, waiting for file drop");
	}

    if (GuiButton(Rectangle{ 205, 260, 20, 20 }, "OK")) {
        if (fileLoaded) {
            std::ifstream fin(pathfile);
            if (fin.is_open()) {
                int m, n;
                fin >> m >> n;
                m = std::min(m, 20);
                n = std::min(n, 100);

                table.clear();
                table.resize(m, nullptr);
                size = m;

                for (int i = 0; i < n && !fin.eof(); i++) {
                    int key, value;
                    if (fin >> key >> value) {
                        Insert(key, value);
                        TraceLog(LOG_INFO, "Inserted: key=%d, value=%d", key, value); // Optional debug
                    }
                }
                fin.close();
                saveCurrentTable();
                curStep = 0;
                done = 0;
                delta = 0;
                doneStep = true;
                doneAnimation = false;
                totalStep = initCodeIndex.size();
                operation_type = 4;
            } else {
                TraceLog(LOG_ERROR, "Failed to open file: %s", pathfile); // Optional debug
                fileLoaded = false;
                showUploadPrompt = true;
            }
        } else {
            Init(mValue, nValue);
        }
        showUploadPrompt = false;
    }

    if (showUploadPrompt) {
        const char* message = "Drag and drop an input file here";
        Vector2 textSize = MeasureTextEx(GetFontDefault(), message, 20, 1);
        Vector2 position = { (float)(GetScreenWidth() - textSize.x) / 2, (float)(GetScreenHeight() - textSize.y) / 2 };
        DrawText(message, (int)position.x, (int)position.y, 20, BLACK);
    }
}

void HashTable::drawInsertOptions() {
    static int key = 0;
    static int value = 0;
    static bool keyInputEnabled = false;
    static bool valueInputEnabled = false;

    GuiLabel(Rectangle{ 105, 230, 15, 20 }, "K: ");
    if (GuiSpinner(Rectangle{ 120, 230, 80, 20 }, NULL, &key, 0, 100, keyInputEnabled)) {
        keyInputEnabled = !keyInputEnabled;
    }
    if (GuiButton(Rectangle{ 205, 230, 20, 20 }, "#193#")) {
        key = GetRandomValue(0, 100);
    }

    GuiLabel(Rectangle{ 105, 260, 15, 20 }, "V: ");
    if (GuiSpinner(Rectangle{ 120, 260, 80, 20 }, NULL, &value, 0, 100, valueInputEnabled)) {
        valueInputEnabled = !valueInputEnabled;
    }
    if (GuiButton(Rectangle{ 205, 260, 20, 20 }, "#193#")) {
        value = GetRandomValue(0, 100);
    }

    if (GuiButton(Rectangle{ 230, 260, 20, 20 }, "OK")) {
        pause = true;
        Insert(key, value);
        done = 0;
        delta = 0;
        pause = false;
    }
}

void HashTable::drawDeleteOptions() {
    static int key = 0;
    static bool keyInputEnabled = false;

    GuiLabel(Rectangle{ 105, 260, 15, 20 }, "K: ");
    if (GuiSpinner(Rectangle{ 120, 260, 80, 20 }, NULL, &key, 0, 100, keyInputEnabled)) {
        keyInputEnabled = !keyInputEnabled;
    }
    if (GuiButton(Rectangle{ 205, 260, 20, 20 }, "#193#")) {
        key = GetRandomValue(0, 100);
    }

    if (GuiButton(Rectangle{ 230, 260, 20, 20 }, "OK")) {
        pause = true;
        Delete(key);
        done = 0;
        delta = 0;
        pause = false;
    }
}

void HashTable::drawSearchOptions() {
    static int key = 0;
    static bool keyInputEnabled = false;

    GuiLabel(Rectangle{ 105, 290, 15, 20 }, "K: ");
    if (GuiSpinner(Rectangle{ 120, 290, 80, 20 }, NULL, &key, 0, 100, keyInputEnabled)) {
        keyInputEnabled = !keyInputEnabled;
    }
    if (GuiButton(Rectangle{ 205, 290, 20, 20 }, "#193#")) {
        key = GetRandomValue(0, 100);
    }

    if (GuiButton(Rectangle{ 230, 290, 20, 20 }, "OK")) {
        pause = true;
        Search(key);
        done = 0;
        delta = 0;
        pause = false;
    }
}

// Operation Menu
void HashTable::drawOperationMenu() {
    static bool showInitializeOption = false;
    Rectangle initializeButtonPos = { 20, 200, 80, 20 };
    Rectangle initializeOptionPos = { 100, 200, 125, 90 };
    if (GuiButton(initializeButtonPos, "Initialize")) {
        showInitializeOption = !showInitializeOption;
    }
    if (showInitializeOption) {
        drawInitializeOptions();
    }
    static bool showInsertOption = false;
    Rectangle insertButtonPos = { 20, 230, 80, 20 };
    Rectangle insertOptionPos = { 100, 230, 150, 60 };
    if (GuiButton(insertButtonPos, "Insert")) {
        showInsertOption = !showInsertOption;
    }
    if (showInsertOption) {
        drawInsertOptions();
    }
    static bool showDeleteOption = false;
    Rectangle deleteButtonPos = { 20, 260, 80, 20 };
    Rectangle deleteOptionPos = { 100, 260, 150, 30 };
    if (GuiButton(deleteButtonPos, "Delete")) {
        showDeleteOption = !showDeleteOption;
    }
    if (showDeleteOption) {
        drawDeleteOptions();
    }
    static bool showSearchOption = false;
    Rectangle searchButtonPos = { 20, 290, 80, 20 };
    Rectangle searchOptionPos = { 100, 290, 150, 30 };
    if (GuiButton(searchButtonPos, "Search")) {
        showSearchOption = !showSearchOption;
    }
    if (showSearchOption) {
        drawSearchOptions();
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        if (!CheckCollisionPointRec(mousePoint, initializeOptionPos) && !CheckCollisionPointRec(mousePoint, initializeButtonPos)) {
            showInitializeOption = false;
        }
        if (!CheckCollisionPointRec(mousePoint, insertOptionPos) && !CheckCollisionPointRec(mousePoint, insertButtonPos)) {
            showInsertOption = false;
        }
        if (!CheckCollisionPointRec(mousePoint, deleteOptionPos) && !CheckCollisionPointRec(mousePoint, deleteButtonPos)) {
            showDeleteOption = false;
        }
        if (!CheckCollisionPointRec(mousePoint, searchOptionPos) && !CheckCollisionPointRec(mousePoint, searchButtonPos)) {
            showSearchOption = false;
        }
    }
}

// Animation
void HashTable::drawSearchAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    for (int i = 0; i < done; i++) {
        auto path = searchPaths2[curStep][i];
        if (std::get<0>(path).width > 0) {
            Color f = std::get<1>(path);
            Color g = (f.r == 255 && f.g == 255 && f.b == 255) ? f : GuiFade(f, 0.2f);
            GuiDrawRectangle(std::get<0>(path), 2, f, g);
        }
    }
    if (curStep < totalStep - 1 && curStep < searchPaths2.size() && done == searchPaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    std::tuple<Rectangle, Color> path = searchPaths2[curStep][done];
    Rectangle bounds = std::get<0>(path);
    Color color = std::get<1>(path);
    Color bg = (color.r == 255 && color.g == 255 && color.b == 255) ? color : GuiFade(color, 0.2f);
    delta += speed;
    GuiDrawRectangle({ bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height }, 2, color, bg);
    if (delta >= bounds.width) {
        done++;
        delta = 0;
    }
}

void HashTable::drawInsertAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    for (int i = 0; i < done; i++) {
        auto path = insertPaths2[curStep][i];
        if (std::get<0>(path).width > 0) {
            Color f = std::get<1>(path);
            Color g = (f.r == 255 && f.g == 255 && f.b == 255) ? f : GuiFade(f, 0.2f);
            GuiDrawRectangle(std::get<0>(path), 2, f, g);
        }
    }
    if (curStep < totalStep - 1 && curStep < insertPaths2.size() && done == insertPaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    std::tuple<Rectangle, Color> path = insertPaths2[curStep][done];
    Rectangle bounds = std::get<0>(path);
    Color color = std::get<1>(path);
    Color bg = (color.r == 255 && color.g == 255 && color.b == 255) ? color : GuiFade(color, 0.2f);
    delta += speed;
    GuiDrawRectangle({ bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height }, 2, color, bg);
    if (delta >= bounds.width) {
        done++;
        delta = 0;
    }
}

void HashTable::drawDeleteAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    for (int i = 0; i < done; i++) {
        auto path = deletePaths2[curStep][i];
        if (std::get<0>(path).width > 0) {
            Color f = std::get<1>(path);
            Color g = (f.r == 255 && f.g == 255 && f.b == 255) ? f : GuiFade(f, 0.2f);
            GuiDrawRectangle(std::get<0>(path), 2, f, g);
        }
    }
    if (curStep < totalStep - 1 && curStep < deletePaths2.size() && done == deletePaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    std::tuple<Rectangle, Color> path = deletePaths2[curStep][done];
    Rectangle bounds = std::get<0>(path);
    Color color = std::get<1>(path);
    Color bg = (color.r == 255 && color.g == 255 && color.b == 255) ? color : GuiFade(color, 0.2f);
    delta += speed;
    GuiDrawRectangle({ bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height }, 2, color, bg);
    if (delta >= bounds.width) {
        done++;
        delta = 0;
    }
}

// Amination Menu
void HashTable::drawAnimationMenu() {
    // Adjusted dimensions and positions for larger buttons and slider
    int buttonWidth = 40;  // Increased button width
    int buttonHeight = 40; // Increased button height
    int spacing = 10;      // Spacing between buttons
    int sliderWidth = 300; // Increased slider width
    int sliderHeight = 20; // Increased slider height

    // Draw the speed slider
    GuiSlider(Rectangle{40, 460, (float)sliderWidth, (float)sliderHeight}, "Speed", ftc(speed), &speed, 1, 30);

    // Draw the animation control buttons
    if (GuiButton(Rectangle{20, 400, (float)buttonWidth, (float)buttonHeight}, "Play")) {
        pause = false;
    }
    if (GuiButton(Rectangle{70, 400, (float)buttonWidth, (float)buttonHeight}, "Pause")) {
        pause = true;
    }
    if (GuiButton(Rectangle{120, 400, (float)buttonWidth, (float)buttonHeight}, "<")) {
        pause = true;
        if (curStep > 0) {
            curStep--;
        }
    }
    if (GuiButton(Rectangle{170, 400, (float)buttonWidth, (float)buttonHeight}, ">")) {
        pause = true;
        if (curStep < totalStep - 1) {
            curStep++;
        }
    }
    if (GuiButton(Rectangle{220, 400, (float)buttonWidth, (float)buttonHeight}, "<<")) {
        pause = true;
        curStep = 0;
    }
    if (GuiButton(Rectangle{270, 400, (float)buttonWidth, (float)buttonHeight}, ">>")) {
        pause = true;
        curStep = totalStep - 1;
    }
}

// Code description Menu
void HashTable::drawInsertDescription() {
    GuiLabel(Rectangle{ 980, 20, 150, 20 }, "Inserting");
    // for (int i = 0; i < insertCodeIndex.size(); i++) {
    //     GuiLabel(Rectangle{ 10.0f + 20 * i, 470, 20, 20 }, std::to_string(insertCodeIndex[i]).c_str());
    // }
    if (insertDescriptions.size() > curStep) {
        GuiLabel(Rectangle{ 980, 40, 300, 20 }, insertDescriptions[curStep].c_str());
    }
    int stt = insertCodeIndex.size() > curStep ? insertCodeIndex[curStep] : 0;
    for (int i = 0; i < insertCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        Rectangle bounds = { 1000, 60.0f + i * 20, 300, 20 };
        GuiDrawText(insertCodes[i].c_str(), GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), c);
        if (stt == i) {
            GuiLabel(Rectangle{ 980, 60.0f + i * 20, 20, 20 }, ">>");
        }
    }
    if (insertPaths1.size() > curStep) {
        for (auto& path : insertPaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                GuiDrawRectangle(std::get<0>(path), 2, std::get<1>(path), GuiFade(std::get<1>(path), 0.2f));
            }
        }
    }
    if (pause && insertPaths2.size() > curStep) {
        for (auto& path : insertPaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                Color f = std::get<1>(path);
                Color g = (f.r == 255 && f.g == 255 && f.b == 255) ? f : GuiFade(f, 0.2f);
                GuiDrawRectangle(std::get<0>(path), 2, f, g);
            }
        }
    }
}

void HashTable::drawDeleteDescription() {
    GuiLabel(Rectangle{ 980, 20, 150, 20 }, "Deleting");
    if (deleteDescriptions.size() > curStep) {
        GuiLabel(Rectangle{ 980, 40, 300, 20 }, deleteDescriptions[curStep].c_str());
    }
    int stt = deleteCodeIndex.size() > curStep ? deleteCodeIndex[curStep] : 0;
    for (int i = 0; i < deleteCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        Rectangle bounds = { 1000, 60.0f + i * 20, 300, 20 };
        GuiDrawText(deleteCodes[i].c_str(), GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), c);
        if (stt == i) {
            GuiLabel(Rectangle{ 980, 60.0f + i * 20, 20, 20 }, ">>");
        }
    }
    if (deletePaths1.size() > curStep) {
        for (auto& path : deletePaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                GuiDrawRectangle(std::get<0>(path), 2, std::get<1>(path), GuiFade(std::get<1>(path), 0.2f));
            }
        }
    }
    if (pause && deletePaths2.size() > curStep) {
        for (auto& path : deletePaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                Color f = std::get<1>(path);
                Color g = (f.r == 255 && f.g == 255 && f.b == 255) ? f : GuiFade(f, 0.2f);
                GuiDrawRectangle(std::get<0>(path), 2, f, g);
            }
        }
    }
}

void HashTable::drawSearchDescription() {
    GuiLabel(Rectangle{ 980, 20, 150, 20 }, "Searching");
    if (deleteDescriptions.size() > curStep) {
        GuiLabel(Rectangle{ 980, 40, 300, 20 }, deleteDescriptions[curStep].c_str());
    }
    int stt = searchCodeIndex.size() > curStep ? searchCodeIndex[curStep] : 0;
    for (int i = 0; i < searchCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        Rectangle bounds = { 1000, 60.0f + i * 20, 300, 20 };
        GuiDrawText(searchCodes[i].c_str(), GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), c);
        if (stt == i) {
            GuiLabel(Rectangle{ 980, 60.0f + i * 20, 20, 20 }, ">>");
        }
    }
    if (searchPaths1.size() > curStep) {
        for (auto& path : searchPaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                GuiDrawRectangle(std::get<0>(path), 2, std::get<1>(path), GuiFade(std::get<1>(path), 0.2f));
            }
        }
    }
    if (pause && searchPaths2.size() > curStep) {
        for (auto& path : searchPaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                Color f = std::get<1>(path);
                Color g = (f.r == 255 && f.g == 255 && f.b == 255) ? f : GuiFade(f, 0.2f);
                GuiDrawRectangle(std::get<0>(path), 2, f, g);
            }
        }
    }
}

void HashTable::drawInitializeDescription() {
    GuiLabel(Rectangle{ 980, 20, 150, 20 }, "Initializing");

    // Display the current description
    if (initDescriptions.size() > curStep) {
        GuiLabel(Rectangle{ 980, 40, 300, 20 }, initDescriptions[curStep].c_str());
    }

    // Highlight the current code line
    int stt = initCodeIndex.size() > curStep ? initCodeIndex[curStep] : 0;
    for (int i = 0; i < insertCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        Rectangle bounds = { 1000, 60.0f + i * 20, 300, 20 };
        GuiDrawText(insertCodes[i].c_str(), GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), c);
        if (stt == i) {
            GuiLabel(Rectangle{ 980, 60.0f + i * 20, 20, 20 }, ">>");
        }
    }

    // Draw the paths for the current step
    if (initPaths1.size() > curStep) {
        for (auto& path : initPaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                GuiDrawRectangle(std::get<0>(path), 2, std::get<1>(path), GuiFade(std::get<1>(path), 0.2f));
            }
        }
    }

    // Highlight the paths for the current step if paused
    if (pause && initPaths2.size() > curStep) {
        for (auto& path : initPaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                GuiDrawRectangle(std::get<0>(path), 2, std::get<1>(path), GuiFade(std::get<1>(path), 0.2f));
            }
        }
    }

    // Display progress percentage
    float progress = (float)(curStep + 1) / totalStep * 100;
    GuiLabel(Rectangle{ 980, 460, 150, 20 }, TextFormat("Progress: %.0f%%", progress));
}
// Node detail Menu
void HashTable::drawNodeDetailMenu() {
    if (selectedNode) {
        DrawRectangle(selectedNodeArea.x, selectedNodeArea.y, selectedNodeArea.width, selectedNodeArea.height, Fade(BLUE, 0.2f));
        
        bool vInputEnabled = false;

        GuiLabel(Rectangle{ 20, 10, 150, 20 }, TextFormat("Node detail:"));

        GuiLabel(Rectangle{ 20, 30, 60, 20 }, TextFormat("Key: %d", selectedNode->key));
        if (GuiSpinner(Rectangle{ 100, 30, 90, 20 }, "Value: ", &selectedValue, 0, 100, vInputEnabled)) {
            vInputEnabled = !vInputEnabled;
        };
        
        if (GuiButton(Rectangle{ 20, 60, 60, 20 }, "Delete")) {
            Delete(selectedNode->key);
            done = 0;
            delta = 0;
            selectedNode = nullptr;
        };
        if (GuiButton(Rectangle{ 90, 60, 60, 20 }, "Update")) {
            Insert(selectedNode->key, selectedValue);
            done = 0;
            delta = 0;
            selectedNode = nullptr;
        };
    }
}

// Screen
void HashTable::DrawScreen() {
    ClearBackground(WHITE);
    drawOperationMenu();
    drawAnimationMenu();

    // Only draw the table or previous table when the animation is complete
    if (doneAnimation && operation_type == 4 || curStep == totalStep - 1 && totalStep > 0 ) {
        drawTable();
    } else {
        // Do not draw the entire previous table during initialization animation
        if (operation_type != 4) {
            drawPrevTable();
        }
    }

    drawNodeDetailMenu();

    // Handle animations based on the operation type
    if (operation_type == 1) {
        drawInsertDescription();
        drawInsertAnimation();
    }
    if (operation_type == 2) {
        drawDeleteDescription();
        drawDeleteAnimation();
    }
    if (operation_type == 3) {
        drawSearchDescription();
        drawSearchAnimation();
    }
    if (operation_type == 4) { // Initialization animation
        drawInitializeDescription();
        drawInitializeAnimation();
    }
}