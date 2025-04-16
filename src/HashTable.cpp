#include "HashTable.h"
#include "raylib.h"
#include <vector>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "Button.h"

static char* ftc(float v) { // Add 'static' here
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << v / 2;
    std::string str = "x" + ss.str();
    char* cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}

HashTable::HashTable(int size) : size(size), selectedNode(nullptr), selectedValue(0), selectedNodeArea({0, 0, 0, 0}) {
    table.resize(size, nullptr);
    saveCurrentTable();
    pathfile[0] = '\0';      // Initialize empty file path
    fileLoaded = false;      // No file loaded initially
    showUploadPrompt = false;
    operation_type = -1;
    // Tải hình ảnh nút "Back"
    backButtonTexture = LoadTexture("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\images\\BackButton.png");
}
HashTable::~HashTable() {
    for (auto& slot : table) {
        while (slot) {
            HashNode* temp = slot;
            slot = slot->next;
            delete temp;
        }
    }
    for (auto& slot : prevTable) {
        while (slot) {
            HashNode* temp = slot;
            slot = slot->next;
            delete temp;
        }
    }
    if (selectedNode) {
        selectedNode = nullptr;
    }
    // Giải phóng texture của nút "Back"
    UnloadTexture(backButtonTexture);
}

void HashTable::saveCurrentTable() {
    for (auto& slot : prevTable) {
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
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    for (int i = 0; i < size; i++) {
        // Node chỉ số (index node) - không bị ảnh hưởng bởi cuộn
        Rectangle indexRect = {tablePosX, tablePosY + (nodeHeight + nodeSpaceY) * i, indexWidth, nodeHeight};
        DrawButton(TextFormat("Index: %d", i), indexRect, GetFontDefault(), buttonClicked, buttonMessage);
        
        HashNode* current = prevTable[i];
        if (!current) {
            // Cạnh (edge[0])
            DrawRectangle(llPosX - 10 - scrollOffsetX, tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2, nodeSpaceX, 2, BLACK);
            // Node NULL
            Rectangle nullRect = {llPosX - scrollOffsetX, tablePosY + (nodeHeight + nodeSpaceY) * i, nodeWidth, nodeHeight};
            if (IsButtonClicked(nullRect)) {
                selectedNode = nullptr;
            }
            DrawButton("NULL", nullRect, GetFontDefault(), buttonClicked, buttonMessage);
        }
        int idx = 0;
        while (current) {
            // Cạnh (edge[0])
            DrawRectangle(llPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX - scrollOffsetX,
                          tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2,
                          nodeSpaceX, lineWidth, BLACK);
            // Node
            Rectangle nodeRect = {
                llPosX + (nodeWidth + nodeSpaceX) * idx - scrollOffsetX,
                tablePosY + (nodeHeight + nodeSpaceY) * i,
                nodeWidth,
                nodeHeight
            };
            if (IsButtonClicked(nodeRect)) {
                selectedNode = current;
                selectedValue = current->value;
                selectedNodeArea = nodeRect;
            }
            DrawButton(TextFormat("%d | %d", current->key, current->value), nodeRect, GetFontDefault(), buttonClicked, buttonMessage);
            current = current->next;
            idx++;
        }
    }
}

void HashTable::drawTable() {
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    // Tính toán độ lệch tối đa dựa trên hàng có nhiều node nhất
    maxScrollOffsetX = 0;
    for (int i = 0; i < size; i++) {
        int idx = 0;
        HashNode* current = table[i];
        while (current) {
            idx++;
            current = current->next;
        }
        float rowWidth = llPosX + (nodeWidth + nodeSpaceX) * idx - tablePosX;
        maxScrollOffsetX = std::max(maxScrollOffsetX, rowWidth - (1600 - tablePosX - 50)); // Chiều rộng khu vực hiển thị
    }
    maxScrollOffsetX = std::max(0.0f, maxScrollOffsetX); // Đảm bảo không âm

    for (int i = 0; i < size; i++) {
        // Node chỉ số (index node) - không bị ảnh hưởng bởi cuộn
        Rectangle indexRect = {tablePosX, tablePosY + (nodeHeight + nodeSpaceY) * i, indexWidth, nodeHeight};
        DrawButton(TextFormat("Index: %d", i), indexRect, GetFontDefault(), buttonClicked, buttonMessage);
        
        HashNode* current = table[i];
        if (!current) {
            // Cạnh (edge[0])
            DrawRectangle(llPosX - 10 - scrollOffsetX, tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2, nodeSpaceX, 2, BLACK);
            // Node NULL
            Rectangle nullRect = {llPosX - scrollOffsetX, tablePosY + (nodeHeight + nodeSpaceY) * i, nodeWidth, nodeHeight};
            if (IsButtonClicked(nullRect)) {
                selectedNode = nullptr;
            }
            DrawButton("NULL", nullRect, GetFontDefault(), buttonClicked, buttonMessage);
        }
        
        int idx = 0;
        while (current) {
            // Cạnh (edge[0])
            DrawRectangle(llPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX - scrollOffsetX,
                          tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2,
                          nodeSpaceX, lineWidth, BLACK);
            // Node
            Rectangle nodeRect = {
                llPosX + (nodeWidth + nodeSpaceX) * idx - scrollOffsetX,
                tablePosY + (nodeHeight + nodeSpaceY) * i,
                nodeWidth,
                nodeHeight
            };
            if (IsButtonClicked(nodeRect)) {
                selectedNode = current;
                selectedValue = current->value;
                selectedNodeArea = nodeRect;
            }
            DrawButton(TextFormat("%d | %d", current->key, current->value), nodeRect, GetFontDefault(), buttonClicked, buttonMessage);
            current = current->next;
            idx++;
        }
    }
}

Rectangle HashTable::calIndexArea(int hashValue) {
    return {
        tablePosX,
        tablePosY + (nodeHeight + nodeSpaceY) * hashValue,
        indexWidth,
        nodeHeight
    };
}

Rectangle HashTable::calNodeArea(int idx, int hashValue) {
    return {
        llPosX + (nodeWidth + nodeSpaceX) * idx,
        tablePosY + (nodeHeight + nodeSpaceY) * hashValue,
        nodeWidth,
        nodeHeight
    };
}

Rectangle HashTable::calEdgeArea(int idx, int hashValue) {
    return {
        llPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX,
        tablePosY + (nodeHeight + nodeSpaceY) * hashValue + nodeHeight / 2,
        nodeSpaceX,
        lineWidth
    };
}

Rectangle HashTable::calEdgeXArea(int idx, int hashValue, int i) {
    switch (i) {
    case 0:
        return {
            llPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX - nodeWidth / 2,
            tablePosY + (nodeHeight + nodeSpaceY) * hashValue - nodeSpaceY / 2,
            lineWidth,
            nodeSpaceY / 2
        };
    case 1:
        return {
            llPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX - nodeWidth / 2,
            tablePosY + (nodeHeight + nodeSpaceY) * hashValue - nodeSpaceY / 2,
            nodeWidth * 2 + nodeSpaceX * 2,
            lineWidth
        };
    case 2:
        return {
            llPosX + (nodeWidth + nodeSpaceX) * (idx + 2) - nodeSpaceX - nodeWidth / 2,
            tablePosY + (nodeHeight + nodeSpaceY) * hashValue - nodeSpaceY / 2,
            lineWidth,
            nodeSpaceY / 2
        };
    }
    return {0, 0, 0, 0}; // Default return to avoid warnings
}

// Operation
void HashTable::Init(int m, int n) {
    saveCurrentTable();
    table.clear();
    table.resize(m, nullptr);
    size = m;

    for (int i = 0; i < n; i++) {
        int key = rand() % 100;
        int value = rand() % 100;
        insert(key, value);
    }
    operation_type = -1;
    selectedNode = nullptr;
}

void HashTable::insert(int key, int value) {
    int hVal = hash(key);
    if (!table[hVal]) {
        HashNode* newNode = new HashNode(key, value);
        table[hVal] = newNode;
    } else {
        HashNode* curr = table[hVal];
        while (curr->next != NULL && curr->key != key) {
            curr = curr->next;
        }
        if (curr->key == key) {
            curr->value = value;
        } else {
            HashNode* newNode = new HashNode(key, value);
            curr->next = newNode;
        }
    }
}

void HashTable::Insert(int key, int value) {
    selectedNode = nullptr;
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
    insertPaths2.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });

    if (!table[hashValue]) {
        insertDescriptions.push_back("Checking collision condition: No collision.");
        insertCodeIndex.push_back(2);
        insertPaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        insertPaths2.push_back({ 
            std::make_tuple(calEdgeArea(0, hashValue), GREEN),
            std::make_tuple(calNodeArea(0, hashValue), GREEN)
         });

        HashNode* newNode = new HashNode(key, value);
        insertDescriptions.push_back("Creating node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
        insertCodeIndex.push_back(3);
        insertPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calEdgeArea(0, hashValue), GREEN),
            std::make_tuple(calNodeArea(0, hashValue), GREEN)
        });
        insertPaths2.push_back({
            std::make_tuple(calNodeArea(1, hashValue), GREEN)
        });
        table[hashValue] = newNode;
        insertDescriptions.push_back("Inserting node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
        insertCodeIndex.push_back(4);
        insertPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calEdgeArea(0, hashValue), WHITE),
            std::make_tuple(calNodeArea(0, hashValue), GREEN),
            std::make_tuple(calNodeArea(1, hashValue), GREEN)
        });
        insertPaths2.push_back({
            std::make_tuple(calEdgeXArea(0, hashValue, 0), GREEN),
            std::make_tuple(calEdgeXArea(0, hashValue, 1), GREEN),
            std::make_tuple(calEdgeXArea(0, hashValue, 2), GREEN)
        });
    } else {
        insertDescriptions.push_back("Checking collision condition: Collision detected.");
        insertCodeIndex.push_back(2);
        insertPaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        insertPaths2.push_back({ 
            std::make_tuple(calEdgeArea(0, hashValue), RED),
            std::make_tuple(calNodeArea(0, hashValue), RED)
         });
        HashNode* current = table[hashValue];
        insertDescriptions.push_back("Begin traversal at slot " + std::to_string(hashValue));
        insertCodeIndex.push_back(6);
        insertPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
        });
        insertPaths2.push_back({ 
            std::make_tuple(calEdgeArea(0, hashValue), BLUE),
            std::make_tuple(calNodeArea(0, hashValue), BLUE)
         });
        int idx = 0;
        while (current->next != NULL && current->key != key) {
            insertDescriptions.push_back("Checking condition: Key not found");
            insertCodeIndex.push_back(7);
            insertPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), BLUE)
            });
            insertPaths2.push_back({ 
                std::make_tuple(calEdgeArea(idx + 1, hashValue), RED),
                std::make_tuple(calNodeArea(idx + 1, hashValue), RED)
            });
            current = current->next;
            idx++;
            insertDescriptions.push_back("Move to the next node");
            insertCodeIndex.push_back(8);
            insertPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx - 1, hashValue), BLUE)
            });
            insertPaths2.push_back({ 
                std::make_tuple(calEdgeArea(idx, hashValue), BLUE),
                std::make_tuple(calNodeArea(idx, hashValue), BLUE)
            });
        }
        if (current->key == key) {
            insertDescriptions.push_back("Checking condition: Found key " + std::to_string(key));
            insertCodeIndex.push_back(10);
            insertPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
            });
            insertPaths2.push_back({ 
                std::make_tuple(calNodeArea(idx, hashValue), GREEN)
            });
            current->value = value;
            insertDescriptions.push_back("Updating value to " + std::to_string(value));
            insertCodeIndex.push_back(11);
            insertPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
            });
            insertPaths2.push_back({ 
                std::make_tuple(calNodeArea(idx, hashValue), GREEN)
            });
        } else {
            HashNode* newNode = new HashNode(key, value);
            insertDescriptions.push_back("Creating node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
            insertCodeIndex.push_back(13);
            insertPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), GREEN)
            });
            insertPaths2.push_back({
                std::make_tuple(calNodeArea(idx + 1, hashValue), GREEN)
            });
            current->next = newNode;
            insertDescriptions.push_back("Inserting node: (key:" + std::to_string(key) + ", value: " + std::to_string(value) +")");
            insertCodeIndex.push_back(14);
            insertPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), GREEN),
                std::make_tuple(calNodeArea(idx + 1, hashValue), GREEN)
            });
            insertPaths2.push_back({
                std::make_tuple(calEdgeArea(idx + 1, hashValue), GREEN)
            });
        }
    }
    insertDescriptions.push_back("Done!");
    insertCodeIndex.push_back(17);
    insertPaths1.push_back({});
    insertPaths2.push_back({});
    
    curStep = 0;
    totalStep = insertCodeIndex.size();
    doneAnimation = false;
    operation_type = 1;
}

