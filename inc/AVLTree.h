#ifndef AVL_TREE_H
#define AVL_TREE_H
#include "raylib.h"
#include <vector>
#include <string>

struct AVLNode {
    int data;
    int height;
    AVLNode* left;
    AVLNode* right;
};

class AVLTree {
private:
    AVLNode* root;
    std::vector<std::string> steps;
    std::vector<AVLNode*> stepStates;
    int Height(AVLNode* node);
    int BalanceFactor(AVLNode* node);
    AVLNode* RotateRight(AVLNode* y);
    AVLNode* RotateLeft(AVLNode* x);
    AVLNode* Insert(AVLNode* node, int value, bool saveSteps = true);
    AVLNode* Delete(AVLNode* node, int value);
    AVLNode* FindMin(AVLNode* node);
    void DrawNode(Font font, AVLNode* node, int x, int y, int level);
    void ClearSteps();
    void SaveStep(const std::string& description);
    AVLNode* CopyTree(AVLNode* source);
public:
    AVLTree();
    ~AVLTree();
    void Initialize(int size);
    void Add(int value);
    void Delete(int value);
    void Update(int oldValue, int newValue);
    bool Search(int value);
    void Draw(Font font, int x, int y, int step);
    int GetTotalSteps() const { return steps.size(); }
};

#endif