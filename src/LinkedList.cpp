#include "LinkedList.h"
#include "Button.h"
#include <vector>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <iomanip>


static char* ftc(float v) { 
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << v / 2;
    std::string str = "x" + ss.str();
    char* cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}

LinkedList::LinkedList() : head(nullptr), selectedNode(nullptr), selectedValue(0) {
    pathfile[0] = '\0';
    fileLoaded = false;
    showUploadPrompt = false;
    prevHead = nullptr;
    operation_type = -1;
    backButtonTexture = LoadTexture("..\\resources\\images\\BackButton.png");
}

LinkedList::~LinkedList() {
    // Giải phóng danh sách chính
    ListNode* current = head;
    while (current) {
        ListNode* temp = current;
        current = current->next;
        delete temp;
    }
    // Giải phóng danh sách trước đó
    current = prevHead;
    while (current) {
        ListNode* temp = current;
        current = current->next;
        delete temp;
    }
    // Giải phóng texture của nút "Back"
    UnloadTexture(backButtonTexture);
}

void LinkedList::saveCurrentList() {
    // Xóa danh sách prevHead cũ
    ListNode* current = prevHead;
    while (current) {
        ListNode* temp = current;
        current = current->next;
        delete temp;
    }
    prevHead = nullptr;

    // Sao chép danh sách hiện tại vào prevHead
    if (!head) return;
    ListNode* currentHead = head;
    prevHead = new ListNode(currentHead->value);
    ListNode* currentPrev = prevHead;
    while (currentHead->next) {
        currentHead = currentHead->next;
        currentPrev->next = new ListNode(currentHead->value);
        currentPrev = currentPrev->next;
        currentPrev->next = nullptr; // Đảm bảo không có con trỏ treo
    }
}

Vector2 LinkedList::calNodeCenter(int idx) {
    float radius = nodeWidth / 2;
    return {listPosX + (nodeWidth + nodeSpaceX) * idx + radius - scrollOffsetX, listPosY + nodeHeight / 2};
}

Rectangle LinkedList::calEdgeArea(int idx) {
    return {
        listPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX - scrollOffsetX,
        listPosY + nodeHeight / 2,
        nodeSpaceX,
        lineWidth
    };
}

Vector2 LinkedList::calEdgeCenter(int idx) {
    float x = listPosX + (nodeWidth + nodeSpaceX) * idx - nodeSpaceX + nodeSpaceX / 2 - scrollOffsetX;
    float y = listPosY + nodeHeight / 2;
    return {x, y};
}

void LinkedList::drawList() {
    ListNode* current = head;
    int idx = 0;
    float radius = nodeWidth / 2;

    // Đếm số lượng node thực tế để xác định vị trí cuối cùng
    int nodeCount = 0;
    ListNode* temp = head;
    while (temp) {
        nodeCount++;
        temp = temp->next;
    }
    maxScrollOffsetX = std::max(0.0f, (nodeWidth + nodeSpaceX) * nodeCount - (1600 - listPosX - 50));

    if (!current) {
        Vector2 nullCenter = calNodeCenter(0);
        float dx = GetMousePosition().x - nullCenter.x;
        float dy = GetMousePosition().y - nullCenter.y;
        bool isHovered = (dx * dx + dy * dy) <= (radius * radius);
        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedNode = nullptr;
        }
        DrawCircleV(nullCenter, radius, isHovered ? GRAY : LIGHTGRAY);
        DrawCircleLines(nullCenter.x, nullCenter.y, radius, BLACK);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), "NULL", 20, 1);
        DrawTextEx(GetFontDefault(), "NULL", {nullCenter.x - textSize.x / 2, nullCenter.y - textSize.y / 2}, 20, 1, BLACK);
        return;
    }
    
    while (current && idx < nodeCount) {
        Vector2 nodeCenter = calNodeCenter(idx);
        float dx = GetMousePosition().x - nodeCenter.x;
        float dy = GetMousePosition().y - nodeCenter.y;
        bool isHovered = (dx * dx + dy * dy) <= (radius * radius);
        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedNode = current;
            selectedValue = current->value;
            selectedNodeArea = {nodeCenter.x - radius, nodeCenter.y - radius, nodeWidth, nodeHeight};
        }
        bool isSelected = (selectedNode == current);
        DrawCircleV(nodeCenter, radius, isSelected ? BLUE : (isHovered ? GRAY : LIGHTGRAY));
        DrawCircleLines(nodeCenter.x, nodeCenter.y, radius, BLACK);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), TextFormat("%d", current->value), 20, 1);
        DrawTextEx(GetFontDefault(), TextFormat("%d", current->value), {nodeCenter.x - textSize.x / 2, nodeCenter.y - textSize.y / 2}, 20, 1, BLACK);
        
        if (current->next && idx < nodeCount - 1) {
            Rectangle edgeRect = calEdgeArea(idx + 1);
            DrawRectangleRec(edgeRect, BLACK);
        }
        
        current = current->next;
        idx++;
    }

    // Chỉ vẽ node "NULL" ở vị trí cuối cùng dựa trên nodeCount
    Vector2 nullCenter = calNodeCenter(nodeCount);
    float dxNull = GetMousePosition().x - nullCenter.x;
    float dyNull = GetMousePosition().y - nullCenter.y;
    bool isHoveredNull = (dxNull * dxNull + dyNull * dyNull) <= (radius * radius);
    if (isHoveredNull && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedNode = nullptr;
    }
    DrawCircleV(nullCenter, radius, isHoveredNull ? GRAY : LIGHTGRAY);
    DrawCircleLines(nullCenter.x, nullCenter.y, radius, BLACK);
    Vector2 textSize = MeasureTextEx(GetFontDefault(), "NULL", 20, 1);
    DrawTextEx(GetFontDefault(), "NULL", {nullCenter.x - textSize.x / 2, nullCenter.y - textSize.y / 2}, 20, 1, BLACK);
}

void LinkedList::drawPrevList() {
    ListNode* current = prevHead;
    int idx = 0;
    float radius = nodeWidth / 2;

    if (!current) {
        Vector2 nullCenter = calNodeCenter(0);
        float dx = GetMousePosition().x - nullCenter.x;
        float dy = GetMousePosition().y - nullCenter.y;
        bool isHovered = (dx * dx + dy * dy) <= (radius * radius);
        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedNode = nullptr;
        }
        DrawCircleV(nullCenter, radius, isHovered ? GRAY : LIGHTGRAY);
        DrawCircleLines(nullCenter.x, nullCenter.y, radius, BLACK);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), "NULL", 20, 1);
        DrawTextEx(GetFontDefault(), "NULL", {nullCenter.x - textSize.x / 2, nullCenter.y - textSize.y / 2}, 20, 1, BLACK);
        return;
    }
    
    while (current) {
        Vector2 nodeCenter = calNodeCenter(idx);
        float dx = GetMousePosition().x - nodeCenter.x;
        float dy = GetMousePosition().y - nodeCenter.y;
        bool isHovered = (dx * dx + dy * dy) <= (radius * radius);
        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedNode = current;
            selectedValue = current->value;
            selectedNodeArea = {nodeCenter.x - radius, nodeCenter.y - radius, nodeWidth, nodeHeight};
        }
        bool isSelected = (selectedNode == current);
        DrawCircleV(nodeCenter, radius, isSelected ? BLUE : (isHovered ? GRAY : LIGHTGRAY));
        DrawCircleLines(nodeCenter.x, nodeCenter.y, radius, BLACK);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), TextFormat("%d", current->value), 20, 1);
        DrawTextEx(GetFontDefault(), TextFormat("%d", current->value), {nodeCenter.x - textSize.x / 2, nodeCenter.y - textSize.y / 2}, 20, 1, BLACK);
        
        if (current->next) {
            Rectangle edgeRect = calEdgeArea(idx + 1);
            DrawRectangleRec(edgeRect, BLACK);
        }
        
        current = current->next;
        idx++;
    }
    
    Vector2 nullCenter = calNodeCenter(idx);
    float dxNull = GetMousePosition().x - nullCenter.x;
    float dyNull = GetMousePosition().y - nullCenter.y;
    bool isHoveredNull = (dxNull * dxNull + dyNull * dyNull) <= (radius * radius);
    if (isHoveredNull && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedNode = nullptr;
    }
    DrawCircleV(nullCenter, radius, isHoveredNull ? GRAY : LIGHTGRAY);
    DrawCircleLines(nullCenter.x, nullCenter.y, radius, BLACK);
    Vector2 textSize = MeasureTextEx(GetFontDefault(), "NULL", 20, 1);
    DrawTextEx(GetFontDefault(), "NULL", {nullCenter.x - textSize.x / 2, nullCenter.y - textSize.y / 2}, 20, 1, BLACK);
}