void HashTable::Delete(int key) {
    selectedNode = nullptr;
    saveCurrentTable();
    deleteDescriptions.clear();
    deleteCodeIndex.clear();
    deletePaths1.clear();
    deletePaths2.clear();
    deleteDescriptions.push_back("Begin deleting key: " + std::to_string(key));
    deleteCodeIndex.push_back(0);
    deletePaths1.push_back({});
    deletePaths2.push_back({});
    int hashValue = hash(key);
    deleteDescriptions.push_back("Computing hash value: " + std::to_string(hashValue));
    deleteCodeIndex.push_back(1);
    deletePaths1.push_back({});
    deletePaths2.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
    HashNode* current = table[hashValue];
    deleteDescriptions.push_back("Begin traversal at slot " + std::to_string(hashValue));
    deleteCodeIndex.push_back(2);
    deletePaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
    deletePaths2.push_back({
        std::make_tuple(calEdgeArea(0, hashValue), BLUE),
        std::make_tuple(calNodeArea(0, hashValue), BLUE)
    });
    if (current == NULL) {
        deleteDescriptions.push_back("Slot NULL: " + std::to_string(hashValue));
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        deletePaths2.push_back({
            std::make_tuple(calNodeArea(0, hashValue), RED)
        });
        deleteDescriptions.push_back("Done!");
        deleteCodeIndex.push_back(4);
        deletePaths1.push_back({});
        deletePaths2.push_back({});
        curStep = 0;
        totalStep = deleteCodeIndex.size();
        operation_type = 2;
        doneAnimation = false;
        return;
    }
    if (current->key == key) {
        deleteDescriptions.push_back("Checking key: Found key " + std::to_string(key));
        deleteCodeIndex.push_back(6);
        deletePaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        deletePaths2.push_back({
            std::make_tuple(calEdgeArea(0, hashValue), BLUE),
            std::make_tuple(calNodeArea(0, hashValue), GREEN)
        });
        table[hashValue] = current->next;
        deleteDescriptions.push_back("Updating next pointer to next node");
        deleteCodeIndex.push_back(7);
        deletePaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calNodeArea(0, hashValue), BLUE),
            std::make_tuple(calNodeArea(1, hashValue), GREEN)
        });
        deletePaths2.push_back({
            std::make_tuple(calEdgeArea(0, hashValue), WHITE),
            std::make_tuple(calEdgeXArea(0, hashValue, 0), GREEN),
            std::make_tuple(calEdgeXArea(0, hashValue, 1), GREEN),
            std::make_tuple(calEdgeXArea(0, hashValue, 2), GREEN),
        });
        delete current;
        deleteDescriptions.push_back("Deleted key: " + std::to_string(key));
        deleteCodeIndex.push_back(8);
        deletePaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calEdgeArea(0, hashValue), BLUE),
            std::make_tuple(calNodeArea(1, hashValue), GREEN),
            std::make_tuple(calEdgeArea(0, hashValue), WHITE),
            std::make_tuple(calEdgeXArea(0, hashValue, 0), GREEN),
            std::make_tuple(calEdgeXArea(0, hashValue, 1), GREEN),
            std::make_tuple(calEdgeXArea(0, hashValue, 2), GREEN),
        });
        deletePaths2.push_back({
            std::make_tuple(calNodeArea(0, hashValue), WHITE),
            std::make_tuple(calEdgeArea(0, hashValue), WHITE),
        });
        deleteDescriptions.push_back("Done!");
        deleteCodeIndex.push_back(9);
        deletePaths1.push_back({});
        deletePaths2.push_back({});
        curStep = 0;
        totalStep = deleteCodeIndex.size();
        operation_type = 2;
        doneAnimation = false;
        return;
    }
    deleteDescriptions.push_back("Checking key: Key not found");
    deleteCodeIndex.push_back(6);
    deletePaths1.push_back({
        std::make_tuple(calIndexArea(hashValue), YELLOW)
    });
    deletePaths2.push_back({
        std::make_tuple(calNodeArea(0, hashValue), RED)
    });
    int idx = 0;
    while (current->next) {
        deleteDescriptions.push_back("Checking next node. Next node exists");
        deleteCodeIndex.push_back(11);
        deletePaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calNodeArea(idx, hashValue), BLUE)
        });
        deletePaths2.push_back({
            std::make_tuple(calEdgeArea(idx + 1, hashValue), GREEN),
            std::make_tuple(calNodeArea(idx + 1, hashValue), GREEN)
        });
        if (current->next->key == key) {
            deleteDescriptions.push_back("Checking next node: Found key " + std::to_string(key));
            deleteCodeIndex.push_back(12);
            deletePaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), BLUE),
                std::make_tuple(calEdgeArea(idx + 1, hashValue), BLUE),
            });
            deletePaths2.push_back({
                std::make_tuple(calNodeArea(idx + 1, hashValue), GREEN)
            });
            HashNode* temp = current->next;
            deleteDescriptions.push_back("Store next node to temp");
            deleteCodeIndex.push_back(13);
            deletePaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), BLUE),
                std::make_tuple(calEdgeArea(idx + 1, hashValue), BLUE),
                std::make_tuple(calNodeArea(idx + 1, hashValue), GREEN)
            });
            deletePaths2.push_back({});
            current->next = temp->next;
            deleteDescriptions.push_back("Updating next pointer to next next node");
            deleteCodeIndex.push_back(14);
            deletePaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), BLUE),
                std::make_tuple(calNodeArea(idx + 1, hashValue), GREEN)
            });
            deletePaths2.push_back({
                std::make_tuple(calEdgeArea(idx + 1, hashValue), WHITE),
                std::make_tuple(calEdgeXArea(idx + 1, hashValue, 0), GREEN),
                std::make_tuple(calEdgeXArea(idx + 1, hashValue, 1), GREEN),
                std::make_tuple(calEdgeXArea(idx + 1, hashValue, 2), GREEN),
            });
            delete temp;
            deleteDescriptions.push_back("Deleting key: " + std::to_string(key));
            deleteCodeIndex.push_back(15);
            deletePaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), BLUE),
                std::make_tuple(calNodeArea(idx + 1, hashValue), GREEN),
                std::make_tuple(calEdgeArea(idx + 1, hashValue), WHITE),
                std::make_tuple(calEdgeXArea(idx + 1, hashValue, 0), GREEN),
                std::make_tuple(calEdgeXArea(idx + 1, hashValue, 1), GREEN),
                std::make_tuple(calEdgeXArea(idx + 1, hashValue, 2), GREEN),
            });
            deletePaths2.push_back({
                std::make_tuple(calNodeArea(idx + 1, hashValue), WHITE),
                std::make_tuple(calEdgeArea(idx + 2, hashValue), WHITE)
            });
            deleteDescriptions.push_back("Done!");
            deleteCodeIndex.push_back(16);
            deletePaths1.push_back({});
            deletePaths2.push_back({});
            curStep = 0;
            totalStep = deleteCodeIndex.size();
            operation_type = 2;
            doneAnimation = false;
            return;
        }
        deleteDescriptions.push_back("Checking next node: Key not found!");
        deleteCodeIndex.push_back(12);
        deletePaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calNodeArea(idx, hashValue), BLUE)
        });
        deletePaths2.push_back({
            std::make_tuple(calNodeArea(idx + 1, hashValue), RED)
        });
        current = current->next;
        deleteDescriptions.push_back("Moving to next node.");
        deleteCodeIndex.push_back(18);
        deletePaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calNodeArea(idx, hashValue), BLUE),
            std::make_tuple(calEdgeArea(idx, hashValue), BLUE)
        });
        deletePaths2.push_back({
            std::make_tuple(calNodeArea(idx + 1, hashValue), BLUE)
        });
        idx += 1;
    }
    deleteDescriptions.push_back("Checking next node. Next node does not exist");
    deleteCodeIndex.push_back(11);
    deletePaths1.push_back({
        std::make_tuple(calIndexArea(hashValue), YELLOW),
        std::make_tuple(calNodeArea(idx, hashValue), BLUE)
    });
    deletePaths2.push_back({
        std::make_tuple(calEdgeArea(idx + 1, hashValue), RED),
        std::make_tuple(calNodeArea(idx + 1, hashValue), RED)
    });
    deleteDescriptions.push_back("Done!");
    deleteCodeIndex.push_back(20);
    deletePaths1.push_back({});
    deletePaths2.push_back({});
    curStep = 0;
    totalStep = deleteCodeIndex.size();
    operation_type = 2;
    doneAnimation = false;
    pause = false;
}

