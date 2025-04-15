#ifndef AVLTREE_H
#define AVLTREE_H

#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>

// Node structure for the AVL tree
struct Node {
    int data;
    Node* left;
    Node* right;
    int height; // Added to track height for balancing

    Node(int val);
};

// AVLTree class with core operations and visualization support
class AVLTree {
public:
    Node* root;

    void destroyTree(Node* node);
    Node* copyTree(const Node* node);

    // Constructor & Destructor
    AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree& other);
    AVLTree& operator=(const AVLTree& other);

    int getHeight(Node* node) const;
    int getBalanceFactor(Node* node);
    float getSubtreeWidth(Node* node);
    Node* minValueNode(Node* node);

    void rightRotate(Node*& root);
    void leftRotate(Node*& root);
    void insert(Node*& node, int value);
    void remove(Node*& node, int value);
    bool search(int value);
    std::vector<Node*> getInsertionPath(int value);

    // Debugging method to print the tree in-order
    void printInOrder() const;
private:
    void printInOrder(Node* node) const; // Helper method for in-order traversal
};

#endif // AVLTREE_H