void LinkedList::Init(int n) {
    // Xóa danh sách cũ
    ListNode* current = head;
    while (current) {
        ListNode* temp = current;
        current = current->next;
        delete temp;
    }
    head = nullptr;

    // Xóa danh sách prevHead để tránh trạng thái sai
    saveCurrentList(); // Gọi saveCurrentList để đảm bảo prevHead được làm mới

    initDescriptions.clear();
    initCodeIndex.clear();
    initPaths1.clear();
    initPaths2.clear();
    
    initDescriptions.push_back("Clearing list");
    initCodeIndex.push_back(0);
    initPaths1.push_back({});
    initPaths2.push_back({});

    // Tạo danh sách mới với n node
    if (n <= 0) {
        head = nullptr;
    } else {
        ListNode* curr = nullptr;
        for (int i = 0; i < n; i++) {
            int value = rand() % 100;
            ListNode* newNode = new ListNode(value);
            newNode->next = nullptr; // Đảm bảo next được khởi tạo đúng
            if (!head) {
                head = newNode;
                curr = head;
            } else {
                curr->next = newNode;
                curr = curr->next;
            }
        }
    }
    
    initDescriptions.push_back("Initialization complete with " + std::to_string(n) + " nodes");
    initCodeIndex.push_back(2);
    initPaths1.push_back({});
    initPaths2.push_back({});
    
    curStep = 0;
    totalStep = initCodeIndex.size();
    doneAnimation = true;
    operation_type = 6;
    selectedNode = nullptr;
}
void LinkedList::AddHead(int value) {
    selectedNode = nullptr;
    saveCurrentList();
    
    addHeadDescriptions.clear();
    addHeadCodeIndex.clear();
    addHeadPaths1.clear();
    addHeadPaths2.clear();
    
    addHeadDescriptions.push_back("Begin adding value " + std::to_string(value) + " to head");
    addHeadCodeIndex.push_back(0);
    addHeadPaths1.push_back({});
    addHeadPaths2.push_back({});
    
    ListNode* newNode = new ListNode(value);
    addHeadDescriptions.push_back("Create new node with value: " + std::to_string(value));
    addHeadCodeIndex.push_back(0);
    addHeadPaths1.push_back({});
    addHeadPaths2.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
    
    newNode->next = head;
    addHeadDescriptions.push_back("Set newNode->next to head");
    addHeadCodeIndex.push_back(1);
    addHeadPaths1.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
    addHeadPaths2.push_back({std::make_tuple(calEdgeCenter(1), GREEN)});
    
    head = newNode;
    addHeadDescriptions.push_back("Set head to newNode");
    addHeadCodeIndex.push_back(2);
    addHeadPaths1.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
    addHeadPaths2.push_back({});
    
    addHeadDescriptions.push_back("Done!");
    addHeadCodeIndex.push_back(2);
    addHeadPaths1.push_back({});
    addHeadPaths2.push_back({});
    
    curStep = 0;
    totalStep = addHeadCodeIndex.size();
    doneAnimation = false;
    operation_type = 1; // AddHead
}

void LinkedList::AddIndex(int value, int index) {
    selectedNode = nullptr;
    saveCurrentList();
    
    addIndexDescriptions.clear();
    addIndexCodeIndex.clear();
    addIndexPaths1.clear();
    addIndexPaths2.clear();
    
    addIndexDescriptions.push_back("Begin adding value " + std::to_string(value) + " at index " + std::to_string(index));
    addIndexCodeIndex.push_back(0);
    addIndexPaths1.push_back({});
    addIndexPaths2.push_back({});
    
    ListNode* newNode = new ListNode(value);
    addIndexDescriptions.push_back("Create new node with value: " + std::to_string(value));
    addIndexCodeIndex.push_back(0);
    addIndexPaths1.push_back({});
    addIndexPaths2.push_back({std::make_tuple(calNodeCenter(index), GREEN)});
    
    if (!head || index <= 0) {
        newNode->next = head;
        head = newNode;
        addIndexDescriptions.push_back("Index <= 0 or empty list, add to head");
        addIndexCodeIndex.push_back(1);
        addIndexPaths1.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
        addIndexPaths2.push_back({std::make_tuple(calEdgeCenter(1), GREEN)});
        
        addIndexDescriptions.push_back("Done!");
        addIndexCodeIndex.push_back(4);
        addIndexPaths1.push_back({});
        addIndexPaths2.push_back({});
        
        curStep = 0;
        totalStep = addIndexCodeIndex.size();
        doneAnimation = false;
        operation_type = 2; // AddIndex
        return;
    }
    
    ListNode* curr = head;
    ListNode* prev = nullptr;
    int idx = 0;
    
    addIndexDescriptions.push_back("Traverse to index " + std::to_string(index));
    addIndexCodeIndex.push_back(1);
    addIndexPaths1.push_back({});
    addIndexPaths2.push_back({std::make_tuple(calNodeCenter(0), BLUE)});
    
    while (curr && idx < index) {
        addIndexDescriptions.push_back("Move to next node");
        addIndexCodeIndex.push_back(1);
        addIndexPaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
        addIndexPaths2.push_back({std::make_tuple(calNodeCenter(idx + 1), BLUE)});
        
        prev = curr;
        curr = curr->next;
        idx++;
    }
    
    if (!curr) {
        // Nếu vượt quá danh sách, chèn vào cuối
        if (prev) {
            prev->next = newNode;
            addIndexDescriptions.push_back("Index exceeds list, append to end");
            addIndexCodeIndex.push_back(4);
            addIndexPaths1.push_back({std::make_tuple(calNodeCenter(idx - 1), BLUE)});
            addIndexPaths2.push_back({std::make_tuple(calNodeCenter(idx), GREEN)});
        } else {
            head = newNode;
            addIndexDescriptions.push_back("Empty list, set head");
            addIndexCodeIndex.push_back(4);
            addIndexPaths1.push_back({});
            addIndexPaths2.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
        }
    } else {
        newNode->next = curr;
        if (prev) {
            prev->next = newNode;
            addIndexDescriptions.push_back("Insert at index " + std::to_string(index));
            addIndexCodeIndex.push_back(3);
            addIndexPaths1.push_back({std::make_tuple(calNodeCenter(idx - 1), BLUE)});
            addIndexPaths2.push_back({std::make_tuple(calNodeCenter(idx), GREEN)});
            
            addIndexDescriptions.push_back("Link previous to new node");
            addIndexCodeIndex.push_back(4);
            addIndexPaths1.push_back({std::make_tuple(calNodeCenter(idx - 1), BLUE), std::make_tuple(calNodeCenter(idx), GREEN)});
            addIndexPaths2.push_back({std::make_tuple(calEdgeCenter(idx), GREEN)});
        } else {
            head = newNode;
            addIndexDescriptions.push_back("Insert at head");
            addIndexCodeIndex.push_back(4);
            addIndexPaths1.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
            addIndexPaths2.push_back({std::make_tuple(calEdgeCenter(1), GREEN)});
        }
    }
    
    addIndexDescriptions.push_back("Done!");
    addIndexCodeIndex.push_back(4);
    addIndexPaths1.push_back({});
    addIndexPaths2.push_back({});
    
    curStep = 0;
    totalStep = addIndexCodeIndex.size();
    doneAnimation = false;
    operation_type = 2; // AddIndex
}

