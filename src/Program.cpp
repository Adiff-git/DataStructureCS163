#include "Program.h"
#include "GUI.h"
#include "raylib.h"
#include "SettingsManager.h"

Program::Program() : currentScreen(Screen::MAIN_MENU), buttonClicked(false), buttonMessage(""), ht(10), graph(5) { // Hàm tạo
    InitWindow(1400, 800, "Data Structure Visualization"); 
    SetTargetFPS(60);                          
    // font = LoadFont("../resources/Rubik-Italic-VariableFont_wght.ttf"); // Tải font
    font = GetFontDefault();                    
}

Program::~Program() {                           // Hàm hủy
    UnloadFont(font);                           // Giải phóng font
    CloseWindow();                              // Đóng cửa sổ
}

void Program::Run() {                           // Chạy chương trình
    while (!WindowShouldClose()) {              // Vòng lặp chính cho đến khi đóng cửa sổ
        BeginDrawing();                        
        sll.SetAnimationSpeed(animationSpeed);  
        ht.SetAnimationSpeed(animationSpeed);   
        avl.SetAnimationSpeed(animationSpeed);  
        graph.SetAnimationSpeed(animationSpeed); 

        switch (currentScreen) {                // Chuyển đổi giữa các màn hình
            case Screen::MAIN_MENU:             // Màn hình menu chính
                DrawMainMenu(font, buttonClicked, buttonMessage, currentScreen); 
                break;
            case Screen::SETTINGS:              // Màn hình cài đặt
                DrawSettingsMenu(font, buttonClicked, buttonMessage, currentScreen); 
                break;
            case Screen::DATA_STRUCTURES:       // Màn hình chọn cấu trúc dữ liệu
                DrawDataStructuresMenu(font, buttonClicked, buttonMessage, currentScreen); 
                break;
            case Screen::SINGLY_LINKED_LIST:    // Màn hình danh sách liên kết
                DrawSinglyLinkedListScreen(font, buttonClicked, buttonMessage, currentScreen, &sll); 
                sll.UpdateAnimation();          // Cập nhật animation
                break;
            case Screen::HASH_TABLE:            // Màn hình bảng băm
                DrawHashTableScreen(font, buttonClicked, buttonMessage, currentScreen, &ht); 
                ht.UpdateAnimation();           // Cập nhật animation
                break;
            case Screen::AVL_TREE:              // Màn hình cây AVL
                DrawAVLTreeScreen(font, buttonClicked, buttonMessage, currentScreen, &avl); 
                avl.UpdateAnimation();          // Cập nhật animation
                break;
            case Screen::GRAPH:                 // Màn hình đồ thị
                DrawGraphScreen(font, buttonClicked, buttonMessage, currentScreen, &graph);
                graph.UpdateAnimation();        // Cập nhật animation
                break;
        }
        if (buttonClicked) {                    // Nếu có nút được nhấn
            buttonClicked = false;              // Đặt lại trạng thái nhấn nút
        }
        EndDrawing();                           // Kết thúc vẽ khung hình
    }
}