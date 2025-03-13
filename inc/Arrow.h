#ifndef ARROW_H
#define ARROW_H

#include "raylib.h"
#include <string>
#include <cmath>

// Lớp Arrow dùng cho trực quan hóa dữ liệu
class Arrow {
public:
    // Constructor: nhận điểm bắt đầu, kết thúc, màu sắc và nhãn (tùy chọn)
    Arrow(Vector2 start, Vector2 end, Color color = BLACK, const std::string& label = "");

    // Hàm vẽ mũi tên
    void Draw();

    // Thiết lập điểm bắt đầu và kết thúc
    void SetStart(Vector2 start);
    void SetEnd(Vector2 end);

    // Lấy điểm bắt đầu và kết thúc
    Vector2 GetStart() const;
    Vector2 GetEnd() const;

private:
    Vector2 start;    // Điểm bắt đầu của mũi tên
    Vector2 end;      // Điểm kết thúc của mũi tên
    Color color;      // Màu sắc của mũi tên
    std::string label;// Nhãn hiển thị giá trị của mũi tên (nếu cần)
};

#endif // ARROW_H