void LinkedList::AddTail(int value) {
    selectedNode = nullptr;
    saveCurrentList();
    
    addTailDescriptions.clear();
    addTailCodeIndex.clear();
    addTailPaths1.clear();
    addTailPaths2.clear();
    
    addTailDescriptions.push_back("Begin adding value " + std::to_string(value) + " to tail");
    addTailCodeIndex.push_back(0);
    addTailPaths1.push_back({});
    addTailPaths2.push_back({});
    
    ListNode* newNode = new ListNode(value);
    addTailDescriptions.push_back("Create new node with value: " + std::to_string(value));
    addTailCodeIndex.push_back(0);
    addTailPaths1.push_back({});
    
    if (!head) {
        head = newNode;
        addTailDescriptions.push_back("Empty list, set head to new node");
        addTailCodeIndex.push_back(0);
        addTailPaths1.push_back({});
        addTailPaths2.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
        
        addTailDescriptions.push_back("Done!");
        addTailCodeIndex.push_back(2);
        addTailPaths1.push_back({});
        addTailPaths2.push_back({});
        
        curStep = 0;
        totalStep = addTailCodeIndex.size();
        doneAnimation = false;
        operation_type = 3; // AddTail
        return;
    }
    
    ListNode* curr = head;
    int idx = 0;
    addTailDescriptions.push_back("Traverse to tail");
    addTailCodeIndex.push_back(1);
    addTailPaths1.push_back({});
    addTailPaths2.push_back({std::make_tuple(calNodeCenter(0), BLUE)});
    
    while (curr->next) {
        curr = curr->next;
        idx++;
        addTailDescriptions.push_back("Move to next node");
        addTailCodeIndex.push_back(1);
        addTailPaths1.push_back({std::make_tuple(calNodeCenter(idx - 1), BLUE)});
        addTailPaths2.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
    }
    
    curr->next = newNode;
    addTailDescriptions.push_back("Append new node to tail");
    addTailCodeIndex.push_back(2);
    addTailPaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
    addTailPaths2.push_back({std::make_tuple(calNodeCenter(idx + 1), GREEN)});
    
    addTailDescriptions.push_back("Done!");
    addTailCodeIndex.push_back(2);
    addTailPaths1.push_back({});
    addTailPaths2.push_back({});
    
    curStep = 0;
    totalStep = addTailCodeIndex.size();
    doneAnimation = false;
    operation_type = 3; // AddTail
}

void LinkedList::Delete(int value) {
    selectedNode = nullptr;
    saveCurrentList();
    
    deleteDescriptions.clear();
    deleteCodeIndex.clear();
    deletePaths1.clear();
    deletePaths2.clear();
    
    deleteDescriptions.push_back("Begin deleting value: " + std::to_string(value));
    deleteCodeIndex.push_back(0);
    deletePaths1.push_back({});
    deletePaths2.push_back({});
    
    if (!head) {
        deleteDescriptions.push_back("List is empty");
        deleteCodeIndex.push_back(0);
        deletePaths1.push_back({});
        deletePaths2.push_back({std::make_tuple(calNodeCenter(0), RED)});
        
        deleteDescriptions.push_back("Done!");
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({});
        deletePaths2.push_back({});
        
        curStep = 0;
        totalStep = deleteCodeIndex.size();
        operation_type = 4; // Delete
        doneAnimation = false;
        return;
    }
    
    if (head->value == value) {
        ListNode* temp = head;
        head = head->next;
        deleteDescriptions.push_back("Value found at head, update head");
        deleteCodeIndex.push_back(1);
        deletePaths1.push_back({std::make_tuple(calNodeCenter(0), GREEN)});
        deletePaths2.push_back({std::make_tuple(calNodeCenter(1), BLUE)});
        
        delete temp;
        deleteDescriptions.push_back("Delete head node");
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({std::make_tuple(calNodeCenter(1), BLUE)});
        deletePaths2.push_back({std::make_tuple(calNodeCenter(0), WHITE)});
        
        deleteDescriptions.push_back("Done!");
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({});
        deletePaths2.push_back({});
        
        curStep = 0;
        totalStep = deleteCodeIndex.size();
        operation_type = 4; // Delete
        doneAnimation = false;
        return;
    }
    
    ListNode* curr = head;
    int idx = 0;
    deleteDescriptions.push_back("Traverse to find value");
    deleteCodeIndex.push_back(0);
    deletePaths1.push_back({});
    deletePaths2.push_back({std::make_tuple(calNodeCenter(0), BLUE)});
    
    while (curr->next && curr->next->value != value) {
        curr = curr->next;
        idx++;
        deleteDescriptions.push_back("Move to next node");
        deleteCodeIndex.push_back(0);
        deletePaths1.push_back({std::make_tuple(calNodeCenter(idx - 1), BLUE)});
        deletePaths2.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
    }
    
    if (curr->next) {
        ListNode* temp = curr->next;
        curr->next = curr->next->next;
        deleteDescriptions.push_back("Found value, update next pointer");
        deleteCodeIndex.push_back(1);
        deletePaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
        deletePaths2.push_back({std::make_tuple(calNodeCenter(idx + 1), GREEN)});
        
        deleteDescriptions.push_back("Delete node");
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE), std::make_tuple(calNodeCenter(idx + 2), BLUE)});
        deletePaths2.push_back({std::make_tuple(calNodeCenter(idx + 1), WHITE)});
        
        delete temp;
    } else {
        deleteDescriptions.push_back("Value not found");
        deleteCodeIndex.push_back(3);
        deletePaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
        deletePaths2.push_back({std::make_tuple(calNodeCenter(idx + 1), RED)});
    }
    
    deleteDescriptions.push_back("Done!");
    deleteCodeIndex.push_back(3);
    deletePaths1.push_back({});
    deletePaths2.push_back({});
    
    curStep = 0;
    totalStep = deleteCodeIndex.size();
    operation_type = 4; // Delete
    doneAnimation = false;
}

bool LinkedList::Search(int value) {
    selectedNode = nullptr;
    saveCurrentList();
    
    searchDescriptions.clear();
    searchCodeIndex.clear();
    searchPaths1.clear();
    searchPaths2.clear();
    
    searchDescriptions.push_back("Begin searching value: " + std::to_string(value));
    searchCodeIndex.push_back(0);
    searchPaths1.push_back({});
    searchPaths2.push_back({});
    
    ListNode* curr = head;
    int idx = 0;
    searchDescriptions.push_back("Start at head");
    searchCodeIndex.push_back(0);
    searchPaths1.push_back({});
    searchPaths2.push_back({std::make_tuple(calNodeCenter(0), BLUE)});
    
    while (curr && curr->value != value) {
        curr = curr->next;
        idx++;
        searchDescriptions.push_back("Move to next node");
        searchCodeIndex.push_back(0);
        searchPaths1.push_back({std::make_tuple(calNodeCenter(idx - 1), BLUE)});
        searchPaths2.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
    }
    
    if (curr) {
        searchDescriptions.push_back("Value found");
        searchCodeIndex.push_back(1);
        searchPaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
        searchPaths2.push_back({std::make_tuple(calNodeCenter(idx), GREEN)});
    } else {
        searchDescriptions.push_back("Value not found");
        searchCodeIndex.push_back(2);
        searchPaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
        searchPaths2.push_back({std::make_tuple(calNodeCenter(idx), RED)});
    }
    
    searchDescriptions.push_back("Done!");
    searchCodeIndex.push_back(2);
    searchPaths1.push_back({});
    searchPaths2.push_back({});
    
    curStep = 0;
    totalStep = searchCodeIndex.size();
    doneAnimation = false;
    operation_type = 5; // Search
    return curr != nullptr;
}

