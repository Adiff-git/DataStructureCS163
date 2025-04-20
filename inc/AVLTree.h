#ifndef AVLTREE_H
#define AVLTREE_H

#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>

// Node structure for the AVL tree
struct Node {
    int data;           // Holds the node's value
    Node* left;         // Pointer to the left child
    Node* right;        // Pointer to the right child
    int height;         // Stores height for AVL tree balancing
    Node(int val);      // Constructor to initialize a node with a value
};

// AVLTree class with core operations and visualization support
class AVLTree {
public:
    Node* root;         // Points to the root node of the AVL tree

    void destroyTree(Node* node); // Recursively deletes subtree
    Node* copyTree(const Node* node); // Creates a deep copy of subtree

    // Constructor & Destructor
    AVLTree();          // Initializes an empty AVL tree
    ~AVLTree();         // Deallocates all nodes to prevent memory leaks
    AVLTree(const AVLTree& other); // Copy constructor for deep copying
    AVLTree& operator=(const AVLTree& other); // Assignment operator for deep copying

    int getHeight(Node* node) const; // Returns node height for balancing
    int getBalanceFactor(Node* node); // Calculates balance factor (left height - right height)
    float getSubtreeWidth(Node* node); // Computes subtree width for visualization
    Node* minValueNode(Node* node); // Finds node with smallest value

    void rightRotate(Node*& root); // Performs right rotation to balance tree
    void leftRotate(Node*& root); // Performs left rotation to balance tree
    void insert(Node*& node, int value); // Inserts a value and rebalances
    void remove(Node*& node, int value); // Removes a value and rebalances
    bool search(int value); // Checks if a value exists in the tree
    std::vector<Node*> getInsertionPath(int value); // Returns nodes traversed during insertion

    void printInOrder() const; // Prints tree in-order for debugging
private:
    void printInOrder(Node* node) const; // Recursive helper for in-order traversal
};

#endif // AVLTREE_H