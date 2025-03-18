#ifndef HASH_TABLE_H                                  // Bảo vệ chống bao gồm nhiều lần
#define HASH_TABLE_H

#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>

class HashTable : public DataStructureLogic {           // Lớp HashTable kế thừa từ DataStructureLogic
private:
    struct HashNode {                                   // Cấu trúc node trong bảng băm
        int key;                                        // Giá trị khóa
        HashNode* next;                                 // Con trỏ tới node tiếp theo
        HashNode(int k) : key(k), next(nullptr) {}      // Hàm tạo với khóa, mặc định next là nullptr
    };
    std::vector<HashNode*> table;                       // Vector lưu các danh sách liên kết (slot)
    int size;                                           // Kích thước bảng băm
    int hash(int key) { return key % size; }            // Hàm băm đơn giản: lấy dư với size
    void* CopyState() override;                         // Sao chép trạng thái bảng băm
    void Draw(Font font, void* state, int x, int y) override; // Vẽ bảng băm lên màn hình
    void ClearStates() override;                        // Xóa các trạng thái đã lưu

public:
    HashTable(int size);                                // Hàm tạo với kích thước bảng
    ~HashTable() override;                              // Hàm hủy
    void Initialize(int param) override;                // Khởi tạo bảng băm
    void Add(int value) override;                       // Thêm giá trị
    void Delete(int value) override;                    // Xóa giá trị
    void Update(int oldValue, int newValue) override;   // Cập nhật giá trị
    bool Search(int value) override;                    // Tìm kiếm giá trị
};

#endif