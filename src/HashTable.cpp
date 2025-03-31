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
    for (int i = 0; i < size; i++) {
		// Index node
        GuiButton({
            tablePosX,
            tablePosY + (nodeHeight + nodeSpaceY) * i,
            indexWidth,
            nodeHeight
        }, TextFormat("Index: %d", i));
        HashNode* current = prevTable[i];
        if (!current) {
			// edge[0]
            GuiDrawRectangle({
                llPosX - 10,
                tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2,
                nodeSpaceX, 2
            }, 2, BLACK, BLACK);
            // node[0]
			if (GuiButton({ llPosX, tablePosY + (nodeHeight + nodeSpaceY) * i, nodeWidth, nodeHeight }, "NULL")) {
                selectedNode = nullptr;
            }
        }
        int idx = 0;
        while (current) {
			// edge[0]
            GuiDrawRectangle({ 
                llPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX,
                tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2,
                nodeSpaceX,
                lineWidth
            }, lineWidth, BLACK, BLACK);
            // node[0]
			if (GuiButton({
                llPosX + (nodeWidth + nodeSpaceX) * idx,
                tablePosY + (nodeHeight + nodeSpaceY) * i,
                nodeWidth,
                nodeHeight
            }, TextFormat("%d | %d", current->key, current->value))) {
                selectedNode = current;
                selectedValue = current->value;
                selectedNodeArea = {
                    llPosX + (nodeWidth + nodeSpaceX) * idx,
                    tablePosY + (nodeHeight + nodeSpaceY) * i,
                    nodeWidth,
                    nodeHeight
                };
            }
            current = current->next;
            idx++;
        }
    }
}

