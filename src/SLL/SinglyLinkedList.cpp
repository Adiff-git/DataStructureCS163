#include "SinglyLinkedList.h"
#include "SLLScreen.h"
#include <iostream>

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
    if (!head) {
        std::cout << "CopyState: List is empty" << std::endl;
        return nullptr;
    }
    Node* newHead = new Node(head->data);
    Node* current = newHead;
    Node* source = head->next;
    int count = 1;
    while (source) {
        current->next = new Node(source->data);
        current = current->next;
        source = source->next;
        count++;
    }
    std::cout << "CopyState: Copied " << count << " nodes" << std::endl;
    return newHead;
}

void SinglyLinkedList::Draw(Font font, void* state, int x, int y) {
    std::cout << "SinglyLinkedList::Draw called" << std::endl;
    sllAnimation.DrawAnimation(font, x, y);
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
    sllAnimation.ClearSteps();
    std::cout << "ClearStates: All states cleared" << std::endl;
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
    sllAnimation.AddStep("Initial state (empty)", sllAnimation.CopyNodes(head));

    // Nếu param <= 0, tạo mặc định 1 node
    if (param <= 0) param = 1;

    // Tạo ngẫu nhiên param node
    head = new Node(rand() % 100);
    SaveStep("Added first node: " + std::to_string(head->data), CopyState());
    sllAnimation.AddStep("Added first node: " + std::to_string(head->data), sllAnimation.CopyNodes(head), 0);
    Node* current = head;
    for (int i = 1; i < param; i++) {
        current->next = new Node(rand() % 100);
        current = current->next;
        SaveStep("Added node: " + std::to_string(current->data), CopyState());
        sllAnimation.AddStep("Added node: " + std::to_string(current->data), sllAnimation.CopyNodes(head), i);
    }

    // Hiển thị ngay lập tức
    currentStep = steps.size() - 1; // Chuyển đến bước cuối cùng
    sllAnimation.UpdateAnimation(); // Cập nhật để hiển thị node ngay
    std::cout << "Initialize: Created " << param << " nodes, currentStep = " << currentStep << std::endl;
}

void SinglyLinkedList::Add(int value) {
    ClearSteps();
    Node* newNode = new Node(value);
    currentSteps = {"if (list is empty) then", "    head = new Node(value)", "    return", "current = head", "while (current->next exists)", "    current = current->next", "current->next = new Node(value)"};
    pseudocode.SetSteps(currentSteps);
    if (!head) {
        head = newNode;
        SaveStep("Added first node: " + std::to_string(value), CopyState());
        sllAnimation.AddStep("Added first node: " + std::to_string(value), sllAnimation.CopyNodes(head), 1);
        std::cout << "Add: Added first node with value " << value << std::endl;
    } else {
        Node* current = head;
        int index = 0;
        while (current->next) {
            SaveStep("Checking node " + std::to_string(current->data), CopyState());
            sllAnimation.AddStep("Checking node " + std::to_string(current->data), sllAnimation.CopyNodes(head), 4);
            current = current->next;
            index++;
        }
        SaveStep("Checking node " + std::to_string(current->data), CopyState());
        sllAnimation.AddStep("Checking node " + std::to_string(current->data), sllAnimation.CopyNodes(head), 4);
        current->next = newNode;
        SaveStep("Added node: " + std::to_string(value), CopyState());
        sllAnimation.AddStep("Added node: " + std::to_string(value), sllAnimation.CopyNodes(head), 6);
        std::cout << "Add: Added node with value " << value << " at index " << index + 1 << std::endl;
    }

    // Hiển thị ngay lập tức
    currentStep = steps.size() - 1; // Chuyển đến bước cuối cùng
    sllAnimation.UpdateAnimation(); // Cập nhật để hiển thị node ngay
}

