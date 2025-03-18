#ifndef GRAPH_H                                      // Bảo vệ chống bao gồm nhiều lần
#define GRAPH_H

#include "DataStructureLogic.h"
#include "raylib.h"
#include <vector>
#include <string>

class Graph : public DataStructureLogic {              // Lớp Graph kế thừa từ DataStructureLogic
private:
    struct Edge {                                      // Cấu trúc cạnh trong đồ thị
        int to;                                        // Đỉnh đích
        int weight;                                    // Trọng số cạnh
        Edge* next;                                    // Con trỏ tới cạnh tiếp theo
        Edge(int t, int w) : to(t), weight(w), next(nullptr) {} // Hàm tạo với đích và trọng số
    };
    struct Vertex {                                    // Cấu trúc đỉnh trong đồ thị
        int data;                                      // Giá trị của đỉnh
        Edge* edges;                                   // Danh sách các cạnh xuất phát từ đỉnh
        Vertex() : data(-1), edges(nullptr) {}         // Hàm tạo mặc định
        Vertex(int d) : data(d), edges(nullptr) {}     // Hàm tạo với giá trị
    };
    std::vector<Vertex> vertices;                      // Vector lưu các đỉnh
    int size;                                          // Số lượng đỉnh
    Edge* CopyEdges(Edge* source);                     // Sao chép danh sách cạnh
    void* CopyState() override;                        // Sao chép trạng thái đồ thị
    void Draw(Font font, void* state, int x, int y) override; // Vẽ đồ thị lên màn hình
    void ClearStates() override;                       // Xóa các trạng thái đã lưu

public:
    Graph(int size);                                   // Hàm tạo với số lượng đỉnh
    ~Graph() override;                                 // Hàm hủy
    void Initialize(int param) override;               // Khởi tạo đồ thị
    void Add(int value) override;                      // Thêm đỉnh (không hỗ trợ)
    void AddEdge(int from, int to, int weight);        // Thêm cạnh
    void Delete(int value) override;                   // Xóa đỉnh
    void Update(int oldValue, int newValue) override;  // Cập nhật giá trị đỉnh
    bool Search(int value) override;                   // Tìm kiếm giá trị trong đồ thị
};

#endif