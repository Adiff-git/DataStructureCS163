#include "GUI.h"
#include "Button.h"
#include "SettingsManager.h"
#include "SinglyLinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "Graph.h"
#include "raylib.h"

static SinglyLinkedList sll;                    // Khởi tạo danh sách liên kết đơn
static HashTable ht(10);                        // Khởi tạo bảng băm với 10 slot
static AVLTree avl;                             // Khởi tạo cây AVL
static Graph graph(5);                          // Khởi tạo đồ thị với 5 đỉnh

// Biến toàn cục để lưu giá trị nhập từ bàn phím
static char inputBuffer1[32] = "";              // Bộ đệm cho giá trị đầu tiên (oldValue hoặc giá trị chung)
static char inputBuffer2[32] = "";              // Bộ đệm cho giá trị thứ hai (newValue cho Update)
static int inputLength1 = 0;                    // Độ dài chuỗi nhập buffer1
static int inputLength2 = 0;                    // Độ dài chuỗi nhập buffer2
static bool inputActive1 = false;               // Trạng thái ô nhập liệu 1 đang hoạt động
static bool inputActive2 = false;               // Trạng thái ô nhập liệu 2 đang hoạt động

// Hàm xử lý nhập liệu từ bàn phím cho ô 1
void HandleInput1() {
    int key = GetCharPressed();                 // Lấy ký tự được nhấn
    while (key > 0) {                           // Xử lý tất cả ký tự được nhấn
        if ((key >= 48 && key <= 57) && inputLength1 < 31) { // Chỉ chấp nhận số (0-9)
            inputBuffer1[inputLength1] = (char)key; // Thêm ký tự vào bộ đệm 1
            inputLength1++;                     // Tăng độ dài
            inputBuffer1[inputLength1] = '\0';  // Kết thúc chuỗi
        }
        key = GetCharPressed();                 // Lấy ký tự tiếp theo
    }
    if (IsKeyPressed(KEY_BACKSPACE) && inputLength1 > 0) { // Xóa ký tự cuối nếu nhấn Backspace
        inputLength1--;                         // Giảm độ dài
        inputBuffer1[inputLength1] = '\0';      // Cập nhật kết thúc chuỗi
    }
}

// Hàm xử lý nhập liệu từ bàn phím cho ô 2
void HandleInput2() {
    int key = GetCharPressed();                 // Lấy ký tự được nhấn
    while (key > 0) {                           // Xử lý tất cả ký tự được nhấn
        if ((key >= 48 && key <= 57) && inputLength2 < 31) { // Chỉ chấp nhận số (0-9)
            inputBuffer2[inputLength2] = (char)key; // Thêm ký tự vào bộ đệm 2
            inputLength2++;                     // Tăng độ dài
            inputBuffer2[inputLength2] = '\0';  // Kết thúc chuỗi
        }
        key = GetCharPressed();                 // Lấy ký tự tiếp theo
    }
    if (IsKeyPressed(KEY_BACKSPACE) && inputLength2 > 0) { // Xóa ký tự cuối nếu nhấn Backspace
        inputLength2--;                         // Giảm độ dài
        inputBuffer2[inputLength2] = '\0';      // Cập nhật kết thúc chuỗi
    }
}

// Hàm chuyển chuỗi nhập thành số nguyên
int GetInputValue1() {
    if (inputLength1 == 0) return 0;            // Nếu không có nhập liệu, trả về 0
    return atoi(inputBuffer1);                  // Chuyển chuỗi thành số nguyên
}

int GetInputValue2() {
    if (inputLength2 == 0) return 0;            // Nếu không có nhập liệu, trả về 0
    return atoi(inputBuffer2);                  // Chuyển chuỗi thành số nguyên
}

// Hàm vẽ ô nhập liệu
void DrawInputBox(Font font, float x, float y, char* buffer, bool active, int length) {
    DrawRectangle(x, y, 200, 50, WHITE);        // Vẽ hình chữ nhật cho ô nhập liệu
    DrawRectangleLines(x, y, 200, 50, BLACK);   // Vẽ viền
    DrawTextEx(font, buffer, {x + 10, y + 10}, 30, 1, BLACK); // Vẽ chuỗi nhập liệu
    if (active && (GetTime() - (int)GetTime()) < 0.5) { // Nhấp nháy con trỏ
        DrawTextEx(font, "|", {x + 10 + MeasureTextEx(font, buffer, 30, 1).x, y + 10}, 30, 1, BLACK);
    }
}

