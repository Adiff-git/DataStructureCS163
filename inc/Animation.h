#ifndef ANIMATION_H                                  // Bảo vệ chống bao gồm nhiều lần
#define ANIMATION_H

#include "raylib.h"
#include <vector>
#include <string>

class Animation {                                   // Lớp cơ sở quản lý animation
protected:
    std::vector<std::string> steps;                 // Danh sách mô tả các bước
    std::vector<void*> stepStates;                  // Danh sách trạng thái các bước
    int currentStep;                                // Bước hiện tại
    bool isRunning;                                 // Trạng thái chạy/tạm dừng
    double lastStepTime;                            // Thời gian của bước cuối
    double animationSpeed;                          // Tốc độ animation (giây/bước)
// 
public:
    Animation();                                    // Hàm tạo
    virtual ~Animation();                           // Hàm hủy ảo
    void ClearSteps();                              // Xóa các bước và trạng thái
    void SaveStep(const std::string& description, void* state); // Lưu một bước mới
    int GetTotalSteps() const { return steps.size(); } // Lấy tổng số bước
    void ResetAnimation();                          // Đặt lại animation
    void StartAnimation() { isRunning = true; }     // Bắt đầu animation
    void PauseAnimation() { isRunning = false; }    // Tạm dừng animation
    void NextStep();                                // Chuyển sang bước tiếp theo
    void PrevStep();                                // Quay lại bước trước
    int GetCurrentStep() const { return currentStep; } // Lấy bước hiện tại
    bool IsRunning() const { return isRunning; }    // Kiểm tra trạng thái chạy
    void SetAnimationSpeed(double speed) { animationSpeed = speed; } // Đặt tốc độ animation
    double GetAnimationSpeed() const { return animationSpeed; } // Lấy tốc độ animation
    virtual void DrawAnimation(Font font, int x, int y) = 0; // Vẽ animation (ảo)
    virtual void UpdateAnimation() = 0;             // Cập nhật animation (ảo)
    std::vector<std::string>& GetSteps() { return steps; } // Trả về tham chiếu đến danh sách bước
};
// tại sao lại sử dụng virtual?

#endif