void SinglyLinkedList::Delete(int value) {
    ClearSteps();
    if (!head) return;
    currentSteps = {"if (list is empty) then", "    return", "if (head->data == value) then", "    temp = head", "    head = head->next", "    delete temp", "    return", "current = head", "while (current->next and current->next->data != value)", "    current = current->next", "if (current->next exists)", "    temp = current->next", "    current->next = temp->next", "    delete temp"};
    pseudocode.SetSteps(currentSteps);
    if (head->data == value) {
        SaveStep("Found node to delete: " + std::to_string(value), CopyState());
        sllAnimation.AddStep("Found node to delete: " + std::to_string(value), sllAnimation.CopyNodes(head), 3);
        Node* temp = head;
        head = head->next;
        delete temp;
        SaveStep("Deleted node: " + std::to_string(value), CopyState());
        sllAnimation.AddStep("Deleted node: " + std::to_string(value), sllAnimation.CopyNodes(head), 5, true);
        return;
    }
    Node* current = head;
    int index = 0;
    while (current->next && current->next->data != value) {
        SaveStep("Checking node " + std::to_string(current->data), CopyState());
        sllAnimation.AddStep("Checking node " + std::to_string(current->data), sllAnimation.CopyNodes(head), 9);
        current = current->next;
        index++;
    }
    if (current->next) {
        SaveStep("Found node to delete: " + std::to_string(value), CopyState());
        sllAnimation.AddStep("Found node to delete: " + std::to_string(value), sllAnimation.CopyNodes(head), 11);
        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
        SaveStep("Deleted node: " + std::to_string(value), CopyState());
        sllAnimation.AddStep("Deleted node: " + std::to_string(value), sllAnimation.CopyNodes(head), 13, true);
    }
}

void SinglyLinkedList::Update(int oldValue, int newValue) {
    ClearSteps();
    currentSteps = {"current = head", "while (current exists)", "    if (current->data == oldValue) then", "        current->data = newValue", "        break", "    current = current->next", "if (not updated) then", "    print 'Value not found'"};
    pseudocode.SetSteps(currentSteps);
    Node* current = head;
    int index = 0;
    bool updated = false;
    while (current) {
        SaveStep("Checking node " + std::to_string(current->data), CopyState());
        sllAnimation.AddStep("Checking node " + std::to_string(current->data), sllAnimation.CopyNodes(head), 2);
        if (current->data == oldValue) {
            current->data = newValue;
            updated = true;
            SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
            sllAnimation.AddStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), sllAnimation.CopyNodes(head), 4);
        }
        current = current->next;
        index++;
    }
    if (!updated) {
        SaveStep("Value " + std::to_string(oldValue) + " not found", CopyState());
        sllAnimation.AddStep("Value " + std::to_string(oldValue) + " not found", sllAnimation.CopyNodes(head), 7);
    }
}

bool SinglyLinkedList::Search(int value) {
    ClearSteps();
    currentSteps = {"current = head", "while (current exists)", "    if (current->data == value) then", "        return true", "    current = current->next", "print 'Not found'", "return false"};
    pseudocode.SetSteps(currentSteps);
    Node* current = head;
    int index = 0;
    while (current) {
        SaveStep("Checking node " + std::to_string(current->data), CopyState());
        sllAnimation.AddStep("Checking node " + std::to_string(current->data), sllAnimation.CopyNodes(head), 2);
        if (current->data == value) {
            SaveStep("Found " + std::to_string(value), CopyState());
            sllAnimation.AddStep("Found " + std::to_string(value), sllAnimation.CopyNodes(head), 3);
            return true;
        }
        current = current->next;
        index++;
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    sllAnimation.AddStep("Not found: " + std::to_string(value), sllAnimation.CopyNodes(head), 5);
    return false;
}

void SinglyLinkedList::UpdateAnimation() {
    Animation::UpdateAnimation();
    sllAnimation.UpdateAnimation();
    std::cout << "SinglyLinkedList::UpdateAnimation called" << std::endl;
}

void SinglyLinkedList::DrawAnimation(Font font, int x, int y) {
    std::cout << "SinglyLinkedList::DrawAnimation called" << std::endl;
    sllAnimation.DrawAnimation(font, x, y);
    pseudocode.Draw(font, GetCurrentStep());
}

void SinglyLinkedList::DrawDirect(Font font, int x, int y) {
    std::cout << "SinglyLinkedList::DrawDirect called" << std::endl;
    Node* current = head;
    int posX = x;
    while (current) {
        Color color = { 200, 200, 200, 255 };
        DrawRectangle(posX, y, 50, 50, color);
        DrawTextEx(font, TextFormat("%d", current->data), { static_cast<float>(posX + 10), static_cast<float>(y + 10) }, 30, 1, DARKBLUE);
        if (current->next) {
            DrawLine(posX + 50, y + 25, posX + 100, y + 25, BLACK);
        }
        current = current->next;
        posX += 100;
    }
}

void SinglyLinkedList::DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    SLLScreen::DrawScreen(font, buttonClicked, buttonMessage, currentScreen, this);
}