HashNode* HashTable::Search(int key) {
    selectedNode = nullptr;
    saveCurrentTable();
    searchDescriptions.clear();
    searchCodeIndex.clear();
    searchPaths1.clear();
    searchPaths2.clear();
    searchDescriptions.push_back("Begin searching key: " + std::to_string(key));
    searchCodeIndex.push_back(0);
    searchPaths1.push_back({});
    searchPaths2.push_back({});
    int hashValue = hash(key);
    searchDescriptions.push_back("Computing hash value: " + std::to_string(hashValue));
    searchCodeIndex.push_back(1);
    searchPaths1.push_back({});
    searchPaths2.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
    HashNode* current = table[hashValue];
    searchDescriptions.push_back("Begin traversal at slot " + std::to_string(hashValue));
    searchCodeIndex.push_back(2);
    searchPaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
    searchPaths2.push_back({
        std::make_tuple(calEdgeArea(0, hashValue), BLUE),
        std::make_tuple(calNodeArea(0, hashValue), BLUE)
    });
    int idx = 0;
    while (current) {
        searchDescriptions.push_back("Checking node NULL: " + std::to_string(hashValue) + ". Not NULL.");
        searchCodeIndex.push_back(3);
        searchPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW)
        });
        searchPaths2.push_back({
            std::make_tuple(calNodeArea(idx, hashValue), GREEN)
        });
        if (current->key == key) {
            searchDescriptions.push_back("Checking key: Found key " + std::to_string(key));
            searchCodeIndex.push_back(4);
            searchPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW)
            });
            searchPaths2.push_back({
                std::make_tuple(calNodeArea(idx, hashValue), GREEN)
            });
            searchDescriptions.push_back("Done: Found key " + std::to_string(key));
            searchCodeIndex.push_back(5);
            searchPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), GREEN)
            });
            searchPaths2.push_back({});
            curStep = 0;
            totalStep = searchCodeIndex.size();
            operation_type = 3;
            doneAnimation = false;
            return current;
        }
        searchDescriptions.push_back("Checking key: Key not found!");
        searchCodeIndex.push_back(4);
        searchPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW)
        });
        searchPaths2.push_back({
            std::make_tuple(calNodeArea(idx, hashValue), RED)
        });
        current = current->next;
        searchDescriptions.push_back("Moving to next node.");
        searchCodeIndex.push_back(7);
        searchPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calNodeArea(idx, hashValue), BLUE)
        });
        searchPaths2.push_back({
            std::make_tuple(calEdgeArea(idx + 1, hashValue), BLUE),
            std::make_tuple(calNodeArea(idx + 1, hashValue), BLUE)
        });
        idx += 1;
    }
    searchDescriptions.push_back("Checking node NULL: " + std::to_string(hashValue) + ". NULL.");
    searchCodeIndex.push_back(3);
    searchPaths1.push_back({
        std::make_tuple(calIndexArea(hashValue), YELLOW)
    });
    searchPaths2.push_back({
        std::make_tuple(calNodeArea(idx, hashValue), RED)
    });
    searchDescriptions.push_back("Not found: " + std::to_string(hashValue) + ". Return NULL.");
    searchCodeIndex.push_back(9);
    searchPaths1.push_back({
        std::make_tuple(calIndexArea(hashValue), YELLOW),
        std::make_tuple(calNodeArea(idx, hashValue), RED)
    });
    searchPaths2.push_back({});
    curStep = 0;
    totalStep = searchCodeIndex.size();
    operation_type = 3;
    doneAnimation = false;
    return NULL;
}

