#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "raylib.h"
#include <vector>
#include <string>
#include <tuple>
#include <fstream>
#include "tinyfiledialogs.h"

struct HashNode {
    int key;
    int value;
    HashNode* next;
    HashNode(int k, int v): key(k), value(v), next(nullptr) {}
};

class HashTable {
private:
        // Thanh kéo để cuộn ngang
    float scrollOffsetX = 0; // Độ lệch ngang khi cuộn
    float maxScrollOffsetX = 0; // Độ lệch tối đa có thể cuộn
    Rectangle scrollBarRect = { 30, 850, 300, 20 }; // Vị trí và kích thước của thanh kéo (góc trái dưới)
    float scrollBarWidth = 20; // Chiều rộng của nút kéo (thumb) trên thanh kéo
    Texture2D backButtonTexture; // Texture cho nút "Back"
    bool backButtonClicked = false; // Trạng thái của nút "Back"
	void insert(int m, int n);

    // hash table
    int hash(int key) { return size > 0 ? key % size : 0; }
    int size;
	float tablePosX = 550;
	float tablePosY = 100;
    float indexWidth = 80;
	float nodeWidth = 65;
	float nodeHeight = 25;
	float nodeSpaceX = 10;
	float nodeSpaceY = 10;
	float llPosX = tablePosX + nodeSpaceX + indexWidth;
    float lineWidth = 2;
    std::vector<HashNode*> table;
    std::vector<HashNode*> prevTable;
    void drawTable();
    void drawPrevTable();
    void saveCurrentTable();
    Rectangle calIndexArea(int hashValue);
    Rectangle calNodeArea(int idx, int hashValue);
    Rectangle calEdgeArea(int idx, int hashValue);
    Rectangle calEdgeXArea(int idx, int hashValue, int i);
    
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
    std::string selectedFilePath;  // Path to selected file
    bool fileLoaded;              // Flag for successful file loading

    // animation control menu
    void drawAnimationMenu();
	void resetAnimation();
    int curStep = 0;
    int totalStep = 0;
    float curX = 0;
    float delta = 0;
    float speed = 1; // Tốc độ ban đầu là 1
    float minSpeed = 0.1f; // Tốc độ tối thiểu
    float maxSpeed = 10.0f; // Tốc độ tối đa 
    bool pause = false;
    int done = 0;
    bool doneStep = true;
    bool doneAnimation = false;
    std::vector<std::pair<int, int>> pendingInsertions; // Store key-value pairs to insert
	int insertionIndex = 0;
    int operation_type;
	// insert, operation type = 1
	int insertIndex;
	std::vector<std::string> initCodes = {
		"Init(m, n): Begin initialization",
		"table.resize(m, nullptr); size = m",
		"for (n insertions): Insert(key, value)",
		"Initialization complete"
	};
	std::vector<std::string> insertCodes = {
		" 1. void HashTable::Insert(int key, int value) {",
		" 2.    int hVal = hash(key);",
		" 3.    if (!table[hVal]) {",
		" 4.    HNode* newNode = new HNode(key, value);",
		" 5.    table[hVal] = newNode;",
		" 6.    } else {",
		" 7.        HNode* curr = table[hVal];",
		" 8. while (curr->next != NULL && curr -> key != key){",
		" 9.            curr = curr->next;",
		"10.        }",
		"11.        if (curr->key == key) {",
		"12.            curr->value = value;",
		"13.        } else {",
		"14.      HNode* newNode = new HNode(key, value);",
		"15.      curr->next = newNode;",
		"16.        }",
		"17.    }",
		"18. }"
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
        " 2.    int hVal = hash(key);",
        " 3.    HNode* curr = table[hVal];",
        " 4.    if (curr == NULL) {",
        " 5.        return;",
        " 6.    }",
        " 7.    if (curr->key == key) {",
        " 8.        table[hVal] = curr->next;",
        " 9.        delete curr;",
        "10.        return;",
        "11.    }",
        "12.    while (curr->next) {",
        "13.        if (curr->next->key == key) {",
        "14.            HNode* temp = curr->next;",
        "15.            curr->next = temp->next;",
        "16.            delete temp;",
        "17.            return;",
        "18.        }",
        "19.        curr = curr->next;",
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
        " 1. HNode* HashTable::Search(int key) {",
        " 2.    int hVal = hash(key);",
        " 3.    HNode* curr = table[hVal];",
        " 4.    while (curr) {",
        " 5.        if (curr->key == key) {",
        " 6.            return curr;",
        " 7.        }",
        " 8.        curr = curr->next;",
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

	// Initialization animation variables
	std::vector<std::string> initDescriptions;
	std::vector<int> initCodeIndex;
	std::vector<std::vector<std::tuple<Rectangle, Color>>> initPaths1;
	std::vector<std::vector<std::tuple<Rectangle, Color>>> initPaths2;
	void drawInitializeAnimation();
	void drawInitializeDescription();
	
	// code area
	float codePosX = 10;
	float codePosY = 60;
	float codePosSpace = 25;
	float codeWidth = 700;
	float codeHeight = 20;

public:
    HashTable(int size);
    ~HashTable();
    void Init(int m, int n);
    void Insert(int key, int value);
    void Delete(int key);
    HashNode* Search(int key);
    void DrawScreen();
    bool IsBackButtonClicked() const { return backButtonClicked; }
};

#endif