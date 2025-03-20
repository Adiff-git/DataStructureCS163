#include "SLLScreen.h"
#include "GUI.h"

namespace SLLScreen {
    void DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, SinglyLinkedList* sll) {
        static char inputBuffer1[32] = "";
        static char inputBuffer2[32] = "";
        static int inputLength1 = 0;
        static int inputLength2 = 0;
        static bool inputActive1 = false;
        static bool inputActive2 = false;
        static float sliderValue = 1.0f; // Giá trị mặc định của thanh kéo (tốc độ)

        auto HandleInput1 = [&]() {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48 && key <= 57) && inputLength1 < 31) {
                    inputBuffer1[inputLength1] = (char)key;
                    inputLength1++;
                    inputBuffer1[inputLength1] = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && inputLength1 > 0) {
                inputLength1--;
                inputBuffer1[inputLength1] = '\0';
            }
        };

        auto HandleInput2 = [&]() {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48 && key <= 57) && inputLength2 < 31) {
                    inputBuffer2[inputLength2] = (char)key;
                    inputLength2++;
                    inputBuffer2[inputLength2] = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && inputLength2 > 0) {
                inputLength2--;
                inputBuffer2[inputLength2] = '\0';
            }
        };

        auto GetInputValue1 = [&]() { return inputLength1 == 0 ? 0 : atoi(inputBuffer1); };
        auto GetInputValue2 = [&]() { return inputLength2 == 0 ? 0 : atoi(inputBuffer2); };

        auto DrawInputBox = [&](float x, float y, char* buffer, bool active) {
            DrawRectangle(x, y, 200, 50, WHITE);
            DrawRectangleLines(x, y, 200, 50, BLACK);
            DrawTextEx(font, buffer, {x + 10, y + 10}, 30, 1, BLACK);
            if (active && (GetTime() - (int)GetTime()) < 0.5) {
                DrawTextEx(font, "|", {x + 10 + MeasureTextEx(font, buffer, 30, 1).x, y + 10}, 30, 1, BLACK);
            }
        };

        ClearBackground(ORANGE);
        DrawTextEx(font, "Singly Linked List", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

        float inputX1 = 50.0f;
        float inputY1 = 100.0f;
        float inputX2 = 270.0f;
        float inputY2 = 100.0f;
        DrawInputBox(inputX1, inputY1, inputBuffer1, inputActive1);
        DrawInputBox(inputX2, inputY2, inputBuffer2, inputActive2);

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
            sll->Initialize(value);
            inputLength1 = 0; inputBuffer1[0] = '\0';
        }
        if (DrawButton("Add", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
            int value = GetInputValue1();
            sll->Add(value);
            sll->StartAnimation();
            inputLength1 = 0; inputBuffer1[0] = '\0';
        }
        if (DrawButton("Delete", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
            int value = GetInputValue1();
            sll->Delete(value);
            sll->StartAnimation();
            inputLength1 = 0; inputBuffer1[0] = '\0';
        }
        if (DrawButton("Update", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
            int oldValue = GetInputValue1();
            int newValue = GetInputValue2();
            sll->Update(oldValue, newValue);
            sll->StartAnimation();
            inputLength1 = 0; inputBuffer1[0] = '\0';
            inputLength2 = 0; inputBuffer2[0] = '\0';
        }
        if (DrawButton("Search", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
            int value = GetInputValue1();
            sll->Search(value);
            sll->StartAnimation();
            inputLength1 = 0; inputBuffer1[0] = '\0';
        }

        // Nút điều khiển ở giữa phía dưới
        float centerX = (GetScreenWidth() - 3 * 220) / 2;
        float controlY = GetScreenHeight() - 100.0f;
        if (DrawButton("Prev", centerX, controlY, font, buttonClicked, buttonMessage)) {
            sll->PrevStep();
        }
        if (DrawButton("Play", centerX + 220, controlY, font, buttonClicked, buttonMessage)) {
            if (sll->IsRunning()) sll->PauseAnimation();
            else sll->StartAnimation();
        }
        if (DrawButton("Skip", centerX + 440, controlY, font, buttonClicked, buttonMessage)) {
            sll->NextStep();
        }

        // Thanh kéo để chỉnh tốc độ
        float sliderX = centerX + 660;
        float sliderY = controlY;
        DrawTextEx(font, "Speed:", { sliderX - 100, sliderY }, 20, 1, DARKGRAY);
        Rectangle sliderRect = { sliderX, sliderY, 200, 20 };
        DrawRectangleRec(sliderRect, LIGHTGRAY);
        DrawRectangleLinesEx(sliderRect, 2, BLACK);

        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, sliderRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            sliderValue = (mousePos.x - sliderX) / 200.0f; // Tính giá trị từ 0.0 đến 1.0
            sliderValue = std::max(0.0f, std::min(1.0f, sliderValue)); // Giới hạn giá trị
        }

        float speed = 0.5f + (1.5f - 0.5f) * sliderValue; // Tốc độ từ 0.5 đến 1.5
        sll->SetAnimationSpeed(speed); // Cập nhật tốc độ cho SinglyLinkedList
        Rectangle sliderKnob = { sliderX + sliderValue * 200 - 5, sliderY - 5, 10, 30 };
        DrawRectangleRec(sliderKnob, DARKGRAY);
        DrawTextEx(font, TextFormat("%.1f", speed), { sliderX + 220, sliderY }, 20, 1, DARKGRAY);

        // Cập nhật và vẽ node trong mỗi khung hình
        sll->UpdateAnimation();
        sll->DrawAnimation(font, 300, 200);
        // Nếu không có node trong currentNodes, vẽ trực tiếp từ head
        sll->DrawDirect(font, 300, 200);
        DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
    }
}