void HashTable::drawInitializeAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep >= size) {
        doneAnimation = true;
        return;
    }
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    for (int i = 0; i <= curStep; i++) {
        Rectangle keyRect = {tablePosX, tablePosY + 30 * i, 60, 20};
        DrawButton(TextFormat("Key: %d", i), keyRect, GetFontDefault(), buttonClicked, buttonMessage);
        
        HashNode* current = table[i];
        int idx = 0;
        while (current && (i < curStep || idx <= done)) {
            Rectangle nodeBounds = {llPosX + 60 * idx, tablePosY + 30 * i, 50, 20};
            if (i == curStep && idx == done) {
                DrawButton(TextFormat("%d | %d", current->key, current->value), nodeBounds, GetFontDefault(), buttonClicked, buttonMessage);
                DrawRectangleLinesEx(nodeBounds, 2, GREEN);
            } else {
                DrawButton(TextFormat("%d | %d", current->key, current->value), nodeBounds, GetFontDefault(), buttonClicked, buttonMessage);
                DrawRectangleLinesEx(nodeBounds, 2, GRAY);
            }
            current = current->next;
            idx++;
        }
    }
    delta += speed;
    if (delta >= 100) {
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
}

// Option Items
void HashTable::drawInitializeOptions() {
    static int mValue = 0;
    static int nValue = 0;
    static char mText[4] = "0";
    static char nText[4] = "0";
    static bool mInputEnabled = false;
    static bool nInputEnabled = false;
    static int mMax = 14;
    static int nMax = 99;
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    DrawText("M: ", 20, 610, 20, BLACK);
    Rectangle mRect = {50, 610, 100, 25};
    DrawRectangleRec(mRect, WHITE);
    DrawRectangleLinesEx(mRect, 1, BLACK);
    if (mInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) mInputEnabled = false;
        int key = GetKeyPressed();
        if (key >= 48 && key <= 57 && strlen(mText) < 3) {
            int len = strlen(mText);
            mText[len] = (char)key;
            mText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(mText) > 0) {
            mText[strlen(mText) - 1] = '\0';
        }
        if (atoi(mText) > mMax) {
            mText[0] = '1';
            mText[1] = '4';
            mText[2] = '\0';
        }
    }
    mValue = atoi(mText);
    DrawText(mText, mRect.x + 5, mRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), mRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        mInputEnabled = true;
        nInputEnabled = false;
    }

    Rectangle mRandomRect = {160, 610, 25, 25};
    if (IsButtonClicked(mRandomRect)) {
        mValue = GetRandomValue(1, 14);
        sprintf(mText, "%d", mValue);
    }
    DrawButton("?", mRandomRect, GetFontDefault(), buttonClicked, buttonMessage);

    DrawText("N: ", 200, 610, 20, BLACK);
    Rectangle nRect = {230, 610, 100, 25};
    DrawRectangleRec(nRect, WHITE);
    DrawRectangleLinesEx(nRect, 1, BLACK);
    if (nInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) nInputEnabled = false;
        int key = GetKeyPressed();
        if (key >= 48 && key <= 57 && strlen(nText) < 3) {
            int len = strlen(nText);
            nText[len] = (char)key;
            nText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(nText) > 0) {
            nText[strlen(nText) - 1] = '\0';
        }
        if (atoi(nText) > nMax) {
            nText[0] = '9';
            nText[1] = '9';
            nText[2] = '\0';
        }
    }
    nValue = atoi(nText);
    DrawText(nText, nRect.x + 5, nRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), nRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        nInputEnabled = true;
        mInputEnabled = false;
    }

    Rectangle nRandomRect = {340, 610, 25, 25};
    if (IsButtonClicked(nRandomRect)) {
        nValue = GetRandomValue(0, 99);
        sprintf(nText, "%d", nValue);
    }
    DrawButton("?", nRandomRect, GetFontDefault(), buttonClicked, buttonMessage);

    Rectangle uploadRect = {30, 650, 80, 30};
    if (IsButtonClicked(uploadRect)) {
        showUploadPrompt = true;
        fileLoaded = false;
        TraceLog(LOG_INFO, "Upload button clicked, waiting for file drop");
    }
    DrawButton("Upload", uploadRect, GetFontDefault(), buttonClicked, buttonMessage);

    Rectangle okRect = {350, 650, 40, 30};
    if (IsButtonClicked(okRect)) {
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
                fileLoaded = false;
                showUploadPrompt = true;
            }
        } else {
            Init(mValue, nValue);
        }
        showUploadPrompt = false;
    }
    DrawButton("OK", okRect, GetFontDefault(), buttonClicked, buttonMessage);

    if (showUploadPrompt) {
        DrawText("Drop a file here", 120, 655, 20, GRAY);
    }
}