// Các hàm vẽ menu không thay đổi
void DrawMainMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(BLUE);
    DrawTextEx(font, "Data Structure Visualization", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float buttonX = 600.0f;
    float buttonY = 200.0f;
    if (DrawButton("Data Structures", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::DATA_STRUCTURES;
    }
    if (DrawButton("Settings", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::SETTINGS;
    }
    if (DrawButton("Exit", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        CloseWindow();
    }
}

void DrawSettingsMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(GREEN);
    DrawTextEx(font, "Settings", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Increase Speed", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        animationSpeed -= 0.1;
        if (animationSpeed < 0.1) animationSpeed = 0.1;
    }
    if (DrawButton("Decrease Speed", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        animationSpeed += 0.1;
        if (animationSpeed > 2.0) animationSpeed = 2.0;
    }

    DrawTextEx(font, TextFormat("Animation Speed: %.1f", animationSpeed), { 50.0f, 300.0f }, 30, 1, DARKGRAY);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawDataStructuresMenu(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    ClearBackground(YELLOW);
    DrawTextEx(font, "Choose a Data Structure", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    float buttonX = 600.0f;
    float buttonY = 150.0f;
    if (DrawButton("Singly Linked List", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::SINGLY_LINKED_LIST;
    }
    if (DrawButton("Hash Table", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::HASH_TABLE;
    }
    if (DrawButton("AVL Tree", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::AVL_TREE;
    }
    if (DrawButton("Graph", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        currentScreen = Screen::GRAPH;
    }
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

// Các hàm vẽ màn hình cấu trúc dữ liệu với hai ô nhập liệu
void DrawSinglyLinkedListScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, SinglyLinkedList* sll) {
    ClearBackground(ORANGE);
    DrawTextEx(font, "Singly Linked List", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    // Vẽ hai ô nhập liệu
    float inputX1 = 50.0f;
    float inputY1 = 100.0f;
    float inputX2 = 270.0f;
    float inputY2 = 100.0f;
    DrawInputBox(font, inputX1, inputY1, inputBuffer1, inputActive1, inputLength1); // Ô 1: Giá trị chung hoặc oldValue
    DrawInputBox(font, inputX2, inputY2, inputBuffer2, inputActive2, inputLength2); // Ô 2: newValue cho Update

    // Xử lý kích hoạt ô nhập liệu
    if (CheckCollisionPointRec(GetMousePosition(), {inputX1, inputY1, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = true; inputActive2 = false; // Kích hoạt ô 1, tắt ô 2
    } else if (CheckCollisionPointRec(GetMousePosition(), {inputX2, inputY2, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = true; // Kích hoạt ô 2, tắt ô 1
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = false; // Tắt cả hai nếu nhấp ra ngoài
    }
    if (inputActive1) HandleInput1();           // Xử lý nhập liệu cho ô 1
    if (inputActive2) HandleInput2();           // Xử lý nhập liệu cho ô 2

    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();           // Lấy giá trị từ ô 1
        sll->Initialize(value);
        sll->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0'; // Xóa ô 1
    }
    if (DrawButton("Add", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();           // Lấy giá trị từ ô 1
        sll->Add(value);
        sll->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0'; // Xóa ô 1
    }
    if (DrawButton("Delete", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();           // Lấy giá trị từ ô 1
        sll->Delete(value);
        sll->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0'; // Xóa ô 1
    }
    if (DrawButton("Update", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        int oldValue = GetInputValue1();        // Lấy giá trị cũ từ ô 1
        int newValue = GetInputValue2();        // Lấy giá trị mới từ ô 2
        sll->Update(oldValue, newValue);        // Cập nhật từ oldValue sang newValue
        sll->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0'; // Xóa ô 1
        inputLength2 = 0; inputBuffer2[0] = '\0'; // Xóa ô 2
    }
    if (DrawButton("Search", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();           // Lấy giá trị từ ô 1
        sll->Search(value);
        sll->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0'; // Xóa ô 1
    }
    sll->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, sll);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawHashTableScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, HashTable* ht) {
    ClearBackground(LIME);
    DrawTextEx(font, "Hash Table", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    // Vẽ hai ô nhập liệu
    float inputX1 = 50.0f;
    float inputY1 = 100.0f;
    float inputX2 = 270.0f;
    float inputY2 = 100.0f;
    DrawInputBox(font, inputX1, inputY1, inputBuffer1, inputActive1, inputLength1);
    DrawInputBox(font, inputX2, inputY2, inputBuffer2, inputActive2, inputLength2);

    // Xử lý kích hoạt ô nhập liệu
    if (CheckCollisionPointRec(GetMousePosition(), {inputX1, inputY1, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = true; inputActive2 = false;
    } else if (CheckCollisionPointRec(GetMousePosition(), {inputX2, inputY2, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = false;
    }
    if (inputActive1) HandleInput1();
    if (inputActive2) HandleInput2();

    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        ht->Initialize(value);
        ht->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Add", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        ht->Add(value);
        ht->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Delete", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        ht->Delete(value);
        ht->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Update", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        int oldValue = GetInputValue1();
        int newValue = GetInputValue2();
        ht->Update(oldValue, newValue);         // Cập nhật từ oldValue sang newValue
        ht->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
        inputLength2 = 0; inputBuffer2[0] = '\0';
    }
    if (DrawButton("Search", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        ht->Search(value);
        ht->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    ht->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, ht);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawAVLTreeScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, AVLTree* avl) {
    ClearBackground(RED);
    DrawTextEx(font, "AVL Tree", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    // Vẽ hai ô nhập liệu
    float inputX1 = 50.0f;
    float inputY1 = 100.0f;
    float inputX2 = 270.0f;
    float inputY2 = 100.0f;
    DrawInputBox(font, inputX1, inputY1, inputBuffer1, inputActive1, inputLength1);
    DrawInputBox(font, inputX2, inputY2, inputBuffer2, inputActive2, inputLength2);

    // Xử lý kích hoạt ô nhập liệu
    if (CheckCollisionPointRec(GetMousePosition(), {inputX1, inputY1, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = true; inputActive2 = false;
    } else if (CheckCollisionPointRec(GetMousePosition(), {inputX2, inputY2, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = false;
    }
    if (inputActive1) HandleInput1();
    if (inputActive2) HandleInput2();

    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        avl->Initialize(value);
        avl->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Add", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        avl->Add(value);
        avl->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Delete", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        avl->Delete(value);
        avl->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Update", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        int oldValue = GetInputValue1();
        int newValue = GetInputValue2();
        avl->Update(oldValue, newValue);        // Cập nhật từ oldValue sang newValue
        avl->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
        inputLength2 = 0; inputBuffer2[0] = '\0';
    }
    if (DrawButton("Search", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        avl->Search(value);
        avl->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    avl->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, avl);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawGraphScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, Graph* graph) {
    ClearBackground(PINK);
    DrawTextEx(font, "Graph", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

    // Vẽ hai ô nhập liệu
    float inputX1 = 50.0f;
    float inputY1 = 100.0f;
    float inputX2 = 270.0f;
    float inputY2 = 100.0f;
    DrawInputBox(font, inputX1, inputY1, inputBuffer1, inputActive1, inputLength1);
    DrawInputBox(font, inputX2, inputY2, inputBuffer2, inputActive2, inputLength2);

    // Xử lý kích hoạt ô nhập liệu
    if (CheckCollisionPointRec(GetMousePosition(), {inputX1, inputY1, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = true; inputActive2 = false;
    } else if (CheckCollisionPointRec(GetMousePosition(), {inputX2, inputY2, 200, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive1 = false; inputActive2 = false;
    }
    if (inputActive1) HandleInput1();
    if (inputActive2) HandleInput2();

    float buttonX = 50.0f;
    float buttonY = 150.0f;
    if (DrawButton("Initialize", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        graph->Initialize(0);                   // Không cần giá trị nhập cho Initialize
        graph->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Add Edge", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();           // Giả sử ô 1 là "to" vertex, "from" mặc định là 0
        graph->AddEdge(0, value, 4);
        graph->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Delete", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        graph->Delete(value);
        graph->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Update", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        int oldValue = GetInputValue1();
        int newValue = GetInputValue2();
        graph->Update(oldValue, newValue);      // Cập nhật từ oldValue sang newValue
        graph->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
        inputLength2 = 0; inputBuffer2[0] = '\0';
    }
    if (DrawButton("Search", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        graph->Search(value);
        graph->ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    graph->DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, graph);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}

void DrawBackButton(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    float buttonX = 50.0f;
    float buttonY = 600.0f;
    if (DrawButton("Back", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        if (currentScreen == Screen::DATA_STRUCTURES || currentScreen == Screen::SETTINGS) {
            currentScreen = Screen::MAIN_MENU;
        } else {
            currentScreen = Screen::DATA_STRUCTURES;
        }
    }
}

void DrawAnimationControls(Font font, bool& buttonClicked, const char*& buttonMessage, DataStructureLogic* ds) {
    float buttonX = 800.0f;
    float buttonY = 500.0f;
    if (DrawButton("Play", buttonX, buttonY, font, buttonClicked, buttonMessage)) {
        ds->StartAnimation();
    }
    if (DrawButton("Pause", buttonX + 220.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->PauseAnimation();
    }
    if (DrawButton("Next", buttonX + 440.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->NextStep();
    }
    if (DrawButton("Prev", buttonX + 660.0f, buttonY, font, buttonClicked, buttonMessage)) {
        ds->PrevStep();
    }

    float stepY = 600.0f;
    for (int i = 0; i < ds->GetTotalSteps(); i++) {
        DrawTextEx(font, ds->GetSteps()[i].c_str(), { 800.0f, stepY + i * 30.0f }, 20, 1, (i == ds->GetCurrentStep()) ? RED : DARKGRAY);
    }
}