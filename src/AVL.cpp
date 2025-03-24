#include "AVL.h"
#include <iostream>
#include <algorithm>

#define NODE_RADIUS 20
#define VERTICAL_SPACING 50
#define HORIZONTAL_SPACING 40

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    Clear();
}

int AVLTree::Height(Node* node) {
    return node ? node->height : 0;
}

int AVLTree::GetBalance(Node* node) {
    return node ? Height(node->left) - Height(node->right) : 0;
}

Node* AVLTree::RotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = std::max(Height(y->left), Height(y->right)) + 1;
    x->height = std::max(Height(x->left), Height(x->right)) + 1;

    return x;
}

Node* AVLTree::RotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = std::max(Height(x->left), Height(x->right)) + 1;
    y->height = std::max(Height(y->left), Height(y->right)) + 1;

    return y;
}

Node* AVLTree::InsertNode(Node* node, int value, int x, int y, int xOffset) {
    if (!node) {
        return new Node(value, x, y);
    }

    if (value < node->value) {
        node->left = InsertNode(node->left, value, x - xOffset, y + VERTICAL_SPACING, xOffset / 2);
    } else if (value > node->value) {
        node->right = InsertNode(node->right, value, x + xOffset, y + VERTICAL_SPACING, xOffset / 2);
    } else {
        return node; // Duplicate values are not allowed
    }

    node->height = std::max(Height(node->left), Height(node->right)) + 1;

    int balance = GetBalance(node);

    // Left Left Case
    if (balance > 1 && value < node->left->value)
        return RotateRight(node);

    // Right Right Case
    if (balance < -1 && value > node->right->value)
        return RotateLeft(node);

    // Left Right Case
    if (balance > 1 && value > node->left->value) {
        node->left = RotateLeft(node->left);
        return RotateRight(node);
    }

    // Right Left Case
    if (balance < -1 && value < node->right->value) {
        node->right = RotateRight(node->right);
        return RotateLeft(node);
    }

    return node;
}

Node* AVLTree::FindNode(Node* node, int value) {
    if (!node || node->value == value)
        return node;

    if (value < node->value)
        return FindNode(node->left, value);
    else
        return FindNode(node->right, value);
}

Node* AVLTree::RemoveNode(Node* node, int value) {
    if (!node)
        return node;

    if (value < node->value)
        node->left = RemoveNode(node->left, value);
    else if (value > node->value)
        node->right = RemoveNode(node->right, value);
    else {
        if (!node->left || !node->right) {
            Node* temp = node->left ? node->left : node->right;

            if (!temp) {
                temp = node;
                node = nullptr;
            } else {
                *node = *temp;
            }

            delete temp;
        } else {
            Node* temp = GetMinValueNode(node->right);
            node->value = temp->value;
            node->right = RemoveNode(node->right, temp->value);
        }
    }

    if (!node)
        return node;

    node->height = std::max(Height(node->left), Height(node->right)) + 1;

    int balance = GetBalance(node);

    // Left Left Case
    if (balance > 1 && GetBalance(node->left) >= 0)
        return RotateRight(node);

    // Left Right Case
    if (balance > 1 && GetBalance(node->left) < 0) {
        node->left = RotateLeft(node->left);
        return RotateRight(node);
    }

    // Right Right Case
    if (balance < -1 && GetBalance(node->right) <= 0)
        return RotateLeft(node);

    // Right Left Case
    if (balance < -1 && GetBalance(node->right) > 0) {
        node->right = RotateRight(node->right);
        return RotateLeft(node);
    }

    return node;
}

Node* AVLTree::GetMinValueNode(Node* node) {
    Node* current = node;

    while (current && current->left != nullptr)
        current = current->left;

    return current;
}

void AVLTree::PrintTree(Node* node, int space, int indent) {
    if (!node)
        return;

    space += indent;

    PrintTree(node->right, space);

    std::cout << std::endl;
    for (int i = indent; i < space; ++i)
        std::cout << " ";
    std::cout << node->value << "\n";

    PrintTree(node->left, space);
}

void AVLTree::DrawTree(Node* node) {
    if (!node)
        return;

    if (node->left) {
        DrawLine(node->x, node->y, node->left->x, node->left->y, DARKGRAY);
        DrawTree(node->left);
    }

    if (node->right) {
        DrawLine(node->x, node->y, node->right->x, node->right->y, DARKGRAY);
        DrawTree(node->right);
    }

    DrawCircle(node->x, node->y, NODE_RADIUS, SKYBLUE);
    DrawCircleLines(node->x, node->y, NODE_RADIUS, DARKGRAY);
    DrawText(TextFormat("%d", node->value), node->x - NODE_RADIUS / 2, node->y - NODE_RADIUS / 2, 20, RAYWHITE);
}

void AVLTree::DeleteTree(Node* node) {
    if (!node)
        return;

    DeleteTree(node->left);
    DeleteTree(node->right);

    delete node;
}

void AVLTree::Insert(int value) {
    root = InsertNode(root, value, GetScreenWidth() / 2, 50, GetScreenWidth() / 4);
}

bool AVLTree::Find(int value) {
    return FindNode(root, value) != nullptr;
}

void AVLTree::Print() {
    PrintTree(root);
}

void AVLTree::Remove(int value) {
    root = RemoveNode(root, value);
}

void AVLTree::Clear() {
    DeleteTree(root);
    root = nullptr;
}

void AVLTree::Render() {
    DrawTree(root);
}