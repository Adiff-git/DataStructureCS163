#include "AVLTree.h"
#include "GUI.h"
#include "Button.h"
#include "raylib.h"
#include <vector>
#include <string>

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    ClearStates();
    std::vector<AVLNode*> nodes;
    if (root) nodes.push_back(root);
    while (!nodes.empty()) {
        AVLNode* node = nodes.back();
        nodes.pop_back();
        if (node->left) nodes.push_back(node->left);
        if (node->right) nodes.push_back(node->right);
        delete node;
    }
}

int AVLTree::Height(AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::BalanceFactor(AVLNode* node) {
    return node ? Height(node->left) - Height(node->right) : 0;
}

AVLTree::AVLNode* AVLTree::RotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(Height(y->left), Height(y->right)) + 1;
    x->height = std::max(Height(x->left), Height(x->right)) + 1;
    return x;
}

AVLTree::AVLNode* AVLTree::RotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(Height(x->left), Height(x->right)) + 1;
    y->height = std::max(Height(y->left), Height(y->right)) + 1;
    return y;
}

AVLTree::AVLNode* AVLTree::Insert(AVLNode* node, int value, bool saveSteps) {
    if (!node) {
        AVLNode* newNode = new AVLNode(value);
        if (saveSteps) SaveStep("Inserted " + std::to_string(value), CopyState());
        return newNode;
    }
    if (value < node->data) node->left = Insert(node->left, value, false);
    else if (value > node->data) node->right = Insert(node->right, value, false);
    else return node;

    node->height = std::max(Height(node->left), Height(node->right)) + 1;
    int balance = BalanceFactor(node);

    if (balance > 1 && value < node->left->data) return RotateRight(node);
    if (balance < -1 && value > node->right->data) return RotateLeft(node);
    if (balance > 1 && value > node->left->data) {
        node->left = RotateLeft(node->left);
        return RotateRight(node);
    }
    if (balance < -1 && value < node->right->data) {
        node->right = RotateRight(node->right);
        return RotateLeft(node);
    }
    if (saveSteps) SaveStep("Inserted " + std::to_string(value), CopyState());
    return node;
}

AVLTree::AVLNode* AVLTree::FindMin(AVLNode* node) {
    while (node->left) node = node->left;
    return node;
}

AVLTree::AVLNode* AVLTree::DeleteHelper(AVLNode* node, int value) {
    if (!node) return nullptr;
    if (value < node->data) node->left = DeleteHelper(node->left, value);
    else if (value > node->data) node->right = DeleteHelper(node->right, value);
    else {
        if (!node->left) {
            AVLNode* temp = node->right;
            delete node;
            SaveStep("Deleted " + std::to_string(value), CopyState());
            return temp;
        } else if (!node->right) {
            AVLNode* temp = node->left;
            delete node;
            SaveStep("Deleted " + std::to_string(value), CopyState());
            return temp;
        }
        AVLNode* temp = FindMin(node->right);
        node->data = temp->data;
        node->right = DeleteHelper(node->right, temp->data);
    }

    if (!node) return nullptr;
    node->height = std::max(Height(node->left), Height(node->right)) + 1;
    int balance = BalanceFactor(node);

    if (balance > 1 && BalanceFactor(node->left) >= 0) return RotateRight(node);
    if (balance > 1 && BalanceFactor(node->left) < 0) {
        node->left = RotateLeft(node->left);
        return RotateRight(node);
    }
    if (balance < -1 && BalanceFactor(node->right) <= 0) return RotateLeft(node);
    if (balance < -1 && BalanceFactor(node->right) > 0) {
        node->right = RotateRight(node->right);
        return RotateLeft(node);
    }
    return node;
}

void AVLTree::DrawNode(Font font, AVLNode* node, int x, int y, int level) {
    if (!node) return;
    DrawCircle(x, y, 20, LIGHTGRAY);
    DrawTextEx(font, TextFormat("%d", node->data), {static_cast<float>(x - 10), static_cast<float>(y - 10)}, 20, 1, DARKBLUE);
    if (node->left) {
        DrawLine(x, y + 20, x - 100 / level, y + 50, BLACK);
        DrawNode(font, node->left, x - 100 / level, y + 50, level + 1);
    }
    if (node->right) {
        DrawLine(x, y + 20, x + 100 / level, y + 50, BLACK);
        DrawNode(font, node->right, x + 100 / level, y + 50, level + 1);
    }
}