void HashTable::drawTable() {
    for (int i = 0; i < size; i++) {
		// Index node
        GuiButton({
            tablePosX,
            tablePosY + (nodeHeight + nodeSpaceY) * i,
            indexWidth,
            nodeHeight
        }, TextFormat("Index: %d", i));
        HashNode* current = table[i];
        if (!current) {
			// edge[0]
            GuiDrawRectangle({
                llPosX - 10,
                tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2,
                nodeSpaceX, 2
            }, 2, BLACK, BLACK);
            // node[0]
			if (GuiButton({ llPosX, tablePosY + (nodeHeight + nodeSpaceY) * i, nodeWidth, nodeHeight }, "NULL")) {
                selectedNode = nullptr;
            }
        }
        int idx = 0;
        while (current) {
			// edge[0]
            GuiDrawRectangle({ 
                llPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX,
                tablePosY + (nodeHeight + nodeSpaceY) * i + nodeHeight / 2,
                nodeSpaceX,
                lineWidth
            }, lineWidth, BLACK, BLACK);
            // node[0]
			if (GuiButton({
                llPosX + (nodeWidth + nodeSpaceX) * idx,
                tablePosY + (nodeHeight + nodeSpaceY) * i,
                nodeWidth,
                nodeHeight
            }, TextFormat("%d | %d", current->key, current->value))) {
                selectedNode = current;
                selectedValue = current->value;
                selectedNodeArea = {
                    llPosX + (nodeWidth + nodeSpaceX) * idx,
                    tablePosY + (nodeHeight + nodeSpaceY) * i,
                    nodeWidth,
                    nodeHeight
                };
            }
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
}

void HashTable::insert(int key, int value) {
	int hVal = hash(key);
	if (!table[hVal]) {
	    HashNode* newNode = new HashNode(key, value);
	    table[hVal] = newNode;
	} else {
	    HashNode* curr = table[hVal];
	    while (curr->next != NULL && curr -> key != key) {
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

    //
    if (!table[hashValue]) {
        // table index is null
        insertDescriptions.push_back("Checking collision condition: No collision.");
        insertCodeIndex.push_back(2);
        insertPaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        insertPaths2.push_back({ 
            std::make_tuple(calEdgeArea(0, hashValue), GREEN),
            std::make_tuple(calNodeArea(0, hashValue), GREEN)
         });

        // create node
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
        // insert node
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
        // table index is not null
        insertDescriptions.push_back("Checking collision condition: Collision detected.");
        insertCodeIndex.push_back(2);
        insertPaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        insertPaths2.push_back({ 
            std::make_tuple(calEdgeArea(0, hashValue), RED),
            std::make_tuple(calNodeArea(0, hashValue), RED)
         });
        // begin traversal
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
       // check the node
    int idx = 0;
    while (current->next != NULL && current->key != key) {
        insertDescriptions.push_back("Checking condition: Key not found");
        insertCodeIndex.push_back(7);
        insertPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            std::make_tuple(calNodeArea(idx, hashValue), BLUE)
        });
        insertPaths2.push_back({ 
            std::make_tuple(calEdgeArea(idx + 1, hashValue), RED), // Highlight current node as RED (key mismatch)
            std::make_tuple(calNodeArea(idx + 1, hashValue), RED)
        });
        current = current->next;
        idx++; // Increment idx to move to the next node position
        insertDescriptions.push_back("Move to the next node");
        insertCodeIndex.push_back(8);
        insertPaths1.push_back({
            std::make_tuple(calIndexArea(hashValue), YELLOW),
            // std::make_tuple(Rectangle{ llPosX + 60 * idx - 10, tablePosY + 30 * hashValue + 10, 10, 2 }, RED),
            std::make_tuple(calNodeArea(idx - 1, hashValue), BLUE)
        });
        insertPaths2.push_back({ 
            std::make_tuple(calEdgeArea(idx, hashValue), BLUE),
            std::make_tuple(calNodeArea(idx, hashValue), BLUE) // Highlight next node as BLUE
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
            //
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
    deletePaths2.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
    //
    HashNode* current = table[hashValue];
    deleteDescriptions.push_back("Begin traversal at slot " + std::to_string(hashValue));
    deleteCodeIndex.push_back(2);
    deletePaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
    deletePaths2.push_back({
        std::make_tuple(calEdgeArea(0, hashValue), BLUE),
        std::make_tuple(calNodeArea(0, hashValue), BLUE)
    });
    //
    if (current == NULL) {
        deleteDescriptions.push_back("Slot NULL: " + std::to_string(hashValue));
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        deletePaths2.push_back({
            // std::make_tuple(Rectangle{ llPosX - 10, tablePosY + 30 * hashValue + 10, 10, 2 }, BLUE),
            std::make_tuple(calNodeArea(0, hashValue), RED)
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
		doneAnimation = false;
        return;
    }
    //
    if (current->key == key) {
        deleteDescriptions.push_back("Checking key: Found key " + std::to_string(key));
        deleteCodeIndex.push_back(6);
        deletePaths1.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
        deletePaths2.push_back({
            std::make_tuple(calEdgeArea(0, hashValue), BLUE),
            std::make_tuple(calNodeArea(0, hashValue), GREEN)
        });
        //
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
        //
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
        //
        deleteDescriptions.push_back("Done!");
        deleteCodeIndex.push_back(9);
        deletePaths1.push_back({});
        deletePaths2.push_back({});
        //
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
        // std::make_tuple(calEdgeArea(0, hashValue), RED),
        std::make_tuple(calNodeArea(0, hashValue), RED)
    });
    //
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
        //
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
            //
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
            //
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
            //
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
            //
            curStep = 0;
            totalStep = deleteCodeIndex.size();
            operation_type = 2;
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

        //
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
    //
    deleteDescriptions.push_back("Done!");
    deleteCodeIndex.push_back(20);
    deletePaths1.push_back({});
    deletePaths2.push_back({});
    //
    curStep = 0;
    totalStep = deleteCodeIndex.size();
    operation_type = 2;
	doneAnimation = false;
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
    searchPaths2.push_back({ std::make_tuple(calIndexArea(hashValue), YELLOW) });
    //
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
            //
            searchDescriptions.push_back("Done: Found key " + std::to_string(key));
            searchCodeIndex.push_back(5);
            searchPaths1.push_back({
                std::make_tuple(calIndexArea(hashValue), YELLOW),
                std::make_tuple(calNodeArea(idx, hashValue), GREEN)
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
        GuiButton(Rectangle{ tablePosX, tablePosY + 30 * i, 60, 20 }, TextFormat("Key: %d", i));
        HashNode* current = table[i];
        int idx = 0;

        // Draw nodes incrementally for the current slot
        while (current && (i < curStep || idx <= done)) {
            Rectangle nodeBounds = { llPosX + 60 * idx, tablePosY + 30 * i, 50, 20 };

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

    GuiLabel(Rectangle{ 20, 610, 25, 25 }, "M: ");
    if (GuiSpinner(Rectangle{ 50, 610, 100, 25 }, NULL, &mValue, 1, 14, mInputEnabled)) {
        mInputEnabled = !mInputEnabled;
    }
    if (GuiButton(Rectangle{ 160, 610, 25, 25 }, "#193#")) {
        mValue = GetRandomValue(1, 14);
    }

    GuiLabel(Rectangle{ 200, 610, 25, 25 }, "N: ");
    if (GuiSpinner(Rectangle{ 230, 610, 100, 25 }, NULL, &nValue, 0, 99, nInputEnabled)) {
        nInputEnabled = !nInputEnabled;
    }
    if (GuiButton(Rectangle{ 340, 610, 25, 25 }, "#193#")) {
        nValue = GetRandomValue(0, 99);
    }

    if (GuiButton(Rectangle{ 30, 650, 80, 30 }, "Upload")) {
		showUploadPrompt = true;
        fileLoaded = false;
        TraceLog(LOG_INFO, "Upload button clicked, waiting for file drop");
	}

    if (GuiButton(Rectangle{ 350, 650, 40, 30 }, "OK")) {
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
        const char* message = "Drop a file here";
        Vector2 textSize = MeasureTextEx(GetFontDefault(), message, 16, 1);
        Vector2 position = { (float)(GetScreenWidth() - textSize.x) / 2, (float)(GetScreenHeight() - textSize.y) / 2 };
        // DrawText(message, (int)position.x, (int)position.y, 20, BLACK);
        DrawText(message, 120, 655, 20, GRAY);
    }
}

void HashTable::drawInsertOptions() {
    static int key = 0;
    static int value = 0;
    static bool keyInputEnabled = false;
    static bool valueInputEnabled = false;

    GuiLabel(Rectangle{ 20, 610, 25, 25 }, "K: ");
    if (GuiSpinner(Rectangle{ 50, 610, 100, 25 }, NULL, &key, 0, 100, keyInputEnabled)) {
        keyInputEnabled = !keyInputEnabled;
    }
    if (GuiButton(Rectangle{ 160, 610, 25, 25 }, "#193#")) {
        key = GetRandomValue(0, 100);
    }

    GuiLabel(Rectangle{ 200, 610, 25, 25 }, "V: ");
    if (GuiSpinner(Rectangle{ 230, 610, 100, 25 }, NULL, &value, 0, 100, valueInputEnabled)) {
        valueInputEnabled = !valueInputEnabled;
    }
    if (GuiButton(Rectangle{ 340, 610, 25, 25 }, "#193#")) {
        value = GetRandomValue(0, 100);
    }

    if (GuiButton(Rectangle{ 350, 650, 40, 30 }, "OK")) {
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

    GuiLabel(Rectangle{ 20, 610, 25, 25 }, "K: ");
    if (GuiSpinner(Rectangle{ 50, 610, 100, 25 }, NULL, &key, 0, 100, keyInputEnabled)) {
        keyInputEnabled = !keyInputEnabled;
    }
    if (GuiButton(Rectangle{ 160, 610, 25, 25 }, "#193#")) {
        key = GetRandomValue(0, 100);
    }

    if (GuiButton(Rectangle{ 350, 650, 40, 30 }, "OK")) {
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

    GuiLabel(Rectangle{ 20, 610, 25, 25 }, "K: ");
    if (GuiSpinner(Rectangle{ 50, 610, 100, 25 }, NULL, &key, 0, 100, keyInputEnabled)) {
        keyInputEnabled = !keyInputEnabled;
    }
    if (GuiButton(Rectangle{ 160, 610, 25, 25 }, "#193#")) {
        key = GetRandomValue(0, 100);
    }

    if (GuiButton(Rectangle{ 350, 650, 40, 30 }, "OK")) {
        pause = true;
        Search(key);
        done = 0;
        delta = 0;
        pause = false;
    }
}

// Operation Menu
void HashTable::drawOperationMenu() {
    // GuiDrawRectangle({ 10, 600, 450, 150 }, 2, GRAY, Color{ 0, 0, 0, 0 });
    static bool showInitializeOption = false;
	static float opPosX = 30;
	static float opPosY = 700;
	static float opWidth = 80;
	static float opHeight = 30;
    Rectangle initializeButtonPos = { opPosX, opPosY, opWidth, opHeight };
    Rectangle initializeOptionPos = { opPosX, 610, 380, 90 };
    if (GuiButton(initializeButtonPos, "Init")) {
        showInitializeOption = !showInitializeOption;
    }
    if (showInitializeOption) {
        drawInitializeOptions();
    }
    static bool showInsertOption = false;
    Rectangle insertButtonPos = { opPosX + (opWidth + 20), opPosY, opWidth, opHeight };
    Rectangle insertOptionPos = { opPosX, 610, 380, 90 };
    if (GuiButton(insertButtonPos, "Insert")) {
        showInsertOption = !showInsertOption;
    }
    if (showInsertOption) {
        drawInsertOptions();
    }
    static bool showDeleteOption = false;
    Rectangle deleteButtonPos = { opPosX + 2 *(opWidth + 20), opPosY, opWidth, opHeight };
    Rectangle deleteOptionPos = { opPosX, 610, 380, 90 };
    if (GuiButton(deleteButtonPos, "Delete")) {
        showDeleteOption = !showDeleteOption;
    }
    if (showDeleteOption) {
        drawDeleteOptions();
    }
    static bool showSearchOption = false;
    Rectangle searchButtonPos = { opPosX + 3 * (opWidth + 20), opPosY, opWidth, opHeight };
    Rectangle searchOptionPos = { opPosX, 610, 380, 90 };
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

void HashTable::resetAnimation() {
	curStep = 0;
	curX = 0;
	delta = 0;
	done = 0;
	doneStep = false;
	doneAnimation = false;
}

// Amination Menu
void HashTable::drawAnimationMenu() {
    // Adjusted dimensions and positions for larger buttons and slider
    float buttonWidth = 40;  // Increased button width
    float buttonHeight = 30; // Increased button height
    float spacing = 20;      // Spacing between buttons
    float sliderWidth = 280; // Increased slider width
    float sliderHeight = 20; // Increased slider height
	float posX = 700;
	float buttonPosY = 700;

    // Draw the speed slider
    GuiSlider(Rectangle{posX, 670, sliderWidth, sliderHeight}, "Speed", ftc(speed), &speed, 1, 30);
	
    // Draw the animation control buttons
	// -- To first step
	if (GuiButton(Rectangle{posX, buttonPosY, buttonWidth, buttonHeight}, "#129#")) {
		resetAnimation();
		pause = true;
	}
	// -- To previous step
	if (GuiButton(Rectangle{posX + buttonWidth + spacing, buttonPosY, buttonWidth, buttonHeight}, "#72#")) {
		pause = true;
		if (curStep > 0) {
			curStep--; done = 0; doneStep = true; doneAnimation = false; curX = 0; delta = 0;
		}
	}
	// -- Play / Pause
	if (pause) {
		if (GuiButton(Rectangle{posX + 2*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight}, "#131#")) {
			pause = false;
		}
	} else {
		if (GuiButton(Rectangle{posX + 2*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight}, "#132#")) {
			pause = true;
		}
	}
	// -- To next step
	if (GuiButton(Rectangle{posX + 3*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight}, "#73#")) {
		pause = true;
		if (curStep < totalStep - 1) {
			curStep++;
		}
	}
	// -- To last step
    if (GuiButton(Rectangle{posX + 4*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight}, "#134#")) {
        pause = true;
        curStep = totalStep - 1;
    }

    GuiLabel(Rectangle{ 500, 670, 130, 20 }, TextFormat("Step %d/%d", curStep + 1, totalStep));
}

// Code description Menu
void HashTable::drawInsertDescription() {
    GuiLabel(Rectangle{ 500, 610, 150, 20 }, "Inserting:");
    // for (int i = 0; i < insertCodeIndex.size(); i++) {
    //     GuiLabel(Rectangle{ 10.0f + 20 * i, 470, 20, 20 }, std::to_string(insertCodeIndex[i]).c_str());
    // }
    if (insertDescriptions.size() > curStep) {
        GuiLabel(Rectangle{ 500, 640, 500, 20 }, insertDescriptions[curStep].c_str());
    }
    int stt = insertCodeIndex.size() > curStep ? insertCodeIndex[curStep] : 0;
    for (int i = 0; i < insertCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        Rectangle bounds = { codePosX, codePosY + i * codePosSpace, codeWidth, codeHeight };
        GuiDrawText(insertCodes[i].c_str(), GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), c);
        if (stt == i) {
            GuiLabel(Rectangle{ codePosX - 20, codePosY + i * codePosSpace, 20, 20 }, ">>");
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
    GuiLabel(Rectangle{ 500, 610, 150, 20 }, "Deleting");
    if (deleteDescriptions.size() > curStep) {
        GuiLabel(Rectangle{ 500, 630, 500, 20 }, deleteDescriptions[curStep].c_str());
    }
    int stt = deleteCodeIndex.size() > curStep ? deleteCodeIndex[curStep] : 0;
    for (int i = 0; i < deleteCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        Rectangle bounds = { codePosX, codePosY + i * codePosSpace, codeWidth, codeHeight };
        GuiDrawText(deleteCodes[i].c_str(), GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), c);
        if (stt == i) {
            GuiLabel(Rectangle{ codePosX - 20, codePosY + i * codePosSpace, 20, codeHeight }, ">>");
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
    GuiLabel(Rectangle{ 500, 610, 150, 20 }, "Searching: ");
    if (searchDescriptions.size() > curStep) {
        GuiLabel(Rectangle{ 500, 640, 500, 20 }, searchDescriptions[curStep].c_str());
    }
    int stt = searchCodeIndex.size() > curStep ? searchCodeIndex[curStep] : 0;
    for (int i = 0; i < searchCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        Rectangle bounds = { codePosX, codePosY + i * codePosSpace, codeWidth, codeHeight };
        GuiDrawText(searchCodes[i].c_str(), GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), c);
        if (stt == i) {
            GuiLabel(Rectangle{ codePosX - 20, codePosY + i * codePosSpace, 20, 20 }, ">>");
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

        GuiLabel(Rectangle{ 1050, 610, 150, 25 }, TextFormat("Node detail:"));

        GuiLabel(Rectangle{ 1070, 640, 100, 25 }, TextFormat("- Key: %d", selectedNode->key));
        GuiLabel(Rectangle{ 1070, 670, 100, 25 }, "- Value: ");

        if (GuiSpinner(Rectangle{ 1070 + 75, 670, 100, 25 }, NULL, &selectedValue, 0, 100, vInputEnabled)) {
            vInputEnabled = !vInputEnabled;
        };
        
        if (GuiButton(Rectangle{ 1100, 700, 80, 30 }, "Delete")) {
            Delete(selectedNode->key);
            done = 0;
            delta = 0;
            selectedNode = nullptr;
        };
        if (GuiButton(Rectangle{ 1200, 700, 80, 30 }, "Update")) {
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
    DrawTextEx(GetFontDefault(), "Hash Table", Vector2{ 500, 20 }, 20, 2, BLACK);
    drawOperationMenu();
    drawAnimationMenu();

    // Only draw the table or previous table when the animation is complete
	if ((operation_type < 0) || (curStep == totalStep - 1 && totalStep > 0)) {
		drawTable();
	} else {
		drawPrevTable();
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
    //if (operation_type == 4) { // Initialization animation
    //    drawInitializeDescription();
    //    drawInitializeAnimation();
    //}
}