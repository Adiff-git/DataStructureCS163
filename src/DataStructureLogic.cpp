#include "DataStructureLogic.h"
#include "Animation.h"
#include "raylib.h"

DataStructureLogic::DataStructureLogic() {}     // Hàm tạo mặc định
DataStructureLogic::~DataStructureLogic() {     // Hàm hủy
    ClearSteps();                               // Xóa các bước đã lưu
}

void DataStructureLogic::DrawAnimation(Font font, int x, int y) { // Vẽ animation của cấu trúc dữ liệu
    if (currentStep >= 0 && currentStep < static_cast<int>(stepStates.size())) { // Nếu bước hợp lệ
        Draw(font, stepStates[currentStep], x, y); // Vẽ trạng thái hiện tại
    }
}

void DataStructureLogic::UpdateAnimation() {    // Cập nhật animation
    Animation::UpdateAnimation();               // Gọi hàm cập nhật từ lớp cha
}