void LinkedList::Update(int oldValue, int newValue) {
    selectedNode = nullptr;
    saveCurrentList();
    
    updateDescriptions.clear();
    updateCodeIndex.clear();
    updatePaths1.clear();
    updatePaths2.clear();
    
    updateDescriptions.push_back("Begin updating value " + std::to_string(oldValue) + " to " + std::to_string(newValue));
    updateCodeIndex.push_back(0);
    updatePaths1.push_back({});
    updatePaths2.push_back({});
    
    ListNode* curr = head;
    int idx = 0;
    updateDescriptions.push_back("Start at head");
    updateCodeIndex.push_back(0);
    updatePaths1.push_back({});
    updatePaths2.push_back({std::make_tuple(calNodeCenter(0), BLUE)});
    
    while (curr && curr->value != oldValue) {
        curr = curr->next;
        idx++;
        updateDescriptions.push_back("Move to next node");
        updateCodeIndex.push_back(0);
        updatePaths1.push_back({std::make_tuple(calNodeCenter(idx - 1), BLUE)});
        updatePaths2.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
    }
    
    if (curr) {
        curr->value = newValue;
        updateDescriptions.push_back("Value found, update to " + std::to_string(newValue));
        updateCodeIndex.push_back(1);
        updatePaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
        updatePaths2.push_back({std::make_tuple(calNodeCenter(idx), GREEN)});
    } else {
        updateDescriptions.push_back("Value not found");
        updateCodeIndex.push_back(1);
        updatePaths1.push_back({std::make_tuple(calNodeCenter(idx), BLUE)});
        updatePaths2.push_back({std::make_tuple(calNodeCenter(idx), RED)});
    }
    
    updateDescriptions.push_back("Done!");
    updateCodeIndex.push_back(1);
    updatePaths1.push_back({});
    updatePaths2.push_back({});
    
    curStep = 0;
    totalStep = updateCodeIndex.size();
    doneAnimation = false;
    operation_type = 7; // Update
}

void LinkedList::drawInitializeAnimation() {
    drawList();
}

void LinkedList::drawAddHeadAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }

    // vẽ các node đã có trc đó
    drawPrevList();
    float radius = nodeWidth / 2;
    for (int i = 0; i < done; i++) {// vòng loop này đang vẽ các đường đi của các node
        auto path = addHeadPaths2[curStep][i];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        if (center.y == listPosY + nodeHeight / 2) { // Node
            DrawCircleV(center, radius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, radius, color);
        } else { // Edge
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
        }
    }

    // bắt đầu vẽ node mới
    if (curStep < totalStep - 1 && curStep < addHeadPaths2.size() && done == addHeadPaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    if (done < addHeadPaths2[curStep].size()) {
        auto path = addHeadPaths2[curStep][done];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        delta += speed;
        if (center.y == listPosY + nodeHeight / 2) { // Node
            float currentRadius = std::min(delta / (2 * radius) * radius, radius);
            DrawCircleV(center, currentRadius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, currentRadius, color);
            if (currentRadius >= radius) {
                done++;
                delta = 0;
            }
        } else { // Edge
            float currentWidth = std::min(delta, nodeSpaceX);
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, Fade(color, 0.2f));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, color);
            if (currentWidth >= nodeSpaceX) {
                done++;
                delta = 0;
            }
        }
    }
}

void LinkedList::drawAddIndexAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    drawPrevList();
    float radius = nodeWidth / 2;;
    for (int i = 0; i < done; i++) {
        auto path = addIndexPaths2[curStep][i];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        if (center.y == listPosY + nodeHeight / 2) { // Node
            DrawCircleV(center, radius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, radius, color);
        } else { // Edge
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
        }
    }
    if (curStep < totalStep - 1 && curStep < addIndexPaths2.size() && done == addIndexPaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    if (done < addIndexPaths2[curStep].size()) {
        auto path = addIndexPaths2[curStep][done];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        delta += speed;
        if (center.y == listPosY + nodeHeight / 2) { // Node
            float currentRadius = std::min(delta / (2 * radius) * radius, radius);
            DrawCircleV(center, currentRadius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, currentRadius, color);
            if (currentRadius >= radius) {
                done++;
                delta = 0;
            }
        } else { // Edge
            float currentWidth = std::min(delta, nodeSpaceX);
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, Fade(color, 0.2f));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, color);
            if (currentWidth >= nodeSpaceX) {
                done++;
                delta = 0;
            }
        }
    }
}

void LinkedList::drawAddTailAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    drawPrevList();
    float radius = nodeWidth / 2;;
    for (int i = 0; i < done; i++) {
        auto path = addTailPaths2[curStep][i];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        if (center.y == listPosY + nodeHeight / 2) { // Node
            DrawCircleV(center, radius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, radius, color);
        } else { // Edge
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
        }
    }
    if (curStep < totalStep - 1 && curStep < addTailPaths2.size() && done == addTailPaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    if (done < addTailPaths2[curStep].size()) {
        auto path = addTailPaths2[curStep][done];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        delta += speed;
        if (center.y == listPosY + nodeHeight / 2) { // Node
            float currentRadius = std::min(delta / (2 * radius) * radius, radius);
            DrawCircleV(center, currentRadius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, currentRadius, color);
            if (currentRadius >= radius) {
                done++;
                delta = 0;
            }
        } else { // Edge
            float currentWidth = std::min(delta, nodeSpaceX);
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, Fade(color, 0.2f));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, color);
            if (currentWidth >= nodeSpaceX) {
                done++;
                delta = 0;
            }
        }
    }
}

void LinkedList::drawDeleteAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    drawPrevList();
    float radius = nodeWidth / 2;;
    for (int i = 0; i < done; i++) {
        auto path = deletePaths2[curStep][i];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        float fade = (color.r == 255 && color.g == 255 && color.b == 255) ? 1.0f : 0.2f;
        if (center.y == listPosY + nodeHeight / 2) { // Node
            DrawCircleV(center, radius, Fade(color, fade));
            DrawCircleLines(center.x, center.y, radius, color);
        } else { // Edge
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, fade));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
        }
    }
    if (curStep < totalStep - 1 && curStep < deletePaths2.size() && done == deletePaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    if (done < deletePaths2[curStep].size()) {
        auto path = deletePaths2[curStep][done];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        float fade = (color.r == 255 && color.g == 255 && color.b == 255) ? 1.0f : 0.2f;
        delta += speed;
        if (center.y == listPosY + nodeHeight / 2) { // Node
            float currentRadius = std::min(delta / (2 * radius) * radius, radius);
            DrawCircleV(center, currentRadius, Fade(color, fade));
            DrawCircleLines(center.x, center.y, currentRadius, color);
            if (currentRadius >= radius) {
                done++;
                delta = 0;
            }
        } else { // Edge
            float currentWidth = std::min(delta, nodeSpaceX);
            DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, Fade(color, fade));
            DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, currentWidth, lineWidth, color);
            if (currentWidth >= nodeSpaceX) {
                done++;
                delta = 0;
            }
        }
    }
}

void LinkedList::drawSearchAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    drawPrevList();
    float radius = nodeWidth / 2;;
    for (int i = 0; i < done; i++) {
        auto path = searchPaths2[curStep][i];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        DrawCircleV(center, radius, Fade(color, 0.2f));
        DrawCircleLines(center.x, center.y, radius, color);
    }
    if (curStep < totalStep - 1 && curStep < searchPaths2.size() && done == searchPaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    if (done < searchPaths2[curStep].size()) {
        auto path = searchPaths2[curStep][done];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        delta += speed;
        float currentRadius = std::min(delta / (2 * radius) * radius, radius);
        DrawCircleV(center, currentRadius, Fade(color, 0.2f));
        DrawCircleLines(center.x, center.y, currentRadius, color);
        if (currentRadius >= radius) {
            done++;
            delta = 0;
        }
    }
}

void LinkedList::drawUpdateAnimation() {
    if (pause) {
        delta = 0;
        return;
    }
    if (curStep == totalStep - 1 && totalStep > 0) {
        doneAnimation = true;
        return;
    }
    drawPrevList();
    float radius = nodeWidth / 2;;
    for (int i = 0; i < done; i++) {
        auto path = updatePaths2[curStep][i];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        DrawCircleV(center, radius, Fade(color, 0.2f));
        DrawCircleLines(center.x, center.y, radius, color);
    }
    if (curStep < totalStep - 1 && curStep < updatePaths2.size() && done == updatePaths2[curStep].size()) {
        curStep++;
        done = 0;
        delta = 0;
        return;
    }
    if (done < updatePaths2[curStep].size()) {
        auto path = updatePaths2[curStep][done];
        Vector2 center = std::get<0>(path);
        Color color = std::get<1>(path);
        delta += speed;
        float currentRadius = std::min(delta / (2 * radius) * radius, radius);
        DrawCircleV(center, currentRadius, Fade(color, 0.2f));
        DrawCircleLines(center.x, center.y, currentRadius, color);
        if (currentRadius >= radius) {
            done++;
            delta = 0;
        }
    }
}

