#ifndef GUI_H                                        // Bảo vệ chống bao gồm nhiều lần
#define GUI_H

#include "raylib.h"
#include "DataStructureLogic.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"

enum class Screen {                                 // Enum định nghĩa các màn hình
    MAIN_MENU,                                      // Menu chính
    SETTINGS,                                       // Cài đặt
    DATA_STRUCTURES,                                // Chọn cấu trúc dữ liệu
    SINGLY_LINKED_LIST,                             // Danh sách liên kết đơn
    HASH_TABLE,                                     // Bảng băm
    AVL_TREE,                                       // Cây AVL
    GRAPH                                           // Đồ thị
};

void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen); // Vẽ menu chính
void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen); // Vẽ menu cài đặt
void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen); // Vẽ menu chọn cấu trúc dữ liệu
void DrawSinglyLinkedListScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, SinglyLinkedList* sll); // Vẽ màn hình danh sách liên kết
void DrawHashTableScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, HashTable* ht); // Vẽ màn hình bảng băm
void DrawAVLTreeScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, AVLTree* avl); // Vẽ màn hình cây AVL
void DrawGraphScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, Graph* graph); // Vẽ màn hình đồ thị
void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen); // Vẽ nút quay lại
void DrawAnimationControls(Font font, bool& buttonClicked, const char*& buttonMessage, DataStructureLogic* ds); // Vẽ nút điều khiển animation

#endif