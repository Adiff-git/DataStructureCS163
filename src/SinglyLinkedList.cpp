#include "SinglyLinkedList.h"
#include "GUI.h"
#include "Button.h"
#include "raylib.h"
#include <string>

SinglyLinkedList::SinglyLinkedList() : head(nullptr) {}

SinglyLinkedList::~SinglyLinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    ClearStates();
}

void* SinglyLinkedList::CopyState() {
    if (!head) return nullptr;
    Node* newHead = new Node(head->data);
    Node* current = newHead;
    Node* source = head->next;
    while (source) {
        current->next = new Node(source->data);
        current = current->next;
        source = source->next;
    }
    return newHead;
}

void SinglyLinkedList::Draw(Font font, void* state, int x, int y) {
    Node* current = static_cast<Node*>(state);
    int offset = 0;
    while (current) {
        DrawRectangle(x + offset, y, 50, 50, LIGHTGRAY);
        DrawTextEx(font, TextFormat("%d", current->data), {static_cast<float>(x + offset + 10), static_cast<float>(y + 10)}, 30, 1, DARKBLUE);
        if (current->next) DrawLine(x + offset + 50, y + 25, x + offset + 100, y + 25, BLACK);
        current = current->next;
        offset += 100;
    }
}

void SinglyLinkedList::ClearStates() {
    for (auto state : stepStates) {
        Node* current = static_cast<Node*>(state);
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    stepStates.clear();
}

void SinglyLinkedList::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    SaveStep("Initial state (empty)", CopyState());
    for (int i = 0; i < param; i++) Add(rand() % 100);
}

void SinglyLinkedList::Add(int value) {
    Node* newNode = new Node(value);
    if (!head) {
        head = newNode;
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
    SaveStep("Added node: " + std::to_string(value), CopyState());
}

void SinglyLinkedList::Delete(int value) {
    if (!head) return;
    if (head->data == value) {
        Node* temp = head;
        head = head->next;
        delete temp;
        SaveStep("Deleted node: " + std::to_string(value), CopyState());
        return;
    }
    Node* current = head;
    while (current->next && current->next->data != value) current = current->next;
    if (current->next) {
        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted node: " + std::to_string(value), CopyState());
    }
}

void SinglyLinkedList::Update(int oldValue, int newValue) {
    Node* current = head;
    bool updated = false;
    while (current) {
        if (current->data == oldValue) {
            current->data = newValue;
            updated = true;
        }
        current = current->next;
    }
    if (updated) SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
}

bool SinglyLinkedList::Search(int value) {
    ClearSteps();
    Node* current = head;
    int index = 0;
    while (current) {
        SaveStep("Searching at position " + std::to_string(index) + ": " + std::to_string(current->data), CopyState());
        if (current->data == value) {
            SaveStep("Found " + std::to_string(value), CopyState());
            return true;
        }
        current = current->next;
        index++;
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    return false;
}

void SinglyLinkedList::DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
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

    ClearBackground(ORANGE);
    DrawTextEx(font, "Singly Linked List", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

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