void LinkedList::drawOperationMenu() {
    static bool showInitializeOption = false;
    static bool showAddOption = false; // Hiển thị menu con của Add
    static bool showAddHeadOption = false;
    static bool showAddIndexOption = false;
    static bool showAddTailOption = false;
    static bool showDeleteOption = false;
    static bool showSearchOption = false;
    static bool showUpdateOption = false;

    float opPosX = 30;
    float opPosY = 700;
    float opWidth = 80;
    float opHeight = 30;

    static bool initClicked = false, addClicked = false;
    static bool addHeadClicked = false, addIndexClicked = false, addTailClicked = false;
    static bool deleteClicked = false, searchClicked = false, updateClicked = false;
    static const char* initMessage = "", *addMessage = "";
    static const char* addHeadMessage = "", *addIndexMessage = "", *addTailMessage = "";
    static const char* deleteMessage = "", *searchMessage = "", *updateMessage = "";

    // Nút Initialize
    Rectangle initializeButtonPos = {opPosX, opPosY, opWidth, opHeight};
    if (DrawButton("Init", initializeButtonPos, GetFontDefault(), initClicked, initMessage)) {
        showInitializeOption = !showInitializeOption;
        showAddOption = false; // Ẩn menu Add nếu đang mở
        showAddHeadOption = false;
        showAddIndexOption = false;
        showAddTailOption = false;
        showDeleteOption = false;
        showSearchOption = false;
        showUpdateOption = false;
        initClicked = false;
    }
    if (showInitializeOption) {
        drawInitializeOptions();
    }

    // Nút Add (hiển thị menu con)
    Rectangle addButtonPos = {opPosX + (opWidth + 20), opPosY, opWidth, opHeight};
    if (DrawButton("Add", addButtonPos, GetFontDefault(), addClicked, addMessage)) {
        showAddOption = !showAddOption;
        showInitializeOption = false;
        showAddHeadOption = false;
        showAddIndexOption = false;
        showAddTailOption = false;
        showDeleteOption = false;
        showSearchOption = false;
        showUpdateOption = false;
        addClicked = false;
    }

    // Menu con của Add
    if (showAddOption) {
        float subMenuPosX = opPosX + (opWidth + 20);
        float subMenuPosY = opPosY - opHeight; // Đặt ngay trên nút Add
        float subMenuWidth = opWidth;
        float subMenuHeight = opHeight;

        // Đảm bảo các nút trong menu con không bị chồng lấp và có thể nhận sự kiện click
        Rectangle addHeadOptionPos = {subMenuPosX, subMenuPosY - opHeight, subMenuWidth, subMenuHeight};
        Rectangle addIndexOptionPos = {subMenuPosX, subMenuPosY - 2 * opHeight, subMenuWidth, subMenuHeight};
        Rectangle addTailOptionPos = {subMenuPosX, subMenuPosY - 3 * opHeight, subMenuWidth, subMenuHeight};

        // Kiểm tra và xử lý nút Add Head
        if (DrawButton("Add Head", addHeadOptionPos, GetFontDefault(), addHeadClicked, addHeadMessage)) {
            showAddHeadOption = true;
            showAddIndexOption = false;
            showAddTailOption = false;
            showAddOption = false;
            addHeadClicked = false;
        }

        // Kiểm tra và xử lý nút Add Index
        if (DrawButton("Add Index", addIndexOptionPos, GetFontDefault(), addIndexClicked, addIndexMessage)) {
            showAddIndexOption = true;
            showAddHeadOption = false;
            showAddTailOption = false;
            showAddOption = false;
            addIndexClicked = false;
        }

        // Kiểm tra và xử lý nút Add Tail
        if (DrawButton("Add Tail", addTailOptionPos, GetFontDefault(), addTailClicked, addTailMessage)) {
            showAddTailOption = true;
            showAddHeadOption = false;
            showAddIndexOption = false;
            showAddOption = false;
            addTailClicked = false;
        }

        // Đóng menu con nếu nhấp ra ngoài
        Vector2 mousePoint = GetMousePosition();
        Rectangle subMenuArea = {subMenuPosX, subMenuPosY - 3 * opHeight, subMenuWidth, 3 * opHeight};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePoint, subMenuArea) && !CheckCollisionPointRec(mousePoint, addButtonPos)) {
            showAddOption = false;
        }
    }

    // Hiển thị giao diện nhập liệu tương ứng
    if (showAddHeadOption) {
        drawAddHeadOptions();
    }
    if (showAddIndexOption) {
        drawAddIndexOptions();
    }
    if (showAddTailOption) {
        drawAddTailOptions();
    }

    // Nút Delete
    Rectangle deleteButtonPos = {opPosX + 2 * (opWidth + 20), opPosY, opWidth, opHeight};
    if (DrawButton("Delete", deleteButtonPos, GetFontDefault(), deleteClicked, deleteMessage)) {
        showDeleteOption = !showDeleteOption;
        showInitializeOption = false;
        showAddOption = false;
        showAddHeadOption = false;
        showAddIndexOption = false;
        showAddTailOption = false;
        showSearchOption = false;
        showUpdateOption = false;
        deleteClicked = false;
    }
    if (showDeleteOption) {
        drawDeleteOptions();
    }

    // Nút Search
    Rectangle searchButtonPos = {opPosX + 3 * (opWidth + 20), opPosY, opWidth, opHeight};
    if (DrawButton("Search", searchButtonPos, GetFontDefault(), searchClicked, searchMessage)) {
        showSearchOption = !showSearchOption;
        showInitializeOption = false;
        showAddOption = false;
        showAddHeadOption = false;
        showAddIndexOption = false;
        showAddTailOption = false;
        showDeleteOption = false;
        showUpdateOption = false;
        searchClicked = false;
    }
    if (showSearchOption) {
        drawSearchOptions();
    }

    // Nút Update
    Rectangle updateButtonPos = {opPosX + 4 * (opWidth + 20), opPosY, opWidth, opHeight};
    if (DrawButton("Update", updateButtonPos, GetFontDefault(), updateClicked, updateMessage)) {
        showUpdateOption = !showUpdateOption;
        showInitializeOption = false;
        showAddOption = false;
        showAddHeadOption = false;
        showAddIndexOption = false;
        showAddTailOption = false;
        showDeleteOption = false;
        showSearchOption = false;
        updateClicked = false;
    }
    if (showUpdateOption) {
        drawUpdateOptions();
    }

    // Đóng các menu nếu nhấp ra ngoài
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        Rectangle optionPos = {opPosX, 580, 380, 120}; // Vùng chung cho các menu con

        // Kiểm tra cho menu Initialize
        if (showInitializeOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, initializeButtonPos)) {
            showInitializeOption = false;
        }

        // Kiểm tra cho menu Delete
        if (showDeleteOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, deleteButtonPos)) {
            showDeleteOption = false;
        }

        // Kiểm tra cho menu Search
        if (showSearchOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, searchButtonPos)) {
            showSearchOption = false;
        }

        // Kiểm tra cho menu Update
        if (showUpdateOption && !CheckCollisionPointRec(mousePoint, optionPos) && !CheckCollisionPointRec(mousePoint, updateButtonPos)) {
            showUpdateOption = false;
        }

        // Kiểm tra cho các menu con của Add (Add Head, Add Index, Add Tail)
        if ((showAddHeadOption || showAddIndexOption || showAddTailOption) && !CheckCollisionPointRec(mousePoint, optionPos)) {
            showAddHeadOption = false;
            showAddIndexOption = false;
            showAddTailOption = false;
        }
    }
}

