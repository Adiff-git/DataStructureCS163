#include "Graph.h"
#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>

const float _PI = 3.14159265358979323846f; // Định nghĩa hằng số PI để tính toán góc trong đồ thị

Graph::Graph(int size) : size(size) {       // Hàm tạo với tham số size để khởi tạo đồ thị
    vertices.resize(size);                  // Điều chỉnh kích thước vector vertices theo size
    for (int i = 0; i < size; i++) {        // Duyệt qua từng đỉnh
        vertices[i] = Vertex(i);            // Khởi tạo mỗi đỉnh với giá trị i
    }
}

Graph::~Graph() {                           // Hàm hủy để giải phóng bộ nhớ
    for (int i = 0; i < size; i++) {        // Duyệt qua từng đỉnh
        Edge* current = vertices[i].edges;  // Lấy danh sách cạnh của đỉnh i
        while (current) {                   // Duyệt qua từng cạnh trong danh sách
            Edge* temp = current;           // Lưu con trỏ cạnh hiện tại để xóa
            current = current->next;        // Chuyển sang cạnh tiếp theo
            delete temp;                    // Giải phóng bộ nhớ của cạnh
        }
    }
    ClearStates();                          // Xóa các trạng thái đã lưu của đồ thị
}

Graph::Edge* Graph::CopyEdges(Edge* source) { // Sao chép danh sách cạnh
    if (!source) return nullptr;              // Nếu không có cạnh, trả về nullptr
    Edge* newEdge = new Edge(source->to, source->weight); // Tạo cạnh mới với cùng đích và trọng số
    newEdge->next = CopyEdges(source->next);  // Sao chép đệ quy cạnh tiếp theo
    return newEdge;                           // Trả về con trỏ tới cạnh mới
}

void* Graph::CopyState() {                    // Sao chép trạng thái hiện tại của đồ thị
    std::vector<Edge*>* state = new std::vector<Edge*>; // Tạo vector mới để lưu trạng thái
    state->resize(size, nullptr);             // Điều chỉnh kích thước vector theo số đỉnh
    for (int i = 0; i < size; i++) {          // Duyệt qua từng đỉnh
        (*state)[i] = CopyEdges(vertices[i].edges); // Sao chép danh sách cạnh của đỉnh i
    }
    return state;                             // Trả về trạng thái dưới dạng con trỏ void*
}

void Graph::Draw(Font font, void* state, int x, int y) { // Vẽ đồ thị lên màn hình
    std::vector<Edge*>* edgesState = static_cast<std::vector<Edge*>*>(state); // Ép kiểu trạng thái về vector cạnh
    const float radius = 200.0f;              // Bán kính vòng tròn để đặt các đỉnh
    for (int i = 0; i < size; i++) {          // Duyệt qua từng đỉnh
        float angle = 2.0f * _PI * i / size;  // Tính góc để đặt đỉnh trên vòng tròn
        int vx = static_cast<int>(x + radius * cos(angle)); // Tọa độ x của đỉnh
        int vy = static_cast<int>(y + radius * sin(angle)); // Tọa độ y của đỉnh
        DrawCircle(vx, vy, 25, LIGHTGRAY);    // Vẽ hình tròn biểu diễn đỉnh
        DrawTextEx(font, TextFormat("%d", vertices[i].data), {static_cast<float>(vx - 10), static_cast<float>(vy - 10)}, 30, 1, DARKBLUE); // Vẽ giá trị của đỉnh

        Edge* edge = (*edgesState)[i];        // Lấy danh sách cạnh của đỉnh i từ trạng thái
        while (edge) {                        // Duyệt qua từng cạnh
            float toAngle = 2.0f * _PI * edge->to / size; // Tính góc của đỉnh đích
            int tx = static_cast<int>(x + radius * cos(toAngle)); // Tọa độ x của đỉnh đích
            int ty = static_cast<int>(y + radius * sin(toAngle)); // Tọa độ y của đỉnh đích
            DrawLine(vx, vy, tx, ty, BLACK);  // Vẽ đường nối giữa hai đỉnh
            DrawTextEx(font, TextFormat("%d", edge->weight), {static_cast<float>((vx + tx) / 2), static_cast<float>((vy + ty) / 2)}, 20, 1, RED); // Vẽ trọng số của cạnh
            edge = edge->next;                // Chuyển sang cạnh tiếp theo
        }
    }
}

void Graph::ClearStates() {                   // Xóa tất cả trạng thái đã lưu
    for (auto state : stepStates) {           // Duyệt qua từng trạng thái
        std::vector<Edge*>* edgesState = static_cast<std::vector<Edge*>*>(state); // Ép kiểu về vector cạnh
        for (auto edge : *edgesState) {       // Duyệt qua từng danh sách cạnh
            while (edge) {                    // Duyệt qua từng cạnh trong danh sách
                Edge* temp = edge;            // Lưu con trỏ cạnh hiện tại
                edge = edge->next;            // Chuyển sang cạnh tiếp theo
                delete temp;                  // Giải phóng bộ nhớ của cạnh
            }
        }
        delete edgesState;                    // Giải phóng vector trạng thái
    }
}

