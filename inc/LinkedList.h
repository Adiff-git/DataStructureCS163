#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "raylib.h"
#include <vector>
#include <string>
#include <tuple>
#include <fstream>

struct ListNode {
    int value;
    ListNode* next;
    ListNode(int v) : value(v), next(nullptr) {}
};

class LinkedList {
private:
    Texture2D backButtonTexture; // Texture cho nút "Back"
    bool backButtonClicked = false; // Trạng thái của nút "Back"
    float scrollOffsetX = 0; // Độ lệch ngang khi cuộn
    float maxScrollOffsetX = 0; // Độ lệch tối đa có thể cuộn
    Rectangle scrollBarRect = { 30, 850, 300, 20 }; // Vị trí và kích thước của thanh kéo (góc trái dưới)
    float scrollBarWidth = 20; // Chiều rộng của nút kéo (thumb) trên thanh kéo
    ListNode* head;
    ListNode* prevHead; // Để lưu trạng thái trước đó
    ListNode* foundNode = nullptr; // To store the node found during search
    
    // Visualization parameters
    float listPosX = 550;
    float listPosY = 300;
    float nodeWidth = 65;
    float nodeHeight = 25;
    float nodeSpaceX = 10;
    float lineWidth = 2;
    
    // Animation control
    int curStep = 0;
    int totalStep = 0;
    float delta = 0;
    float speed = 1; // Tốc độ ban đầu là 1
    float minSpeed = 0.1f; // Tốc độ tối thiểu
    float maxSpeed = 10.0f; // Tốc độ tối đa
    bool pause = false;
    int done = 0;
    bool doneStep = true;
    bool doneAnimation = false;
    int operation_type; // -1: none, 1: add_head, 2: add_index, 3: add_tail, 4: delete, 5: search, 6: init, 7: update
    
    // File handling
    char pathfile[1024];
    bool fileLoaded;
    bool showUploadPrompt;
    
    // Node selection
    ListNode* selectedNode;
    int selectedValue;
    Rectangle selectedNodeArea;
    
    // Drawing functions
    void drawList();
    void drawPrevList();
    void saveCurrentList();
    Vector2 calNodeCenter(int idx);
    // float calNodeRadius();
    Rectangle calEdgeArea(int idx);
    Vector2 calEdgeCenter(int idx);
    
    // Operation menu
    void drawOperationMenu();
    void drawInitializeOptions();
    void drawAddHeadOptions();
    void drawAddIndexOptions();
    void drawAddTailOptions();
    void drawDeleteOptions();
    void drawSearchOptions();
    void drawUpdateOptions();
    
    // Node detail menu
    void drawNodeDetailMenu();
    
    // Animation menu
    void drawAnimationMenu();
    void resetAnimation();
    
    // Code and descriptions
    std::vector<std::string> initCodes = {
        "Initialize: Clear List",
        "Add Random Node",
        "Initialization complete"
    };
    std::vector<std::string> addHeadCodes = {
        "AddHead: Node* newNode = new Node(value);",
        "newNode->next = head;",
        "head = newNode;"
    };
    std::vector<std::string> addIndexCodes = {
        "AddIndex: Node* newNode = new Node(value);",
        "while (!cur && cur->val != value) cur = cur->next;",
        "prev = prev->next;",
        "if (cur) newNode->next = cur;",
        "prev->next = newNode;"
    };
    std::vector<std::string> addTailCodes = {
        "AddTail: if (!head) head = new Node(value);",
        "while (cur->next) cur = cur->next;",
        "cur->next = new Node(value);"
    };
    std::vector<std::string> deleteCodes = {
        "Delete: while (cur->next && cur->next->val != value) cur = cur->next;",
        "if (cur->next) Node* tmp = cur->next;",
        "cur->next = cur->next->next;",
        "delete tmp;"
    };
    std::vector<std::string> searchCodes = {
        "Search: while (cur && cur->val != value) cur = cur->next;",
        "if (cur) return true;",
        "return false;"
    };
    std::vector<std::string> updateCodes = {
        "Update: while (cur && cur->val != oldValue) cur = cur->next;",
        "if (cur) cur->val = newValue;"
    };
    
    std::vector<std::string> initDescriptions;
    std::vector<int> initCodeIndex;
    std::vector<std::vector<std::tuple<Vector2, Color>>> initPaths1;
    std::vector<std::vector<std::tuple<Vector2, Color>>> initPaths2;

    std::vector<std::string> addHeadDescriptions;
    std::vector<int> addHeadCodeIndex;
    std::vector<std::vector<std::tuple<Vector2, Color>>> addHeadPaths1;
    std::vector<std::vector<std::tuple<Vector2, Color>>> addHeadPaths2;
    
    std::vector<std::string> addIndexDescriptions;
    std::vector<int> addIndexCodeIndex;
    std::vector<std::vector<std::tuple<Vector2, Color>>> addIndexPaths1;
    std::vector<std::vector<std::tuple<Vector2, Color>>> addIndexPaths2;
    
    std::vector<std::string> addTailDescriptions;
    std::vector<int> addTailCodeIndex;
    std::vector<std::vector<std::tuple<Vector2, Color>>> addTailPaths1;
    std::vector<std::vector<std::tuple<Vector2, Color>>> addTailPaths2;
    
    std::vector<std::string> deleteDescriptions;
    std::vector<int> deleteCodeIndex;
    std::vector<std::vector<std::tuple<Vector2, Color>>> deletePaths1;
    std::vector<std::vector<std::tuple<Vector2, Color>>> deletePaths2;
    
    std::vector<std::string> searchDescriptions;
    std::vector<int> searchCodeIndex;
    std::vector<std::vector<std::tuple<Vector2, Color>>> searchPaths1;
    std::vector<std::vector<std::tuple<Vector2, Color>>> searchPaths2;
    
    std::vector<std::string> updateDescriptions;
    std::vector<int> updateCodeIndex;
    std::vector<std::vector<std::tuple<Vector2, Color>>> updatePaths1;
    std::vector<std::vector<std::tuple<Vector2, Color>>> updatePaths2;
    
    // Animation and description drawing
    void drawInitializeAnimation();
    void drawInitializeDescription();
    void drawAddHeadAnimation();
    void drawAddHeadDescription();
    void drawAddIndexAnimation();
    void drawAddIndexDescription();
    void drawAddTailAnimation();
    void drawAddTailDescription();
    void drawDeleteAnimation();
    void drawDeleteDescription();
    void drawSearchAnimation();
    void drawSearchDescription();
    void drawUpdateAnimation();
    void drawUpdateDescription();
    
    // Code area
    float codePosX = 10;
    float codePosY = 60;
    float codePosSpace = 25;
    
public:
    LinkedList();
    ~LinkedList();
    void Init(int n);
    void AddHead(int value);
    void AddIndex(int value, int index);
    void AddTail(int value);
    void Delete(int value);
    bool Search(int value);
    void Update(int oldValue, int newValue);
    void DrawScreen();
    void handleFileDrop();
    bool IsBackButtonClicked() const { return backButtonClicked; }
};

#endif