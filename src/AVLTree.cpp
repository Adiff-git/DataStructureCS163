#include "AVLTree.h"
#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>

int AVLTree::Height(AVLTree::AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::BalanceFactor(AVLTree::AVLNode* node) {
    return node ? Height(node->left) - Height(node->right) : 0;
}

AVLTree::AVLTree() : root(nullptr) {}
AVLTree::~AVLTree() {
    while (root) {
        Delete(root->data);
    }
    ClearStates();
}

AVLTree::AVLNode* AVLTree::CopyTree(AVLTree::AVLNode* source) {
    if (!source) return nullptr;
    AVLNode* newNode = new AVLNode(source->data);
    newNode->height = source->height;
    newNode->left = CopyTree(source->left);
    newNode->right = CopyTree(source->right);
    return newNode;
}

void* AVLTree::CopyState() {
    return CopyTree(root);
}

void AVLTree::DrawNode(Font font, AVLTree::AVLNode* node, int x, int y, int level) {
    if (!node) return;
    DrawRectangle(x - 25, y, 50, 50, LIGHTGRAY);
    DrawTextEx(font, TextFormat("%d", node->data), {static_cast<float>(x - 15), static_cast<float>(y + 10)}, 30, 1, DARKBLUE);
    if (node->left) {
        DrawLine(x, y + 50, x - 100 / level, y + 100, BLACK);
        DrawNode(font, node->left, x - 100 / level, y + 100, level + 1);
    }
    if (node->right) {
        DrawLine(x, y + 50, x + 100 / level, y + 100, BLACK);
        DrawNode(font, node->right, x + 100 / level, y + 100, level + 1);
    }
}

void AVLTree::Draw(Font font, void* state, int x, int y) {
    AVLNode* node = static_cast<AVLNode*>(state);
    DrawNode(font, node, x, y, 1);
}

void AVLTree::ClearStates() {
    for (auto state : stepStates) {
        AVLNode* current = static_cast<AVLNode*>(state);
        if (!current) continue;
        std::vector<AVLNode*> nodes;
        nodes.push_back(current);
        while (!nodes.empty()) {
            AVLNode* node = nodes.back();
            nodes.pop_back();
            if (node->left) nodes.push_back(node->left);
            if (node->right) nodes.push_back(node->right);
            delete node;
        }
    }
}

AVLTree::AVLNode* AVLTree::RotateRight(AVLTree::AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(Height(y->left), Height(y->right)) + 1;
    x->height = std::max(Height(x->left), Height(x->right)) + 1;
    SaveStep("Performed right rotation", CopyState());
    return x;
}

AVLTree::AVLNode* AVLTree::RotateLeft(AVLTree::AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(Height(x->left), Height(x->right)) + 1;
    y->height = std::max(Height(y->left), Height(y->right)) + 1;
    SaveStep("Performed left rotation", CopyState());
    return y;
}

AVLTree::AVLNode* AVLTree::Insert(AVLTree::AVLNode* node, int value, bool saveSteps) {
    if (!node) {
        AVLNode* newNode = new AVLNode(value);
        if (saveSteps) SaveStep("Added node: " + std::to_string(value), CopyState());
        return newNode;
    }
    if (value < node->data) node->left = Insert(node->left, value, saveSteps);
    else if (value > node->data) node->right = Insert(node->right, value, saveSteps);
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
    if (saveSteps) SaveStep("Updated height of node: " + std::to_string(node->data), CopyState());
    return node;
}

AVLTree::AVLNode* AVLTree::FindMin(AVLTree::AVLNode* node) {
    while (node && node->left) node = node->left;
    return node;
}

AVLTree::AVLNode* AVLTree::DeleteHelper(AVLTree::AVLNode* node, int value) {
    if (!node) return nullptr;
    if (value < node->data) node->left = DeleteHelper(node->left, value);
    else if (value > node->data) node->right = DeleteHelper(node->right, value);
    else {
        if (!node->left) {
            AVLNode* temp = node->right;
            delete node;
            SaveStep("Deleted node: " + std::to_string(value), CopyState());
            return temp;
        } else if (!node->right) {
            AVLNode* temp = node->left;
            delete node;
            SaveStep("Deleted node: " + std::to_string(value), CopyState());
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
    SaveStep("Updated height of node: " + std::to_string(node->data), CopyState());
    return node;
}

void AVLTree::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    while (root) {
        Delete(root->data);
    }
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
}

bool AVLTree::Search(int value) {
    ClearSteps();
    AVLNode* current = root;
    while (current) {
        SaveStep("Visiting node: " + std::to_string(current->data), CopyState());
        if (value == current->data) {
            SaveStep("Found " + std::to_string(value), CopyState());
            return true;
        }
        if (value < current->data) current = current->left;
        else current = current->right;
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    return false;
}