void HashTable::drawInsertOptions() {
    static int key = 0;
    static int value = 0;
    static int keyMax = 99;
    static int valueMax = 99;
    static char keyText[4] = "0";
    static char valueText[4] = "0";
    static bool keyInputEnabled = false;
    static bool valueInputEnabled = false;
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    DrawText("K: ", 20, 610, 20, BLACK);
    Rectangle kRect = {50, 610, 100, 25};
    DrawRectangleRec(kRect, WHITE);
    DrawRectangleLinesEx(kRect, 1, BLACK);
    if (keyInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) keyInputEnabled = false;
        int k = GetKeyPressed();
        if (k >= 48 && k <= 57 && strlen(keyText) < 3) {
            int len = strlen(keyText);
            keyText[len] = (char)k;
            keyText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(keyText) > 0) {
            keyText[strlen(keyText) - 1] = '\0';
        }
        if (atoi(keyText) > keyMax) {
            keyText[0] = '9';
            keyText[1] = '9';
            keyText[2] = '\0';
        }
    }
    key = atoi(keyText);
    DrawText(keyText, kRect.x + 5, kRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), kRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        keyInputEnabled = true;
        valueInputEnabled = false;
    }

    Rectangle kRandomRect = {160, 610, 25, 25};
    if (IsButtonClicked(kRandomRect)) {
        key = GetRandomValue(0, 100);
        sprintf(keyText, "%d", key);
    }
    DrawButton("?", kRandomRect, GetFontDefault(), buttonClicked, buttonMessage);

    DrawText("V: ", 200, 610, 20, BLACK);
    Rectangle vRect = {230, 610, 100, 25};
    DrawRectangleRec(vRect, WHITE);
    DrawRectangleLinesEx(vRect, 1, BLACK);
    if (valueInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) valueInputEnabled = false;
        int k = GetKeyPressed();
        if (k >= 48 && k <= 57 && strlen(valueText) < 3) {
            int len = strlen(valueText);
            valueText[len] = (char)k;
            valueText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(valueText) > 0) {
            valueText[strlen(valueText) - 1] = '\0';
        }
        if (atoi(valueText) > valueMax) {
            valueText[0] = '9';
            valueText[1] = '9';
            valueText[2] = '\0';
        }
    }
    value = atoi(valueText);
    if (value > valueMax) value = valueMax;
    DrawText(valueText, vRect.x + 5, vRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), vRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
        keyInputEnabled = false;
    }

    Rectangle vRandomRect = {340, 610, 25, 25};
    if (IsButtonClicked(vRandomRect)) {
        value = GetRandomValue(0, 100);
        sprintf(valueText, "%d", value);
    }
    DrawButton("?", vRandomRect, GetFontDefault(), buttonClicked, buttonMessage);

    Rectangle okRect = {350, 650, 40, 30};
    if (IsButtonClicked(okRect)) {
        pause = true;
        Insert(key, value);
        done = 0;
        delta = 0;
        pause = false;
    }
    DrawButton("OK", okRect, GetFontDefault(), buttonClicked, buttonMessage);
}

