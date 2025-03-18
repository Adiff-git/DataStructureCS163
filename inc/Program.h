#ifndef PROGRAM_H                                    // Bảo vệ chống bao gồm nhiều lần
#define PROGRAM_H

#include "GUI.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"
#include "raylib.h"

class Program {                                     // Lớp chính của chương trình
private:
    Screen currentScreen;                           // Màn hình hiện tại
    Font font;                                      // Font để vẽ văn bản
    bool buttonClicked;                             // Trạng thái nút được nhấn
    const char* buttonMessage;                      // Thông điệp từ nút
    SinglyLinkedList sll;                           // Đối tượng danh sách liên kết đơn
    HashTable ht;                                   // Đối tượng bảng băm
    AVLTree avl;                                    // Đối tượng cây AVL
    Graph graph;                                    // Đối tượng đồ thị

public:
    Program();                                      // Hàm tạo
    ~Program();                                     // Hàm hủy
    void Run();                                     // Chạy chương trình
};

#endif