void LinkedList::drawInitializeOptions() {
    static int nValue = 0;
    static char nText[4] = "0";
    static bool nInputEnabled = false;
    static int nMax = 20;
    
    DrawText("N: ", 20, 610, 20, BLACK);
    Rectangle nRect = {50, 610, 100, 25};
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
            nText[0] = '2';
            nText[1] = '0';
            nText[2] = '\0';
        }
    }
    nValue = atoi(nText);
    DrawText(nText, nRect.x + 5, nRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), nRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        nInputEnabled = true;
    }
    
    static bool randomClicked = false, uploadClicked = false, okClicked = false;
    static const char* randomMessage = "", *uploadMessage = "", *okMessage = "";
    
    Rectangle nRandomRect = {160, 610, 25, 25};
    if (DrawButton("?", nRandomRect, GetFontDefault(), randomClicked, randomMessage)) {
        nValue = GetRandomValue(1, 20);
        sprintf(nText, "%d", nValue);
        randomClicked = false;
    }
    
    Rectangle uploadRect = {30, 650, 80, 30};
    if (DrawButton("Upload", uploadRect, GetFontDefault(), uploadClicked, uploadMessage)) {
        showUploadPrompt = true;
        fileLoaded = false;
        uploadClicked = false;
    }
    
    Rectangle okRect = {350, 650, 40, 30};
    if (DrawButton("OK", okRect, GetFontDefault(), okClicked, okMessage)) {
        if (fileLoaded) {
            std::ifstream fin(pathfile);
            if (fin.is_open()) {
                int n;
                fin >> n;
                n = std::min(n, 20);
                while (head) {
                    ListNode* temp = head;
                    head = head->next;
                    delete temp;
                }
                for (int i = 0; i < n && !fin.eof(); i++) {
                    int value;
                    if (fin >> value) {
                        AddTail(value); // Sử dụng AddTail
                    }
                }
                fin.close();
                saveCurrentList();
                curStep = 0;
                done = 0;
                delta = 0;
                doneStep = true;
                doneAnimation = false;
                operation_type = 6; // Init
            } else {
                fileLoaded = false;
                showUploadPrompt = true;
            }
        } else {
            Init(nValue);
        }
        showUploadPrompt = false;
        okClicked = false;
    }
    
    if (showUploadPrompt) {
        DrawText("Drop a file here", 120, 655, 20, GRAY);
    }
}

void LinkedList::drawAddHeadOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;
    
    DrawText("V: ", 20, 610, 20, BLACK);
    Rectangle vRect = {50, 610, 100, 25};
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
    DrawText(valueText, vRect.x + 5, vRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), vRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
    }
    
    static bool randomClicked = false, okClicked = false;
    static const char* randomMessage = "", *okMessage = "";
    
    Rectangle vRandomRect = {160, 610, 25, 25};
    if (DrawButton("?", vRandomRect, GetFontDefault(), randomClicked, randomMessage)) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        randomClicked = false;
    }
    
    Rectangle okRect = {350, 650, 40, 30};
    if (DrawButton("OK", okRect, GetFontDefault(), okClicked, okMessage)) {
        pause = true;
        AddHead(value);
        done = 0;
        delta = 0;
        pause = false;
        okClicked = false;
    }
}

void LinkedList::drawAddIndexOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;
    
    static int index = 0;
    static char indexText[4] = "0";
    static bool indexInputEnabled = false;
    static int indexMax = 99;
    
    DrawText("V: ", 20, 610, 20, BLACK);
    Rectangle vRect = {50, 610, 100, 25};
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
    DrawText(valueText, vRect.x + 5, vRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), vRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
        indexInputEnabled = false;
    }
    
    static bool vRandomClicked = false;
    static const char* vRandomMessage = "";
    Rectangle vRandomRect = {160, 610, 25, 25};
    if (DrawButton("?", vRandomRect, GetFontDefault(), vRandomClicked, vRandomMessage)) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        vRandomClicked = false;
    }
    
    DrawText("Idx: ", 200, 610, 20, BLACK);
    Rectangle idxRect = {230, 610, 100, 25};
    DrawRectangleRec(idxRect, WHITE);
    DrawRectangleLinesEx(idxRect, 1, BLACK);
    if (indexInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) indexInputEnabled = false;
        int k = GetKeyPressed();
        if (k >= 48 && k <= 57 && strlen(indexText) < 3) {
            int len = strlen(indexText);
            indexText[len] = (char)k;
            indexText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(indexText) > 0) {
            indexText[strlen(indexText) - 1] = '\0';
        }
        int listSize = 0;
        ListNode* temp = head;
        while (temp) {
            listSize++;
            temp = temp->next;
        }
        indexMax = listSize;
        if (atoi(indexText) > indexMax) {
            sprintf(indexText, "%d", indexMax);
        }
    }
    index = atoi(indexText);
    DrawText(indexText, idxRect.x + 5, idxRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), idxRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        indexInputEnabled = true;
        valueInputEnabled = false;
    }
    
    static bool idxRandomClicked = false, okClicked = false;
    static const char* idxRandomMessage = "", *okMessage = "";
    
    Rectangle idxRandomRect = {340, 610, 25, 25};
    if (DrawButton("?", idxRandomRect, GetFontDefault(), idxRandomClicked, idxRandomMessage)) {
        int listSize = 0;
        ListNode* temp = head;
        while (temp) {
            listSize++;
            temp = temp->next;
        }
        if (listSize > 0) {
            index = GetRandomValue(0, listSize);
        } else {
            index = 0;
        }
        sprintf(indexText, "%d", index);
        idxRandomClicked = false;
    }
    
    Rectangle okRect = {350, 650, 40, 30};
    if (DrawButton("OK", okRect, GetFontDefault(), okClicked, okMessage)) {
        pause = true;
        AddIndex(value, index);
        done = 0;
        delta = 0;
        pause = false;
        okClicked = false;
    }
}

void LinkedList::drawAddTailOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;
    
    DrawText("V: ", 20, 610, 20, BLACK);
    Rectangle vRect = {50, 610, 100, 25};
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
    DrawText(valueText, vRect.x + 5, vRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), vRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
    }
    
    static bool randomClicked = false, okClicked = false;
    static const char* randomMessage = "", *okMessage = "";
    
    Rectangle vRandomRect = {160, 610, 25, 25};
    if (DrawButton("?", vRandomRect, GetFontDefault(), randomClicked, randomMessage)) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        randomClicked = false;
    }
    
    Rectangle okRect = {350, 650, 40, 30};
    if (DrawButton("OK", okRect, GetFontDefault(), okClicked, okMessage)) {
        pause = true;
        AddTail(value);
        done = 0;
        delta = 0;
        pause = false;
        okClicked = false;
    }
}

void LinkedList::drawDeleteOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;
    
    DrawText("V: ", 20, 610, 20, BLACK);
    Rectangle vRect = {50, 610, 100, 25};
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
    DrawText(valueText, vRect.x + 5, vRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), vRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
    }
    
    static bool randomClicked = false, okClicked = false;
    static const char* randomMessage = "", *okMessage = "";
    
    Rectangle vRandomRect = {160, 610, 25, 25};
    if (DrawButton("?", vRandomRect, GetFontDefault(), randomClicked, randomMessage)) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        randomClicked = false;
    }
    
    Rectangle okRect = {350, 650, 40, 30};
    if (DrawButton("OK", okRect, GetFontDefault(), okClicked, okMessage)) {
        pause = true;
        Delete(value);
        done = 0;
        delta = 0;
        pause = false;
        okClicked = false;
    }
}

void LinkedList::drawSearchOptions() {
    static int value = 0;
    static char valueText[4] = "0";
    static bool valueInputEnabled = false;
    static int valueMax = 99;
    
    DrawText("V: ", 20, 610, 20, BLACK);
    Rectangle vRect = {50, 610, 100, 25};
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
    DrawText(valueText, vRect.x + 5, vRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), vRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        valueInputEnabled = true;
    }
    
    static bool randomClicked = false, okClicked = false;
    static const char* randomMessage = "", *okMessage = "";
    
    Rectangle vRandomRect = {160, 610, 25, 25};
    if (DrawButton("?", vRandomRect, GetFontDefault(), randomClicked, randomMessage)) {
        value = GetRandomValue(0, 99);
        sprintf(valueText, "%d", value);
        randomClicked = false;
    }
    
    Rectangle okRect = {350, 650, 40, 30};
    if (DrawButton("OK", okRect, GetFontDefault(), okClicked, okMessage)) {
        pause = true;
        Search(value);
        done = 0;
        delta = 0;
        pause = false;
        okClicked = false;
    }
}

