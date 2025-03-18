#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "DataStructureLogic.h"
#include "raylib.h"
#include "Screen.h" // Include Screen.h
#include <vector>
#include <string>

class AVLTree : public DataStructureLogic {
private:
    struct AVLNode {
        int data;
        int height;
        AVLNode* left;
        AVLNode* right;
        AVLNode(int d) : data(d), height(1), left(nullptr), right(nullptr) {}
    };
    AVLNode* root;
    int Height(AVLNode* node);
    int BalanceFactor(AVLNode* node);
    AVLNode* RotateRight(AVLNode* y);
    AVLNode* RotateLeft(AVLNode* x);
    AVLNode* Insert(AVLNode* node, int value, bool saveSteps);
    AVLNode* DeleteHelper(AVLNode* node, int value);
    AVLNode* FindMin(AVLNode* node);
    void DrawNode(Font font, AVLNode* node, int x, int y, int level);
    AVLNode* CopyTree(AVLNode* source);
    void* CopyState() override;
    void Draw(Font font, void* state, int x, int y) override;
    void ClearStates() override;

public:
    AVLTree();
    ~AVLTree() override;
    void Initialize(int param) override;
    void Add(int value) override;
    void Delete(int value) override;
    void Update(int oldValue, int newValue) override;
    bool Search(int value) override;
    void DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen);
};

#endif