void HashTable::drawDeleteOptions() {
    static int key = 0;
    static char keyText[4] = "0";
    static int keyMax = 99;
    static bool keyInputEnabled = false;
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    DrawText("K: ", 20, 610, 20, BLACK);
    Rectangle kRect = {50, 610, 100, 25};
    DrawRectangleRec(kRect, WHITE);
    DrawRectangleLinesEx(kRect, 1, BLACK);
    if (keyInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) keyInputEnabled = false;
        int k = GetKeyPressed();
        if (k >= 48 && k <= 57 && strlen(keyText) < 3) {
            int len = strlen(keyText);
            keyText[len] = (char)k;
            keyText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(keyText) > 0) {
            keyText[strlen(keyText) - 1] = '\0';
        }
        if (atoi(keyText) > keyMax) {
            keyText[0] = '9';
            keyText[1] = '9';
            keyText[2] = '\0';
        }
    }
    key = atoi(keyText);
    DrawText(keyText, kRect.x + 5, kRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), kRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        keyInputEnabled = true;
    }

    Rectangle kRandomRect = {160, 610, 25, 25};
    if (IsButtonClicked(kRandomRect)) {
        key = GetRandomValue(0, 100);
        sprintf(keyText, "%d", key);
    }
    DrawButton("?", kRandomRect, GetFontDefault(), buttonClicked, buttonMessage);

    Rectangle okRect = {350, 650, 40, 30};
    if (IsButtonClicked(okRect)) {
        pause = true;
        Delete(key);
        done = 0;
        delta = 0;
        pause = false;
    }
    DrawButton("OK", okRect, GetFontDefault(), buttonClicked, buttonMessage);
}

void HashTable::drawSearchOptions() {
    static int key = 0;
    static char keyText[4] = "0";
    static int keyMax = 99;
    static bool keyInputEnabled = false;
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    DrawText("K: ", 20, 610, 20, BLACK);
    Rectangle kRect = {50, 610, 100, 25};
    DrawRectangleRec(kRect, WHITE);
    DrawRectangleLinesEx(kRect, 1, BLACK);
    if (keyInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) keyInputEnabled = false;
        int k = GetKeyPressed();
        if (k >= 48 && k <= 57 && strlen(keyText) < 3) {
            int len = strlen(keyText);
            keyText[len] = (char)k;
            keyText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(keyText) > 0) {
            keyText[strlen(keyText) - 1] = '\0';
        }
        if (atoi(keyText) > keyMax) {
            keyText[0] = '9';
            keyText[1] = '9';
            keyText[2] = '\0';
        }
    }
    key = atoi(keyText);
    DrawText(keyText, kRect.x + 5, kRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), kRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        keyInputEnabled = true;
    }

    Rectangle kRandomRect = {160, 610, 25, 25};
    if (IsButtonClicked(kRandomRect)) {
        key = GetRandomValue(0, 100);
        sprintf(keyText, "%d", key);
    }
    DrawButton("?", kRandomRect, GetFontDefault(), buttonClicked, buttonMessage);

    Rectangle okRect = {350, 650, 40, 30};
    if (IsButtonClicked(okRect)) {
        pause = true;
        Search(key);
        done = 0;
        delta = 0;
        pause = false;
    }
    DrawButton("OK", okRect, GetFontDefault(), buttonClicked, buttonMessage);
}