void Graph::Initialize(int param) {           // Khởi tạo đồ thị với các cạnh mẫu
    ClearSteps();                             // Xóa các bước đã lưu
    ResetAnimation();                         // Đặt lại trạng thái animation
    for (int i = 0; i < size; i++) {          // Duyệt qua từng đỉnh
        while (vertices[i].edges) {           // Xóa tất cả cạnh hiện có của đỉnh
            Edge* temp = vertices[i].edges;   // Lưu con trỏ cạnh hiện tại
            vertices[i].edges = temp->next;   // Chuyển sang cạnh tiếp theo
            delete temp;                      // Giải phóng bộ nhớ
        }
    }
    SaveStep("Initial state (empty)", CopyState()); // Lưu trạng thái ban đầu (rỗng)
    AddEdge(0, 1, 4);                         // Thêm cạnh từ 0 đến 1 với trọng số 4
    AddEdge(1, 2, 5);                         // Thêm cạnh từ 1 đến 2 với trọng số 5
    AddEdge(2, 3, 6);                         // Thêm cạnh từ 2 đến 3 với trọng số 6
    AddEdge(3, 4, 7);                         // Thêm cạnh từ 3 đến 4 với trọng số 7
}

void Graph::Add(int value) {                  // Thêm đỉnh (không hỗ trợ trong đồ thị này)
    SaveStep("Add operation not supported for Graph", CopyState()); // Lưu thông báo không hỗ trợ
}

void Graph::AddEdge(int from, int to, int weight) { // Thêm một cạnh vào đồ thị
    Edge* newEdge = new Edge(to, weight);     // Tạo cạnh mới với đích và trọng số
    if (!vertices[from].edges) vertices[from].edges = newEdge; // Nếu chưa có cạnh, gán trực tiếp
    else {                                    // Nếu đã có cạnh
        Edge* current = vertices[from].edges; // Bắt đầu từ cạnh đầu tiên
        while (current->next) current = current->next; // Tìm cạnh cuối cùng
        current->next = newEdge;              // Thêm cạnh mới vào cuối danh sách
    }
    SaveStep("Added edge " + std::to_string(from) + " -> " + std::to_string(to), CopyState()); // Lưu bước thêm cạnh
}

void Graph::Delete(int value) {               // Xóa đỉnh và các cạnh liên quan
    for (int i = 0; i < size; i++) {          // Duyệt qua từng đỉnh
        Edge* current = vertices[i].edges;    // Lấy danh sách cạnh của đỉnh i
        Edge* prev = nullptr;                 // Con trỏ cạnh trước đó
        while (current) {                     // Duyệt qua từng cạnh
            if (current->to == value || i == value) { // Nếu cạnh đến đỉnh cần xóa hoặc đỉnh i là đỉnh cần xóa
                if (prev) {                   // Nếu có cạnh trước đó
                    prev->next = current->next; // Bỏ qua cạnh hiện tại
                    delete current;           // Giải phóng bộ nhớ
                    current = prev->next;     // Tiếp tục với cạnh tiếp theo
                } else {                      // Nếu đây là cạnh đầu tiên
                    vertices[i].edges = current->next; // Cập nhật danh sách cạnh
                    delete current;           // Giải phóng bộ nhớ
                    current = vertices[i].edges; // Tiếp tục với cạnh mới
                }
            } else {                          // Nếu không cần xóa
                prev = current;               // Cập nhật prev
                current = current->next;      // Chuyển sang cạnh tiếp theo
            }
        }
    }
    SaveStep("Deleted vertex " + std::to_string(value), CopyState()); // Lưu bước xóa đỉnh
}

void Graph::Update(int oldValue, int newValue) { // Cập nhật giá trị của đỉnh
    for (int i = 0; i < size; i++) {          // Duyệt qua từng đỉnh
        Edge* current = vertices[i].edges;    // Lấy danh sách cạnh
        while (current) {                     // Duyệt qua từng cạnh
            if (current->to == oldValue) current->to = newValue; // Cập nhật đích nếu trùng
            current = current->next;          // Chuyển sang cạnh tiếp theo
        }
        if (i == oldValue) vertices[i].data = newValue; // Cập nhật giá trị đỉnh nếu trùng
    }
    SaveStep("Updated vertex " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState()); // Lưu bước cập nhật
}

bool Graph::Search(int value) {               // Tìm kiếm giá trị trong đồ thị
    ClearSteps();                             // Xóa các bước cũ
    for (int i = 0; i < size; i++) {          // Duyệt qua từng đỉnh
        if (vertices[i].data == value) {      // Nếu tìm thấy ở đỉnh
            SaveStep("Found vertex " + std::to_string(value), CopyState()); // Lưu bước tìm thấy
            return true;                      // Trả về true
        }
        Edge* current = vertices[i].edges;    // Lấy danh sách cạnh
        while (current) {                     // Duyệt qua từng cạnh
            if (current->to == value) {       // Nếu tìm thấy ở đích của cạnh
                SaveStep("Found edge to " + std::to_string(value), CopyState()); // Lưu bước tìm thấy
                return true;                  // Trả về true
            }
            current = current->next;          // Chuyển sang cạnh tiếp theo
        }
    }
    SaveStep("Vertex " + std::to_string(value) + " not found", CopyState()); // Lưu bước không tìm thấy
    return false;                             // Trả về false
}