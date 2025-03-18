#include "HashTable.h"
#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>

HashTable::HashTable(int size) : size(size) {   // Hàm tạo với kích thước bảng băm
    table.resize(size, nullptr);                // Khởi tạo bảng với size slot, tất cả là nullptr
}

HashTable::~HashTable() {                       // Hàm hủy
    for (int i = 0; i < size; i++) {            // Duyệt qua từng slot
        HashNode* current = table[i];           // Lấy danh sách liên kết tại slot i
        while (current) {                       // Duyệt qua từng node
            HashNode* temp = current;           // Lưu con trỏ node hiện tại
            current = current->next;            // Chuyển sang node tiếp theo
            delete temp;                        // Giải phóng bộ nhớ
        }
    }
    ClearStates();                              // Xóa các trạng thái đã lưu
}

void* HashTable::CopyState() {                  // Sao chép trạng thái bảng băm
    std::vector<HashNode*>* state = new std::vector<HashNode*>; // Tạo vector mới để lưu trạng thái
    state->resize(size, nullptr);               // Điều chỉnh kích thước vector
    for (int i = 0; i < size; i++) {            // Duyệt qua từng slot
        HashNode* newHead = nullptr;            // Đầu danh sách mới
        HashNode* current = nullptr;            // Con trỏ hiện tại trong danh sách mới
        HashNode* source = table[i];            // Danh sách nguồn tại slot i
        while (source) {                        // Sao chép từng node
            HashNode* newNode = new HashNode(source->key); // Tạo node mới với cùng key
            if (!newHead) {                     // Nếu chưa có đầu
                newHead = newNode;              // Gán node mới làm đầu
                current = newHead;              // Cập nhật con trỏ hiện tại
            } else {                            // Nếu đã có đầu
                current->next = newNode;        // Liên kết node mới
                current = current->next;        // Cập nhật con trỏ hiện tại
            }
            source = source->next;              // Chuyển sang node tiếp theo trong nguồn
        }
        (*state)[i] = newHead;                  // Gán danh sách mới vào trạng thái
    }
    return state;                               // Trả về trạng thái
}

void HashTable::Draw(Font font, void* state, int x, int y) { // Vẽ bảng băm lên màn hình
    std::vector<HashNode*>* tableState = static_cast<std::vector<HashNode*>*>(state); // Ép kiểu trạng thái
    for (int i = 0; i < size; i++) {            // Duyệt qua từng slot
        DrawTextEx(font, TextFormat("Slot %d:", i), {static_cast<float>(x), static_cast<float>(y + i * 60)}, 30, 1, DARKBLUE); // Vẽ nhãn slot
        HashNode* current = (*tableState)[i];   // Lấy danh sách node từ trạng thái
        int offset = 100;                       // Độ lệch để vẽ các node
        while (current) {                       // Duyệt qua từng node
            DrawRectangle(x + offset, y + i * 60, 50, 50, LIGHTGRAY); // Vẽ hình chữ nhật cho node
            DrawTextEx(font, TextFormat("%d", current->key), {static_cast<float>(x + offset + 10), static_cast<float>(y + i * 60 + 10)}, 30, 1, DARKBLUE); // Vẽ giá trị node
            if (current->next) DrawLine(x + offset + 50, y + i * 60 + 25, x + offset + 100, y + i * 60 + 25, BLACK); // Vẽ đường nối nếu có node tiếp theo
            current = current->next;            // Chuyển sang node tiếp theo
            offset += 100;                      // Tăng độ lệch
        }
    }
}

void HashTable::ClearStates() {                 // Xóa tất cả trạng thái đã lưu
    for (auto state : stepStates) {             // Duyệt qua từng trạng thái
        std::vector<HashNode*>* tableState = static_cast<std::vector<HashNode*>*>(state); // Ép kiểu trạng thái
        for (auto node : *tableState) {         // Duyệt qua từng danh sách node
            while (node) {                      // Duyệt qua từng node
                HashNode* temp = node;          // Lưu con trỏ node hiện tại
                node = node->next;              // Chuyển sang node tiếp theo
                delete temp;                    // Giải phóng bộ nhớ
            }
        }
        delete tableState;                      // Giải phóng vector trạng thái
    }
}