void LinkedList::drawUpdateOptions() {
    static int oldValue = 0, newValue = 0;
    static char oldValueText[4] = "0", newValueText[4] = "0";
    static bool oldValueInputEnabled = false, newValueInputEnabled = false;
    static int valueMax = 99;
    
    DrawText("Old: ", 20, 610, 20, BLACK);
    Rectangle oldRect = {50, 610, 100, 25};
    DrawRectangleRec(oldRect, WHITE);
    DrawRectangleLinesEx(oldRect, 1, BLACK);
    if (oldValueInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) oldValueInputEnabled = false;
        int k = GetKeyPressed();
        if (k >= 48 && k <= 57 && strlen(oldValueText) < 3) {
            int len = strlen(oldValueText);
            oldValueText[len] = (char)k;
            oldValueText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(oldValueText) > 0) {
            oldValueText[strlen(oldValueText) - 1] = '\0';
        }
        if (atoi(oldValueText) > valueMax) {
            oldValueText[0] = '9';
            oldValueText[1] = '9';
            oldValueText[2] = '\0';
        }
    }
    oldValue = atoi(oldValueText);
    DrawText(oldValueText, oldRect.x + 5, oldRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), oldRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        oldValueInputEnabled = true;
        newValueInputEnabled = false;
    }
    
    static bool oldRandomClicked = false;
    static const char* oldRandomMessage = "";
    Rectangle oldRandomRect = {160, 610, 25, 25};
    if (DrawButton("?", oldRandomRect, GetFontDefault(), oldRandomClicked, oldRandomMessage)) {
        oldValue = GetRandomValue(0, 99);
        sprintf(oldValueText, "%d", oldValue);
        oldRandomClicked = false;
    }
    
    DrawText("New: ", 200, 610, 20, BLACK);
    Rectangle newRect = {230, 610, 100, 25};
    DrawRectangleRec(newRect, WHITE);
    DrawRectangleLinesEx(newRect, 1, BLACK);
    if (newValueInputEnabled) {
        if (IsKeyPressed(KEY_ENTER)) newValueInputEnabled = false;
        int k = GetKeyPressed();
        if (k >= 48 && k <= 57 && strlen(newValueText) < 3) {
            int len = strlen(newValueText);
            newValueText[len] = (char)k;
            newValueText[len + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(newValueText) > 0) {
            newValueText[strlen(newValueText) - 1] = '\0';
        }
        if (atoi(newValueText) > valueMax) {
            newValueText[0] = '9';
            newValueText[1] = '9';
            newValueText[2] = '\0';
        }
    }
    newValue = atoi(newValueText);
    DrawText(newValueText, newRect.x + 5, newRect.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), newRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        newValueInputEnabled = true;
        oldValueInputEnabled = false;
    }
    
    static bool newRandomClicked = false, okClicked = false;
    static const char* newRandomMessage = "", *okMessage = "";
    
    Rectangle newRandomRect = {340, 610, 25, 25};
    if (DrawButton("?", newRandomRect, GetFontDefault(), newRandomClicked, newRandomMessage)) {
        newValue = GetRandomValue(0, 99);
        sprintf(newValueText, "%d", newValue);
        newRandomClicked = false;
    }
    
    Rectangle okRect = {350, 650, 40, 30};
    if (DrawButton("OK", okRect, GetFontDefault(), okClicked, okMessage)) {
        pause = true;
        Update(oldValue, newValue);
        done = 0;
        delta = 0;
        pause = false;
        okClicked = false;
    }
}

void LinkedList::drawNodeDetailMenu() {
    if (selectedNode) {
        DrawRectangleRec(selectedNodeArea, Fade(BLUE, 0.2f));
        
        static char valueText[4] = "0";
        static bool vInputEnabled = false;
        
        DrawText("Node detail:", 1100, 610, 20, BLACK);
        DrawText(TextFormat("- Value: %d", selectedNode->value), 1100, 640, 20, BLACK);
        
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
        
        static bool deleteClicked = false, updateClicked = false;
        static const char* deleteMessage = "", *updateMessage = "";
        
        Rectangle deleteRect = {1100, 700, 80, 30};
        if (DrawButton("Delete", deleteRect, GetFontDefault(), deleteClicked, deleteMessage)) {
            Delete(selectedNode->value);
            done = 0;
            delta = 0;
            selectedNode = nullptr;
            deleteClicked = false;
        }
        
        Rectangle updateRect = {1200, 700, 80, 30};
        if (DrawButton("Update", updateRect, GetFontDefault(), updateClicked, updateMessage)) {
            Update(selectedNode->value, selectedValue);
            done = 0;
            delta = 0;
            selectedNode = nullptr;
            updateClicked = false;
        }
    }
}

void LinkedList::drawAnimationMenu() {
    float buttonWidth = 80;
    float buttonHeight = 30;
    float spacing = 20;
    float sliderWidth = 280;
    float sliderHeight = 20;
    float posX = 600;
    float buttonPosY = 700;
    
    static bool firstClicked = false, prevClicked = false, playPauseClicked = false, nextClicked = false, lastClicked = false;
    static const char* firstMessage = "", *prevMessage = "", *playPauseMessage = "", *nextMessage = "", *lastMessage = "";
    
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
    if (DrawButton("First", firstRect, GetFontDefault(), firstClicked, firstMessage)) {
        resetAnimation();
        pause = true;
        firstClicked = false;
    }
    
    Rectangle prevRect = {posX + buttonWidth + spacing, buttonPosY, buttonWidth, buttonHeight};
    if (DrawButton("Prev", prevRect, GetFontDefault(), prevClicked, prevMessage)) {
        pause = true;
        if (curStep > 0) {
            curStep--;
            done = 0;
            doneStep = true;
            doneAnimation = false;
            delta = 0;
        }
        prevClicked = false;
    }
    
    Rectangle playPauseRect = {posX + 2*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight};
    if (pause) {
        if (DrawButton("Play", playPauseRect, GetFontDefault(), playPauseClicked, playPauseMessage)) {
            pause = false;
            playPauseClicked = false;
        }
    } else {
        if (DrawButton("Pause", playPauseRect, GetFontDefault(), playPauseClicked, playPauseMessage)) {
            pause = true;
            playPauseClicked = false;
        }
    }
    
    Rectangle nextRect = {posX + 3*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight};
    if (DrawButton("Next", nextRect, GetFontDefault(), nextClicked, nextMessage)) {
        pause = true;
        if (curStep < totalStep - 1) {
            curStep++;
            done = 0;
            delta = 0;
        }
        nextClicked = false;
    }
    
    Rectangle lastRect = {posX + 4*(buttonWidth + spacing), buttonPosY, buttonWidth, buttonHeight};
    if (DrawButton("Last", lastRect, GetFontDefault(), lastClicked, lastMessage)) {
        pause = true;
        curStep = totalStep - 1;
        done = 0;
        delta = 0;
        lastClicked = false;
    }
}

void LinkedList::resetAnimation() {
    curStep = 0;
    delta = 0;
    done = 0;
    doneStep = false;
    doneAnimation = false;
}

