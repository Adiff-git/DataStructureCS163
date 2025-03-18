#ifndef DATA_STRUCTURE_LOGIC_H                     // Bảo vệ chống bao gồm nhiều lần
#define DATA_STRUCTURE_LOGIC_H

#include "Animation.h"
#include "raylib.h"
#include <vector>

class DataStructureLogic : public Animation {        // Lớp cơ sở cho các cấu trúc dữ liệu
protected:
    virtual void* CopyState() = 0;                   // Sao chép trạng thái (ảo)
    virtual void Draw(Font font, void* state, int x, int y) = 0; // Vẽ trạng thái (ảo)
    virtual void ClearStates() = 0;                  // Xóa các trạng thái (ảo)

public:
    DataStructureLogic();                            // Hàm tạo
    virtual ~DataStructureLogic() override;          // Hàm hủy ảo
    virtual void Initialize(int param) = 0;          // Khởi tạo cấu trúc (ảo)
    virtual void Add(int value) = 0;                 // Thêm giá trị (ảo)
    virtual void Delete(int value) = 0;              // Xóa giá trị (ảo)
    virtual void Update(int oldValue, int newValue) = 0; // Cập nhật giá trị (ảo)
    virtual bool Search(int value) = 0;              // Tìm kiếm giá trị (ảo)
    void DrawAnimation(Font font, int x, int y) override; // Vẽ animation
    void UpdateAnimation() override;                 // Cập nhật animation
};

#endif