void HashTable::Initialize(int param) {         // Khởi tạo bảng băm với các giá trị ngẫu nhiên
    ClearSteps();                               // Xóa các bước cũ
    ResetAnimation();                           // Đặt lại animation
    for (int i = 0; i < size; i++) {            // Duyệt qua từng slot
        while (table[i]) {                      // Xóa danh sách hiện có
            HashNode* temp = table[i];          // Lưu con trỏ node hiện tại
            table[i] = temp->next;              // Chuyển sang node tiếp theo
            delete temp;                        // Giải phóng bộ nhớ
        }
    }
    SaveStep("Initial state (empty)", CopyState()); // Lưu trạng thái ban đầu
    for (int i = 0; i < param; i++) Add(rand() % 100); // Thêm param giá trị ngẫu nhiên
}

void HashTable::Add(int value) {                // Thêm giá trị vào bảng băm
    int index = hash(value);                    // Tính chỉ số slot bằng hàm hash
    HashNode* newNode = new HashNode(value);    // Tạo node mới với giá trị
    if (!table[index]) {                        // Nếu slot trống
        table[index] = newNode;                 // Gán node mới vào slot
    } else {                                    // Nếu slot đã có node
        HashNode* current = table[index];       // Bắt đầu từ node đầu tiên
        while (current->next) current = current->next; // Tìm node cuối cùng
        current->next = newNode;                // Thêm node mới vào cuối
    }
    SaveStep("Added " + std::to_string(value) + " to slot " + std::to_string(index), CopyState()); // Lưu bước thêm
}

void HashTable::Delete(int value) {             // Xóa giá trị khỏi bảng băm
    int index = hash(value);                    // Tính chỉ số slot
    if (!table[index]) return;                  // Nếu slot trống, thoát
    if (table[index]->key == value) {           // Nếu node đầu tiên là giá trị cần xóa
        HashNode* temp = table[index];          // Lưu con trỏ node đầu
        table[index] = temp->next;              // Cập nhật slot
        delete temp;                            // Giải phóng bộ nhớ
        SaveStep("Deleted " + std::to_string(value) + " from slot " + std::to_string(index), CopyState()); // Lưu bước xóa
        return;
    }
    HashNode* current = table[index];           // Bắt đầu từ node đầu tiên
    while (current->next && current->next->key != value) current = current->next; // Tìm node trước giá trị cần xóa
    if (current->next) {                        // Nếu tìm thấy
        HashNode* temp = current->next;         // Lưu con trỏ node cần xóa
        current->next = temp->next;             // Bỏ qua node cần xóa
        delete temp;                            // Giải phóng bộ nhớ
        SaveStep("Deleted " + std::to_string(value) + " from slot " + std::to_string(index), CopyState()); // Lưu bước xóa
    }
}

void HashTable::Update(int oldValue, int newValue) { // Cập nhật giá trị trong bảng băm
    int index = hash(oldValue);                 // Tính chỉ số slot
    HashNode* current = table[index];           // Bắt đầu từ node đầu tiên
    bool updated = false;                       // Biến kiểm tra cập nhật
    while (current) {                           // Duyệt qua từng node
        if (current->key == oldValue) {         // Nếu tìm thấy giá trị cũ
            current->key = newValue;            // Cập nhật thành giá trị mới
            updated = true;                     // Đánh dấu đã cập nhật
        }
        current = current->next;                // Chuyển sang node tiếp theo
    }
    if (updated) SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState()); // Lưu bước cập nhật nếu có
}

bool HashTable::Search(int value) {             // Tìm kiếm giá trị trong bảng băm
    ClearSteps();                               // Xóa các bước cũ
    int index = hash(value);                    // Tính chỉ số slot
    SaveStep("Searching in slot " + std::to_string(index), CopyState()); // Lưu bước tìm kiếm slot
    HashNode* current = table[index];           // Bắt đầu từ node đầu tiên
    while (current) {                           // Duyệt qua từng node
        SaveStep("Checking value: " + std::to_string(current->key), CopyState()); // Lưu bước kiểm tra giá trị
        if (current->key == value) {            // Nếu tìm thấy
            SaveStep("Found " + std::to_string(value), CopyState()); // Lưu bước tìm thấy
            return true;                        // Trả về true
        }
        current = current->next;                // Chuyển sang node tiếp theo
    }
    SaveStep("Not found: " + std::to_string(value), CopyState()); // Lưu bước không tìm thấy
    return false;                               // Trả về false
}