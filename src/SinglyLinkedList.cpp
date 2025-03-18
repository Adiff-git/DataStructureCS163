#include "SinglyLinkedList.h"
#include "DataStructureLogic.h"
#include "raylib.h"
#include <string>

SinglyLinkedList::SinglyLinkedList() : head(nullptr) {} // Hàm tạo mặc định, danh sách rỗng
SinglyLinkedList::~SinglyLinkedList() {                 // Hàm hủy
    while (head) {                                      // Duyệt qua danh sách để xóa từng node
        Node* temp = head;                              // Lưu con trỏ node đầu tiên
        head = head->next;                              // Chuyển sang node tiếp theo
        delete temp;                                    // Giải phóng bộ nhớ node
    }
    ClearStates();                                      // Xóa các trạng thái đã lưu
}

void* SinglyLinkedList::CopyState() {                   // Sao chép trạng thái danh sách
    if (!head) return nullptr;                          // Nếu danh sách rỗng, trả về nullptr
    Node* newHead = new Node(head->data);               // Tạo node mới cho đầu danh sách sao chép
    Node* current = newHead;                            // Con trỏ để xây dựng danh sách sao chép
    Node* source = head->next;                          // Bắt đầu từ node thứ hai của danh sách gốc
    while (source) {                                    // Duyệt qua danh sách gốc
        current->next = new Node(source->data);         // Tạo node mới và liên kết
        current = current->next;                        // Chuyển sang node vừa tạo
        source = source->next;                          // Chuyển sang node tiếp theo trong gốc
    }
    return newHead;                                     // Trả về đầu danh sách sao chép
}

void SinglyLinkedList::Draw(Font font, void* state, int x, int y) { // Vẽ danh sách lên màn hình
    Node* current = static_cast<Node*>(state);          // Ép kiểu trạng thái về con trỏ Node
    int offset = 0;                                     // Độ lệch để vẽ các node
    while (current) {                                   // Duyệt qua từng node
        DrawRectangle(x + offset, y, 50, 50, LIGHTGRAY); // Vẽ hình chữ nhật cho node
        DrawTextEx(font, TextFormat("%d", current->data), {static_cast<float>(x + offset + 10), static_cast<float>(y + 10)}, 30, 1, DARKBLUE); // Vẽ giá trị node
        if (current->next) DrawLine(x + offset + 50, y + 25, x + offset + 100, y + 25, BLACK); // Vẽ đường nối nếu có node tiếp theo
        current = current->next;                        // Chuyển sang node tiếp theo
        offset += 100;                                  // Tăng độ lệch để vẽ node tiếp theo
    }
}

void SinglyLinkedList::ClearStates() {                  // Xóa tất cả trạng thái đã lưu
    for (auto state : stepStates) {                     // Duyệt qua từng trạng thái
        Node* current = static_cast<Node*>(state);      // Ép kiểu trạng thái về con trỏ Node
        while (current) {                               // Duyệt qua từng node trong trạng thái
            Node* temp = current;                       // Lưu con trỏ node hiện tại
            current = current->next;                    // Chuyển sang node tiếp theo
            delete temp;                                // Giải phóng bộ nhớ node
        }
    }
}

void SinglyLinkedList::Initialize(int param) {          // Khởi tạo danh sách với các giá trị ngẫu nhiên
    ClearSteps();                                       // Xóa các bước cũ
    ResetAnimation();                                   // Đặt lại animation
    while (head) {                                      // Xóa danh sách hiện tại
        Node* temp = head;                              // Lưu con trỏ node đầu
        head = head->next;                              // Chuyển sang node tiếp theo
        delete temp;                                    // Giải phóng bộ nhớ
    }
    SaveStep("Initial state (empty)", CopyState());     // Lưu trạng thái ban đầu (rỗng)
    for (int i = 0; i < param; i++) Add(rand() % 100);  // Thêm param giá trị ngẫu nhiên
}

void SinglyLinkedList::Add(int value) {                 // Thêm giá trị vào cuối danh sách
    Node* newNode = new Node(value);                    // Tạo node mới với giá trị
    if (!head) {                                        // Nếu danh sách rỗng
        head = newNode;                                 // Gán node mới làm đầu
    } else {                                            // Nếu danh sách không rỗng
        Node* temp = head;                              // Bắt đầu từ đầu danh sách
        while (temp->next) temp = temp->next;           // Tìm node cuối cùng
        temp->next = newNode;                           // Liên kết node mới vào cuối
    }
    SaveStep("Added node: " + std::to_string(value), CopyState()); // Lưu bước thêm node
}

void SinglyLinkedList::Delete(int value) {              // Xóa node có giá trị cho trước
    if (!head) return;                                  // Nếu danh sách rỗng, thoát
    if (head->data == value) {                          // Nếu node đầu là giá trị cần xóa
        Node* temp = head;                              // Lưu con trỏ node đầu
        head = head->next;                              // Cập nhật đầu danh sách
        delete temp;                                    // Giải phóng bộ nhớ
        SaveStep("Deleted node: " + std::to_string(value), CopyState()); // Lưu bước xóa
        return;
    }
    Node* current = head;                               // Bắt đầu từ đầu danh sách
    while (current->next && current->next->data != value) current = current->next; // Tìm node trước giá trị cần xóa
    if (current->next) {                                // Nếu tìm thấy
        Node* temp = current->next;                     // Lưu con trỏ node cần xóa
        current->next = temp->next;                     // Bỏ qua node cần xóa
        delete temp;                                    // Giải phóng bộ nhớ
        SaveStep("Deleted node: " + std::to_string(value), CopyState()); // Lưu bước xóa
    }
}

void SinglyLinkedList::Update(int oldValue, int newValue) { // Cập nhật giá trị trong danh sách
    Node* current = head;                               // Bắt đầu từ đầu danh sách
    bool updated = false;                               // Biến kiểm tra cập nhật
    while (current) {                                   // Duyệt qua từng node
        if (current->data == oldValue) {                // Nếu tìm thấy giá trị cũ
            current->data = newValue;                   // Cập nhật thành giá trị mới
            updated = true;                             // Đánh dấu đã cập nhật
        }
        current = current->next;                        // Chuyển sang node tiếp theo
    }
    if (updated) SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState()); // Lưu bước cập nhật nếu có
}

bool SinglyLinkedList::Search(int value) {              // Tìm kiếm giá trị trong danh sách
    ClearSteps();                                       // Xóa các bước cũ
    Node* current = head;                               // Bắt đầu từ đầu danh sách
    int index = 0;                                      // Vị trí hiện tại
    while (current) {                                   // Duyệt qua từng node
        SaveStep("Searching at position " + std::to_string(index) + ": " + std::to_string(current->data), CopyState()); // Lưu bước tìm kiếm
        if (current->data == value) {                   // Nếu tìm thấy
            SaveStep("Found " + std::to_string(value), CopyState()); // Lưu bước tìm thấy
            return true;                                // Trả về true
        }
        current = current->next;                        // Chuyển sang node tiếp theo
        index++;                                        // Tăng vị trí
    }
    SaveStep("Not found: " + std::to_string(value), CopyState()); // Lưu bước không tìm thấy
    return false;                                       // Trả về false
}