void LinkedList::drawInitializeDescription() {
    DrawText("Initializing:", 500, 610, 20, BLACK);
    if (initDescriptions.size() > curStep) {
        DrawText(initDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = initCodeIndex.size() > curStep ? initCodeIndex[curStep] : 0;
    for (int i = 0; i < initCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(initCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    // Không cần hiển thị paths vì không có animation
}

void LinkedList::drawAddHeadDescription() {
    DrawText("Adding to Head:", 500, 610, 20, BLACK);
    if (addHeadDescriptions.size() > curStep) {
        DrawText(addHeadDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = addHeadCodeIndex.size() > curStep ? addHeadCodeIndex[curStep] : 0;
    for (int i = 0; i < addHeadCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(addHeadCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    float radius = nodeWidth / 2;;
    if (addHeadPaths1.size() > curStep) {
        for (size_t i = 0; i < addHeadPaths1[curStep].size(); i++) {
            Vector2 center = std::get<0>(addHeadPaths1[curStep][i]);
            Color color = std::get<1>(addHeadPaths1[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
    if (pause && addHeadPaths2.size() > curStep) {
        for (size_t i = 0; i < addHeadPaths2[curStep].size(); i++) {
            Vector2 center = std::get<0>(addHeadPaths2[curStep][i]);
            Color color = std::get<1>(addHeadPaths2[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
}

void LinkedList::drawAddIndexDescription() {
    DrawText("Adding at Index:", 500, 610, 20, BLACK);
    if (addIndexDescriptions.size() > curStep) {
        DrawText(addIndexDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = addIndexCodeIndex.size() > curStep ? addIndexCodeIndex[curStep] : 0;
    for (int i = 0; i < addIndexCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(addIndexCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    float radius = nodeWidth / 2;;
    if (addIndexPaths1.size() > curStep) {
        for (size_t i = 0; i < addIndexPaths1[curStep].size(); i++) {
            Vector2 center = std::get<0>(addIndexPaths1[curStep][i]);
            Color color = std::get<1>(addIndexPaths1[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
    if (pause && addIndexPaths2.size() > curStep) {
        for (size_t i = 0; i < addIndexPaths2[curStep].size(); i++) {
            Vector2 center = std::get<0>(addIndexPaths2[curStep][i]);
            Color color = std::get<1>(addIndexPaths2[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
}

void LinkedList::drawAddTailDescription() {
    DrawText("Adding to Tail:", 500, 610, 20, BLACK);
    if (addTailDescriptions.size() > curStep) {
        DrawText(addTailDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = addTailCodeIndex.size() > curStep ? addTailCodeIndex[curStep] : 0;
    for (int i = 0; i < addTailCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(addTailCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    float radius = nodeWidth / 2;;
    if (addTailPaths1.size() > curStep) {
        for (size_t i = 0; i < addTailPaths1[curStep].size(); i++) {
            Vector2 center = std::get<0>(addTailPaths1[curStep][i]);
            Color color = std::get<1>(addTailPaths1[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
    if (pause && addTailPaths2.size() > curStep) {
        for (size_t i = 0; i < addTailPaths2[curStep].size(); i++) {
            Vector2 center = std::get<0>(addTailPaths2[curStep][i]);
            Color color = std::get<1>(addTailPaths2[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
}

void LinkedList::drawDeleteDescription() {
    DrawText("Deleting:", 500, 610, 20, BLACK);
    if (deleteDescriptions.size() > curStep) {
        DrawText(deleteDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = deleteCodeIndex.size() > curStep ? deleteCodeIndex[curStep] : 0;
    for (int i = 0; i < deleteCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(deleteCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    float radius = nodeWidth / 2;;
    if (deletePaths1.size() > curStep) {
        for (size_t i = 0; i < deletePaths1[curStep].size(); i++) {
            Vector2 center = std::get<0>(deletePaths1[curStep][i]);
            Color color = std::get<1>(deletePaths1[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
    if (pause && deletePaths2.size() > curStep) {
        for (size_t i = 0; i < deletePaths2[curStep].size(); i++) {
            Vector2 center = std::get<0>(deletePaths2[curStep][i]);
            Color color = std::get<1>(deletePaths2[curStep][i]);
            if (center.y == listPosY + nodeHeight / 2) { // Node
                DrawCircleV(center, radius, Fade(color, 0.2f));
                DrawCircleLines(center.x, center.y, radius, color);
            } else { // Edge
                DrawRectangle(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, Fade(color, 0.2f));
                DrawRectangleLines(center.x - nodeSpaceX / 2, center.y - lineWidth / 2, nodeSpaceX, lineWidth, color);
            }
        }
    }
}

void LinkedList::drawSearchDescription() {
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
    float radius = nodeWidth / 2;;
    if (searchPaths1.size() > curStep) {
        for (size_t i = 0; i < searchPaths1[curStep].size(); i++) {
            Vector2 center = std::get<0>(searchPaths1[curStep][i]);
            Color color = std::get<1>(searchPaths1[curStep][i]);
            DrawCircleV(center, radius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, radius, color);
        }
    }
    if (pause && searchPaths2.size() > curStep) {
        for (size_t i = 0; i < searchPaths2[curStep].size(); i++) {
            Vector2 center = std::get<0>(searchPaths2[curStep][i]);
            Color color = std::get<1>(searchPaths2[curStep][i]);
            DrawCircleV(center, radius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, radius, color);
        }
    }
}

void LinkedList::drawUpdateDescription() {
    DrawText("Updating:", 500, 610, 20, BLACK);
    if (updateDescriptions.size() > curStep) {
        DrawText(updateDescriptions[curStep].c_str(), 500, 640, 20, BLACK);
    }
    int stt = updateCodeIndex.size() > curStep ? updateCodeIndex[curStep] : 0;
    for (int i = 0; i < updateCodes.size(); i++) {
        Color c = stt == i ? RED : BLACK;
        DrawText(updateCodes[i].c_str(), codePosX, codePosY + i * codePosSpace, 20, c);
        if (stt == i) {
            DrawText(">>", codePosX - 20, codePosY + i * codePosSpace, 20, RED);
        }
    }
    float radius = nodeWidth / 2;;
    if (updatePaths1.size() > curStep) {
        for (size_t i = 0; i < updatePaths1[curStep].size(); i++) {
            Vector2 center = std::get<0>(updatePaths1[curStep][i]);
            Color color = std::get<1>(updatePaths1[curStep][i]);
            DrawCircleV(center, radius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, radius, color);
        }
    }
    if (pause && updatePaths2.size() > curStep) {
        for (size_t i = 0; i < updatePaths2[curStep].size(); i++) {
            Vector2 center = std::get<0>(updatePaths2[curStep][i]);
            Color color = std::get<1>(updatePaths2[curStep][i]);
            DrawCircleV(center, radius, Fade(color, 0.2f));
            DrawCircleLines(center.x, center.y, radius, color);
        }
    }
}

void LinkedList::handleFileDrop() {
    if (showUploadPrompt && IsFileDropped()) {
        FilePathList droppedFiles = LoadDroppedFiles();
        if (droppedFiles.count > 0) {
            TextCopy(pathfile, droppedFiles.paths[0]);
            fileLoaded = true;
            showUploadPrompt = false;
        }
        UnloadDroppedFiles(droppedFiles);
    }
}

void LinkedList::DrawScreen() {
    ClearBackground(WHITE);
    DrawTextEx(GetFontDefault(), "Linked List", Vector2{500, 20}, 20, 2, BLACK);
    drawOperationMenu();
    drawAnimationMenu();
    
    if ((operation_type < 0) || (curStep == totalStep - 1 && totalStep > 0)) {
        drawList();
    } else {
        drawPrevList();
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
        drawAddHeadDescription();
        drawAddHeadAnimation();
    }
    if (operation_type == 2) {
        drawAddIndexDescription();
        drawAddIndexAnimation();
    }
    if (operation_type == 3) {
        drawAddTailDescription();
        drawAddTailAnimation();
    }
    if (operation_type == 4) {
        drawDeleteDescription();
        drawDeleteAnimation();
    }
    if (operation_type == 5) {
        drawSearchDescription();
        drawSearchAnimation();
    }
    if (operation_type == 6) {
        drawInitializeDescription();
        drawInitializeAnimation();
    }
    if (operation_type == 7) {
        drawUpdateDescription();
        drawUpdateAnimation();
    }

    // Vẽ nút "Back" ở góc trái trên với kích thước tương đương một node
    Rectangle backButtonBounds = { 20, 20, 65, 65 }; // Kích thước nút 65x65 pixels, tương đương nodeWidth
    DrawBackButton(backButtonTexture, backButtonBounds, backButtonClicked);
}