AVLTree::AVLNode* AVLTree::CopyTree(AVLNode* source) {
    if (!source) return nullptr;

    std::vector<std::pair<AVLNode*, AVLNode*>> stack;
    AVLNode* newRoot = new AVLNode(source->data);
    newRoot->height = source->height;
    stack.push_back({source, newRoot});

    while (!stack.empty()) {
        auto pair = stack.back(); // Thay structured bindings bằng cách truy cập thông thường
        AVLNode* src = pair.first;
        AVLNode* dest = pair.second;
        stack.pop_back();

        if (src->left) {
            dest->left = new AVLNode(src->left->data); // Sử dụng constructor có tham số
            dest->left->height = src->left->height;
            stack.push_back({src->left, dest->left});
        }
        if (src->right) {
            dest->right = new AVLNode(src->right->data); // Sử dụng constructor có tham số
            dest->right->height = src->right->height;
            stack.push_back({src->right, dest->right});
        }
    }
    return newRoot;
}

void* AVLTree::CopyState() {
    return CopyTree(root);
}

void AVLTree::Draw(Font font, void* state, int x, int y) {
    AVLNode* treeState = static_cast<AVLNode*>(state);
    DrawNode(font, treeState, x, y, 1);
}

void AVLTree::ClearStates() {
    for (auto state : stepStates) {
        AVLNode* treeState = static_cast<AVLNode*>(state);
        std::vector<AVLNode*> nodes;
        if (treeState) nodes.push_back(treeState);
        while (!nodes.empty()) {
            AVLNode* node = nodes.back();
            nodes.pop_back();
            if (node->left) nodes.push_back(node->left);
            if (node->right) nodes.push_back(node->right);
            delete node;
        }
    }
    stepStates.clear();
}

void AVLTree::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    std::vector<AVLNode*> nodes;
    if (root) nodes.push_back(root);
    while (!nodes.empty()) {
        AVLNode* node = nodes.back();
        nodes.pop_back();
        if (node->left) nodes.push_back(node->left);
        if (node->right) nodes.push_back(node->right);
        delete node;
    }
    root = nullptr;
    SaveStep("Initial state (empty)", CopyState());
    for (int i = 0; i < param; i++) Add(rand() % 100);
}

void AVLTree::Add(int value) {
    root = Insert(root, value, true);
}

void AVLTree::Delete(int value) {
    root = DeleteHelper(root, value);
}

void AVLTree::Update(int oldValue, int newValue) {
    Delete(oldValue);
    Add(newValue);
    SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
}

bool AVLTree::Search(int value) {
    ClearSteps();
    AVLNode* current = root;
    while (current) {
        SaveStep("Searching: " + std::to_string(current->data), CopyState());
        if (value == current->data) {
            SaveStep("Found " + std::to_string(value), CopyState());
            return true;
        }
        current = value < current->data ? current->left : current->right;
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    return false;
}

void AVLTree::DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
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

    auto GetInputValue1 = [&]() {
        int value = inputLength1 == 0 ? 0 : atoi(inputBuffer1);
        const int MAX_NODES = 100;
        return std::min(value, MAX_NODES);
    };

    auto GetInputValue2 = [&]() { return inputLength2 == 0 ? 0 : atoi(inputBuffer2); };

    auto DrawInputBox = [&](float x, float y, char* buffer, bool active) {
        DrawRectangle(x, y, 200, 50, WHITE);
        DrawRectangleLines(x, y, 200, 50, BLACK);
        DrawTextEx(font, buffer, {x + 10, y + 10}, 30, 1, BLACK);
        if (active && (GetTime() - (int)GetTime()) < 0.5) {
            DrawTextEx(font, "|", {x + 10 + MeasureTextEx(font, buffer, 30, 1).x, y + 10}, 30, 1, BLACK);
        }
    };

    ClearBackground(RED);
    DrawTextEx(font, "AVL Tree", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

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