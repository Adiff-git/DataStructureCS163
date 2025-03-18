#include "Animation.h"
#include "raylib.h"

Animation::Animation() : currentStep(0), isRunning(false), lastStepTime(0.0), animationSpeed(1.0) {} // Hàm tạo mặc định
Animation::~Animation() {                       // Hàm hủy
    ClearSteps();                               // Xóa các bước đã lưu
}

void Animation::ClearSteps() {                  // Xóa tất cả các bước và trạng thái
    steps.clear();                              // Xóa danh sách mô tả bước
    for (auto state : stepStates) {             // Duyệt qua từng trạng thái
        // Không giải phóng bộ nhớ ở đây, để lớp con xử lý
    }
    stepStates.clear();                         // Xóa danh sách trạng thái
    currentStep = 0;                            // Đặt lại bước hiện tại về 0
    isRunning = false;                          // Tạm dừng animation
}

void Animation::SaveStep(const std::string& description, void* state) { // Lưu một bước mới
    steps.push_back(description);               // Thêm mô tả bước vào danh sách
    stepStates.push_back(state);                // Thêm trạng thái vào danh sách
}

void Animation::ResetAnimation() {              // Đặt lại animation
    currentStep = 0;                            // Đặt bước hiện tại về 0
    isRunning = false;                          // Tạm dừng animation
    lastStepTime = GetTime();                   // Lưu thời gian hiện tại
}

void Animation::NextStep() {                    // Chuyển sang bước tiếp theo
    if (currentStep < static_cast<int>(steps.size()) - 1) currentStep++; // Tăng bước nếu chưa đến cuối
    isRunning = false;                          // Tạm dừng animation
}

void Animation::PrevStep() {                    // Quay lại bước trước đó
    if (currentStep > 0) currentStep--;         // Giảm bước nếu không ở đầu
    isRunning = false;                          // Tạm dừng animation
}

void Animation::UpdateAnimation() {             // Cập nhật animation
    if (isRunning && currentStep < static_cast<int>(steps.size()) - 1) { // Nếu đang chạy và chưa hết bước
        double currentTime = GetTime();         // Lấy thời gian hiện tại
        if (currentTime - lastStepTime >= animationSpeed) { // Nếu đủ thời gian chuyển bước
            currentStep++;                      // Tăng bước hiện tại
            lastStepTime = currentTime;         // Cập nhật thời gian bước cuối
        }
    }
}