#ifndef AVL_TREE_H                                   // Bảo vệ chống bao gồm nhiều lần
#define AVL_TREE_H

#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>

class AVLTree : public DataStructureLogic {          // Lớp AVLTree kế thừa từ DataStructureLogic
private:
    struct AVLNode {                                // Cấu trúc node trong cây AVL
        int data;                                   // Giá trị của node
        int height;                                 // Chiều cao của node
        AVLNode* left;                              // Con trỏ nhánh trái
        AVLNode* right;                             // Con trỏ nhánh phải
        AVLNode(int d) : data(d), height(1), left(nullptr), right(nullptr) {} // Hàm tạo với giá trị
    };
    AVLNode* root;                                  // Gốc của cây
    int Height(AVLNode* node);                      // Tính chiều cao của node
    int BalanceFactor(AVLNode* node);               // Tính hệ số cân bằng
    AVLNode* RotateRight(AVLNode* y);               // Xoay phải
    AVLNode* RotateLeft(AVLNode* x);                // Xoay trái
    AVLNode* Insert(AVLNode* node, int value, bool saveSteps); // Chèn node mới
    AVLNode* DeleteHelper(AVLNode* node, int value); // Hàm trợ giúp xóa node
    AVLNode* FindMin(AVLNode* node);                // Tìm node nhỏ nhất
    void DrawNode(Font font, AVLNode* node, int x, int y, int level); // Vẽ một node
    AVLNode* CopyTree(AVLNode* source);             // Sao chép cây
    void* CopyState() override;                     // Sao chép trạng thái cây
    void Draw(Font font, void* state, int x, int y) override; // Vẽ cây lên màn hình
    void ClearStates() override;                    // Xóa các trạng thái đã lưu

public:
    AVLTree();                                      // Hàm tạo
    ~AVLTree() override;                            // Hàm hủy
    void Initialize(int param) override;            // Khởi tạo cây
    void Add(int value) override;                   // Thêm giá trị
    void Delete(int value) override;                // Xóa giá trị
    void Update(int oldValue, int newValue) override; // Cập nhật giá trị
    bool Search(int value) override;                // Tìm kiếm giá trị
};

#endif