// Operation Menu
void HashTable::drawOperationMenu() {
    static bool showInitializeOption = false;
    static float opPosX = 30;
    static float opPosY = 700;
    static float opWidth = 80;
    static float opHeight = 30;
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    Rectangle initializeButtonPos = {opPosX, opPosY, opWidth, opHeight};
    if (IsButtonClicked(initializeButtonPos)) {
        showInitializeOption = !showInitializeOption;
    }
    DrawButton("Init", initializeButtonPos, GetFontDefault(), buttonClicked, buttonMessage);
    if (showInitializeOption) {
        drawInitializeOptions();
    }
    
    static bool showInsertOption = false;
    Rectangle insertButtonPos = {opPosX + (opWidth + 20), opPosY, opWidth, opHeight};
    if (IsButtonClicked(insertButtonPos)) {
        showInsertOption = !showInsertOption;
    }
    DrawButton("Insert", insertButtonPos, GetFontDefault(), buttonClicked, buttonMessage);
    if (showInsertOption) {
        drawInsertOptions();
    }
    
    static bool showDeleteOption = false;
    Rectangle deleteButtonPos = {opPosX + 2 * (opWidth + 20), opPosY, opWidth, opHeight};
    if (IsButtonClicked(deleteButtonPos)) {
        showDeleteOption = !showDeleteOption;
    }
    DrawButton("Delete", deleteButtonPos, GetFontDefault(), buttonClicked, buttonMessage);
    if (showDeleteOption) {
        drawDeleteOptions();
    }
    
    static bool showSearchOption = false;
    Rectangle searchButtonPos = {opPosX + 3 * (opWidth + 20), opPosY, opWidth, opHeight};
    if (IsButtonClicked(searchButtonPos)) {
        showSearchOption = !showSearchOption;
    }
    DrawButton("Search", searchButtonPos, GetFontDefault(), buttonClicked, buttonMessage);
    if (showSearchOption) {
        drawSearchOptions();
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        Rectangle initializeOptionPos = {opPosX, 610, 380, 90};
        if (!CheckCollisionPointRec(mousePoint, initializeOptionPos) && !CheckCollisionPointRec(mousePoint, initializeButtonPos)) {
            showInitializeOption = false;
        }
        Rectangle insertOptionPos = {opPosX, 610, 380, 90};
        if (!CheckCollisionPointRec(mousePoint, insertOptionPos) && !CheckCollisionPointRec(mousePoint, insertButtonPos)) {
            showInsertOption = false;
        }
        Rectangle deleteOptionPos = {opPosX, 610, 380, 90};
        if (!CheckCollisionPointRec(mousePoint, deleteOptionPos) && !CheckCollisionPointRec(mousePoint, deleteButtonPos)) {
            showDeleteOption = false;
        }
        Rectangle searchOptionPos = {opPosX, 610, 380, 90};
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
            DrawRectangleRec(std::get<0>(path), Fade(f, 0.2f));
            DrawRectangleLinesEx(std::get<0>(path), 2, f);
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
    delta += speed;
    DrawRectangle(bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height, Fade(color, 0.2f));
    DrawRectangleLinesEx({bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height}, 2, color);
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
            DrawRectangleRec(std::get<0>(path), Fade(f, 0.2f));
            DrawRectangleLinesEx(std::get<0>(path), 2, f);
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
    delta += speed;
    DrawRectangle(bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height, Fade(color, 0.2f));
    DrawRectangleLinesEx({bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height}, 2, color);
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
            float fade = 0.2f;
            if (f.r == 255 && f.b == 255 && f.g == 255) {
                fade = 1.0f;
            }
            DrawRectangleRec(std::get<0>(path), Fade(f, fade));
            DrawRectangleLinesEx(std::get<0>(path), 2, f);
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
    float fade = 0.2f;
    if (color.r == 255 && color.b == 255 && color.g == 255) {
        fade = 1.0f;
    }
    delta += speed;
    DrawRectangle(bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height, Fade(color, fade));
    DrawRectangleLinesEx({bounds.x, bounds.y, std::min(delta, bounds.width), bounds.height}, 2, color);
    if (delta >= bounds.width) {
        done++;
        delta = 0;
    }
}

void HashTable::resetAnimation() {
    curStep = 0;
    curX = 0;
    delta = 0;
    done = 0;
    doneStep = false;
    doneAnimation = false;
}

// Animation Menu
void HashTable::drawAnimationMenu() {
    float buttonWidth = 80;
    float buttonHeight = 30;
    float spacing = 20;
    float sliderWidth = 280;
    float sliderHeight = 20;
    float posX = 600;
    float buttonPosY = 700;
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    // Vẽ thanh tốc độ
    Rectangle sliderRect = {posX + 100, 670, sliderWidth, sliderHeight};
    DrawRectangleRec(sliderRect, LIGHTGRAY); // Nền của thanh tốc độ

    // Tính toán vị trí của nút kéo dựa trên tốc độ
    // Tốc độ từ 0.1 (minSpeed) đến 10 (maxSpeed), với giá trị 1 ở giữa thanh kéo (t = 0.5)
    float t;
    if (speed <= 1.0f) {
        // Đoạn từ 0.1 đến 1 ánh xạ từ t = 0 đến t = 0.5
        t = (speed - minSpeed) / (1.0f - minSpeed) * 0.5f;
    } else {
        // Đoạn từ 1 đến 10 ánh xạ từ t = 0.5 đến t = 1
        t = 0.5f + (speed - 1.0f) / (maxSpeed - 1.0f) * 0.5f;
    }
    float sliderPos = t * (sliderWidth - scrollBarWidth);
    Rectangle thumbRect = { posX + 100 + sliderPos, 670, scrollBarWidth, sliderHeight };
    DrawRectangleRec(thumbRect, DARKGRAY); // Nút kéo của thanh tốc độ

    // Hiển thị nhãn "Speed" và giá trị tốc độ
    DrawText("Speed", posX + 35, 670, 20, BLACK);
    char* speedText = ftc(speed);
    DrawText(speedText, posX + 390, 670, 20, BLACK);
    delete[] speedText;

    // Xử lý tương tác với thanh tốc độ
    if (CheckCollisionPointRec(GetMousePosition(), sliderRect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float mouseX = GetMouseX();
        t = (mouseX - (posX + 100)) / (sliderWidth - scrollBarWidth);
        t = std::max(0.0f, std::min(t, 1.0f)); // Giới hạn t trong [0, 1]

        // Tính speed từ t
        if (t <= 0.5f) {
            // Đoạn từ t = 0 đến t = 0.5 ánh xạ từ speed = 0.1 đến speed = 1
            speed = minSpeed + (t / 0.5f) * (1.0f - minSpeed);
        } else {
            // Đoạn từ t = 0.5 đến t = 1 ánh xạ từ speed = 1 đến speed = 10
            speed = 1.0f + ((t - 0.5f) / 0.5f) * (maxSpeed - 1.0f);
        }
        speed = std::max(minSpeed, std::min(speed, maxSpeed)); // Đảm bảo tốc độ nằm trong khoảng [0.1, 10]
    }
    
    Rectangle firstRect = {posX, buttonPosY, buttonWidth, buttonHeight};
    if (IsButtonClicked(firstRect)) {
        resetAnimation();
        pause = true;
    }
    DrawButton("First", firstRect, GetFontDefault(), buttonClicked, buttonMessage);
    
    Rectangle prevRect = {posX + buttonWidth + spacing, buttonPosY, buttonWidth, buttonHeight};
    if (IsButtonClicked(prevRect)) {
        pause = true;
        if (curStep > 0) {
            curStep--;
            done = 0;
            doneStep = true;
            doneAnimation = false;
            curX = 0;
            delta = 0;
        }
    }
    DrawButton("Prev", prevRect, GetFontDefault(), buttonClicked, buttonMessage);
    
    Rectangle playPauseRect = {posX + 2*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight};
    if (pause) {
        if (IsButtonClicked(playPauseRect)) {
            pause = false;
        }
        DrawButton("Play", playPauseRect, GetFontDefault(), buttonClicked, buttonMessage);
    } else {
        if (IsButtonClicked(playPauseRect)) {
            pause = true;
        }
        DrawButton("Pause", playPauseRect, GetFontDefault(), buttonClicked, buttonMessage);
    }
    
    Rectangle nextRect = {posX + 3*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight};
    if (IsButtonClicked(nextRect)) {
        pause = true;
        if (curStep < totalStep - 1) {
            curStep++;
        }
    }
    DrawButton("Next", nextRect, GetFontDefault(), buttonClicked, buttonMessage);
    
    Rectangle lastRect = {posX + 4*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight};
    if (IsButtonClicked(lastRect)) {
        pause = true;
        curStep = totalStep - 1;
    }
    DrawButton("Last", lastRect, GetFontDefault(), buttonClicked, buttonMessage);
}

// Code description Menu
void HashTable::drawInsertDescription() {
    DrawText("Inserting:", 500, 610, 20, BLACK);
    if (insertDescriptions.size() > curStep) {
        DrawText(insertDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = insertCodeIndex.size() > curStep ? insertCodeIndex[curStep] : 0;
    for (int i = 0; i < insertCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(insertCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    if (insertPaths1.size() > curStep) {
        for (auto& path : insertPaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
    if (pause && insertPaths2.size() > curStep) {
        for (auto& path : insertPaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
}

void HashTable::drawDeleteDescription() {
    DrawText("Deleting:", 500, 610, 20, BLACK);
    if (deleteDescriptions.size() > curStep) {
        DrawText(deleteDescriptions[curStep].c_str(), 500, 630, 20, BLACK);
    }
    int stt = deleteCodeIndex.size() > curStep ? deleteCodeIndex[curStep] : 0;
    for (int i = 0; i < deleteCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(deleteCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    if (deletePaths1.size() > curStep) {
        for (auto& path : deletePaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
    if (pause && deletePaths2.size() > curStep) {
        for (auto& path : deletePaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
}

void HashTable::drawSearchDescription() {
    DrawText("Searching:", 500, 610, 20, BLACK);
    if (searchDescriptions.size() > curStep) {
        DrawText(searchDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = searchCodeIndex.size() > curStep ? searchCodeIndex[curStep] : 0;
    for (int i = 0; i < searchCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(searchCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    if (searchPaths1.size() > curStep) {
        for (auto& path : searchPaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
    if (pause && searchPaths2.size() > curStep) {
        for (auto& path : searchPaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
}

void HashTable::drawInitializeDescription() {
    DrawText("Initializing:", 980, 20, 20, BLACK);
    if (initDescriptions.size() > curStep) {
        DrawText(initDescriptions[curStep].c_str(), 980, 40, 20, BLACK);
    }
    int stt = initCodeIndex.size() > curStep ? initCodeIndex[curStep] : 0;
    for (int i = 0; i < insertCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(insertCodes[i].c_str(), 1000, 60 + i * 20, 20, c);
        if (stt == i) {
            DrawText(">>", 980, 60 + i * 20, 20, RED);
        }
    }
    if (initPaths1.size() > curStep) {
        for (auto& path : initPaths1[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
    if (pause && initPaths2.size() > curStep) {
        for (auto& path : initPaths2[curStep]) {
            if (std::get<0>(path).width > 0) {
                DrawRectangleRec(std::get<0>(path), Fade(std::get<1>(path), 0.2f));
                DrawRectangleLinesEx(std::get<0>(path), 2, std::get<1>(path));
            }
        }
    }
    float progress = (float)(curStep + 1) / totalStep * 100;
    DrawText(TextFormat("Progress: %.0f%%", progress), 980, 460, 20, BLACK);
}

// Node detail Menu
void HashTable::drawNodeDetailMenu() {
    bool buttonClicked = false;
    const char* buttonMessage = nullptr;

    if (selectedNode) {
        DrawRectangleRec(selectedNodeArea, Fade(BLUE, 0.2f));
        
        static char valueText[4] = "0";
        static bool vInputEnabled = false;

        DrawText("Node detail:", 1100, 610, 20, BLACK);
        DrawText(TextFormat("- Key: %d", selectedNode->key), 1100, 640, 20, BLACK);
        DrawText("- Value: ", 1100, 670, 20, BLACK);

        Rectangle vRect = {1100 + 80, 670, 100, 25};
        DrawRectangleRec(vRect, WHITE);
        DrawRectangleLinesEx(vRect, 1, BLACK);
        if (vInputEnabled) {
            if (IsKeyPressed(KEY_ENTER)) vInputEnabled = false;
            int k = GetKeyPressed();
            if (k >= 48 && k <= 57 && strlen(valueText) < 3) {
                int len = strlen(valueText);
                valueText[len] = (char)k;
                valueText[len + 1] = '\0';
            }
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(valueText) > 0) {
                valueText[strlen(valueText) - 1] = '\0';
            }
        }
        selectedValue = atoi(valueText);
        DrawText(valueText, vRect.x + 5, vRect.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), vRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            vInputEnabled = true;
        }
        
        Rectangle deleteRect = {1100, 700, 80, 30};
        if (IsButtonClicked(deleteRect)) {
            Delete(selectedNode->key);
            done = 0;
            delta = 0;
            selectedNode = nullptr;
        }
        DrawButton("Delete", deleteRect, GetFontDefault(), buttonClicked, buttonMessage);

        Rectangle updateRect = {1200, 700, 80, 30};
        if (IsButtonClicked(updateRect)) {
            Insert(selectedNode->key, selectedValue);
            done = 0;
            delta = 0;
            selectedNode = nullptr;
        }
        DrawButton("Update", updateRect, GetFontDefault(), buttonClicked, buttonMessage);
    }
}

// Screen
void HashTable::DrawScreen() {
    ClearBackground(WHITE);
    DrawTextEx(GetFontDefault(), "Hash Table", Vector2{ 500, 20 }, 20, 2, BLACK);
    drawOperationMenu();
    drawAnimationMenu();

    if ((operation_type < 0) || (curStep == totalStep - 1 && totalStep > 0)) {
        drawTable();
    } else {
        drawPrevTable();
    }

    // Vẽ và xử lý thanh kéo ngang
    DrawRectangleRec(scrollBarRect, LIGHTGRAY); // Nền của thanh kéo
    float scrollBarPos = maxScrollOffsetX > 0 ? (scrollOffsetX / maxScrollOffsetX) * (scrollBarRect.width - scrollBarWidth) : 0;
    Rectangle thumbRect = { scrollBarRect.x + scrollBarPos, scrollBarRect.y, scrollBarWidth, scrollBarRect.height };
    DrawRectangleRec(thumbRect, DARKGRAY); // Nút kéo (thumb) của thanh kéo

    // Xử lý tương tác với thanh kéo
    if (CheckCollisionPointRec(GetMousePosition(), scrollBarRect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float mouseX = GetMouseX();
        float t = (mouseX - scrollBarRect.x) / (scrollBarRect.width - scrollBarWidth);
        t = std::max(0.0f, std::min(t, 1.0f)); // Giới hạn giá trị t trong khoảng [0, 1]
        scrollOffsetX = t * maxScrollOffsetX;
    }

    drawNodeDetailMenu();

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

    // Vẽ nút "Back" ở góc trái trên với kích thước tương đương một node
    Rectangle backButtonBounds = { 20, 20, 65, 65 }; // Kích thước nút 65x65 pixels, tương đương nodeWidth
    DrawBackButton(backButtonTexture, backButtonBounds, backButtonClicked);
}