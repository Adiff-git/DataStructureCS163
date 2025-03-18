#include "HashTable.h"
#include "GUI.h"
#include "Button.h"
#include "raylib.h"
#include <vector>
#include <string>

HashTable::HashTable(int size) : size(size) {
    table.resize(size, nullptr);
}

HashTable::~HashTable() {
    ClearStates();
    for (auto& slot : table) {
        while (slot) {
            HashNode* temp = slot;
            slot = slot->next;
            delete temp;
        }
    }
}

void* HashTable::CopyState() {
    std::vector<HashNode*> newTable(size, nullptr);
    for (int i = 0; i < size; i++) {
        HashNode* current = table[i];
        if (current) {
            newTable[i] = new HashNode(current->key);
            HashNode* newCurrent = newTable[i];
            while (current->next) {
                newCurrent->next = new HashNode(current->next->key);
                newCurrent = newCurrent->next;
                current = current->next;
            }
        }
    }
    return new std::vector<HashNode*>(newTable);
}

void HashTable::Draw(Font font, void* state, int x, int y) {
    std::vector<HashNode*>* tableState = static_cast<std::vector<HashNode*>*>(state);
    for (int i = 0; i < size; i++) {
        DrawTextEx(font, TextFormat("%d:", i), {static_cast<float>(x), static_cast<float>(y + i * 60)}, 20, 1, BLACK);
        HashNode* current = (*tableState)[i];
        int offset = 50;
        while (current) {
            DrawRectangle(x + offset, y + i * 60, 50, 50, LIGHTGRAY);
            DrawTextEx(font, TextFormat("%d", current->key), {static_cast<float>(x + offset + 10), static_cast<float>(y + i * 60 + 10)}, 20, 1, DARKBLUE);
            if (current->next) DrawLine(x + offset + 50, y + i * 60 + 25, x + offset + 100, y + i * 60 + 25, BLACK);
            current = current->next;
            offset += 100;
        }
    }
}

void HashTable::ClearStates() {
    for (auto state : stepStates) {
        std::vector<HashNode*>* tableState = static_cast<std::vector<HashNode*>*>(state);
        for (auto& slot : *tableState) {
            while (slot) {
                HashNode* temp = slot;
                slot = slot->next;
                delete temp;
            }
        }
        delete tableState;
    }
    stepStates.clear();
}

void HashTable::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    for (auto& slot : table) {
        while (slot) {
            HashNode* temp = slot;
            slot = slot->next;
            delete temp;
        }
    }
    SaveStep("Initial state (empty)", CopyState());
    for (int i = 0; i < param; i++) Add(rand() % 100);
}

void HashTable::Add(int value) {
    int index = hash(value);
    HashNode* newNode = new HashNode(value);
    if (!table[index]) {
        table[index] = newNode;
    } else {
        HashNode* current = table[index];
        while (current->next) current = current->next;
        current->next = newNode;
    }
    SaveStep("Added key: " + std::to_string(value), CopyState());
}

void HashTable::Delete(int value) {
    int index = hash(value);
    if (!table[index]) return;
    if (table[index]->key == value) {
        HashNode* temp = table[index];
        table[index] = table[index]->next;
        delete temp;
        SaveStep("Deleted key: " + std::to_string(value), CopyState());
        return;
    }
    HashNode* current = table[index];
    while (current->next && current->next->key != value) current = current->next;
    if (current->next) {
        HashNode* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted key: " + std::to_string(value), CopyState());
    }
}

void HashTable::Update(int oldValue, int newValue) {
    int index = hash(oldValue);
    HashNode* current = table[index];
    bool updated = false;
    while (current) {
        if (current->key == oldValue) {
            current->key = newValue;
            updated = true;
        }
        current = current->next;
    }
    if (updated) SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
}

bool HashTable::Search(int value) {
    ClearSteps();
    int index = hash(value);
    HashNode* current = table[index];
    int pos = 0;
    while (current) {
        SaveStep("Searching at slot " + std::to_string(index) + ", pos " + std::to_string(pos) + ": " + std::to_string(current->key), CopyState());
        if (current->key == value) {
            SaveStep("Found " + std::to_string(value), CopyState());
            return true;
        }
        current = current->next;
        pos++;
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    return false;
}

void HashTable::DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    static char inputBuffer1[32] = "";
    static char inputBuffer2[32] = "";
    static int inputLength1 = 0;
    static int inputLength2 = 0;
    static bool inputActive1 = false;
    static bool inputActive2 = false;

    auto HandleInput1 = [&]() {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && inputLength1 < 31) {
                inputBuffer1[inputLength1] = (char)key;
                inputLength1++;
                inputBuffer1[inputLength1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && inputLength1 > 0) {
            inputLength1--;
            inputBuffer1[inputLength1] = '\0';
        }
    };

    auto HandleInput2 = [&]() {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && inputLength2 < 31) {
                inputBuffer2[inputLength2] = (char)key;
                inputLength2++;
                inputBuffer2[inputLength2] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && inputLength2 > 0) {
            inputLength2--;
            inputBuffer2[inputLength2] = '\0';
        }
    };

    auto GetInputValue1 = [&]() { return inputLength1 == 0 ? 0 : atoi(inputBuffer1); };
    auto GetInputValue2 = [&]() { return inputLength2 == 0 ? 0 : atoi(inputBuffer2); };

    auto DrawInputBox = [&](float x, float y, char* buffer, bool active) {
        DrawRectangle(x, y, 200, 50, WHITE);
        DrawRectangleLines(x, y, 200, 50, BLACK);
        DrawTextEx(font, buffer, {x + 10, y + 10}, 30, 1, BLACK);
        if (active && (GetTime() - (int)GetTime()) < 0.5) {
            DrawTextEx(font, "|", {x + 10 + MeasureTextEx(font, buffer, 30, 1).x, y + 10}, 30, 1, BLACK);
        }
    };

    ClearBackground(LIME);
    DrawTextEx(font, "Hash Table", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float inputX1 = 50.0f;
    float inputY1 = 100.0f;
    float inputX2 = 270.0f;
    float inputY2 = 100.0f;
    DrawInputBox(inputX1, inputY1, inputBuffer1, inputActive1);
    DrawInputBox(inputX2, inputY2, inputBuffer2, inputActive2);

    if (CheckCollisionPointRec(GetMousePosition(), {inputX1, inputY1, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = true; inputActive2 = false;
    } else if (CheckCollisionPointRec(GetMousePosition(), {inputX2, inputY2, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = false;
    }
    if (inputActive1) HandleInput1();
    if (inputActive2) HandleInput2();

    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        Initialize(value);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Add", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        Add(value);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Delete", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        Delete(value);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Update", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        int oldValue = GetInputValue1();
        int newValue = GetInputValue2();
        Update(oldValue, newValue);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
        inputLength2 = 0; inputBuffer2[0] = '\0';
    }
    if (DrawButton("Search", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        Search(value);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, this);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}