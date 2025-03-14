#include "AVLTree.h"
#include <algorithm>
#include <cstdlib>

AVLTree::AVLTree() : root(nullptr) {}
AVLTree::~AVLTree() {
    while (root) Delete(root->data);
}

int AVLTree::Height(AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::BalanceFactor(AVLNode* node) {
    return node ? Height(node->left) - Height(node->right) : 0;
}

AVLNode* AVLTree::RotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(Height(y->left), Height(y->right)) + 1;
    x->height = std::max(Height(x->left), Height(x->right)) + 1;
    return x;
}

AVLNode* AVLTree::RotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(Height(x->left), Height(x->right)) + 1;
    y->height = std::max(Height(y->left), Height(y->right)) + 1;
    return y;
}

AVLNode* AVLTree::Insert(AVLNode* node, int value) {
    if (!node) return new AVLNode{value, 1, nullptr, nullptr};
    if (value < node->data) node->left = Insert(node->left, value);
    else if (value > node->data) node->right = Insert(node->right, value);
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
    return node;
}

AVLNode* AVLTree::FindMin(AVLNode* node) {
    while (node->left) node = node->left;
    return node;
}

AVLNode* AVLTree::Delete(AVLNode* node, int value) {
    if (!node) return nullptr;
    if (value < node->data) node->left = Delete(node->left, value);
    else if (value > node->data) node->right = Delete(node->right, value);
    else {
        if (!node->left) {
            AVLNode* temp = node->right;
            delete node;
            return temp;
        } else if (!node->right) {
            AVLNode* temp = node->left;
            delete node;
            return temp;
        }
        AVLNode* temp = FindMin(node->right);
        node->data = temp->data;
        node->right = Delete(node->right, temp->data);
    }

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

void AVLTree::Initialize(int size) {
    for (int i = 0; i < size; i++) Add(rand() % 100);
}

void AVLTree::Add(int value) {
    root = Insert(root, value);
}

void AVLTree::Delete(int value) {
    root = Delete(root, value);
}

void AVLTree::Update(int oldValue, int newValue) {
    Delete(oldValue);
    Add(newValue);
}

bool AVLTree::Search(int value) {
    AVLNode* current = root;
    while (current) {
        if (value == current->data) return true;
        if (value < current->data) current = current->left;
        else current = current->right;
    }
    return false;
}

void AVLTree::DrawNode(Font font, AVLNode* node, int x, int y, int level) {
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

void AVLTree::Draw(Font font, int x, int y) {
    DrawNode(font, root, x, y, 1);
}