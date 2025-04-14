#include "GraphMain.h"
#include "raylib.h" // Cần thiết cho các hàm Draw...
#include <vector>
#include <string>
#include <iomanip>  // For std::fixed, std::setprecision
#include <sstream>  // For std::stringstream
#include <algorithm> // For std::max, std::min

// --- Định nghĩa Hàm Vẽ Nút Tiện Ích ---
void DrawButtonUI(const char* text, Rectangle button, Font font, Color tint, Color textColor) {
    Color finalTint = tint;
    // Ví dụ thêm hiệu ứng hover đơn giản
    if (CheckCollisionPointRec(GetMousePosition(), button)) {
        //  // Làm màu sáng hơn một chút khi hover, nhưng không phải màu trắng hoàn toàn nếu tint không phải trắng
        //  if (tint.r != 255 || tint.g != 255 || tint.b != 255) {
        //      finalTint.r = (unsigned char)std::min(255, tint.r + 30);
        //      finalTint.g = (unsigned char)std::min(255, tint.g + 30);
        //      finalTint.b = (unsigned char)std::min(255, tint.b + 30);
        //  } else {
        //      finalTint = LIGHTGRAY; // Nếu tint là trắng, hover thành xám nhạt
        //  }
        DrawRectangleRec(button, DARKGRAY);
    }
    else
    {
        DrawRectangleRec(button, LIGHTGRAY);
    }
    DrawRectangleLinesEx(button, 1, BLACK); // Viền đen
    Vector2 textSize = MeasureTextEx(font, text, 20, 1);
    DrawTextEx(font, text, { button.x + (button.width - textSize.x) / 2, button.y + (button.height - textSize.y) / 2 }, 20, 1, BLACK);
}


// --- Định nghĩa các hàm vẽ UI khác ---

void DrawMainControlsUI(const UIState& state) {
    DrawButtonUI("Create", state.createButton, GetFontDefault(), state.isCreating ? ORANGE : WHITE);
    DrawButtonUI("Random", state.randomButton, GetFontDefault(), WHITE); // Cần thêm cờ isRandomizingActive nếu muốn highlight
    DrawButtonUI("Examples", state.exampleButton, GetFontDefault(), state.isShowingExamplesActive ? ORANGE : WHITE);
    DrawButtonUI("File", state.fileButton, GetFontDefault(), state.isFileActive ? ORANGE : WHITE);
    DrawButtonUI("Edit", state.editButton, GetFontDefault(), state.isEditingGraph ? ORANGE : WHITE);
    DrawButtonUI("MST", state.mstButton, GetFontDefault(), state.showMSTMenu ? ORANGE : WHITE);
}

void DrawCreateModeUI(const UIState& state) {
    // Draw label and input for nodes (N)
    DrawText("N:", state.nodesInputRect.x - 25, state.nodesInputRect.y + 5, 20, WHITE);
    DrawRectangleRec(state.nodesInputRect, LIGHTGRAY);
    DrawButtonUI("", state.nodesInputRect, GetFontDefault(), LIGHTGRAY); // Nút giả để có viền
    DrawText(state.numNodesStr.c_str(), state.nodesInputRect.x + 5, state.nodesInputRect.y + 5, 20, BLACK);
    if (state.nodesFocused && ((int)(state.cursorTimer * 2.5f) % 2 == 0)) // Tăng tốc độ nháy
        DrawText("|", state.nodesInputRect.x + 5 + MeasureText(state.numNodesStr.c_str(), 20), state.nodesInputRect.y + 5, 20, BLACK);
    
    // Draw question mark button for nodes
    Rectangle nodesQuestionButton = { state.nodesInputRect.x + state.nodesInputRect.width + 5, state.nodesInputRect.y, 30, 30 };
    DrawRectangleRounded(nodesQuestionButton, 0.3f, 8, CheckCollisionPointRec(GetMousePosition(), nodesQuestionButton) ? SKYBLUE : LIGHTGRAY);
    DrawRectangleRoundedLines(nodesQuestionButton, 0.3f, 8, DARKGRAY);
    DrawText("?", nodesQuestionButton.x + 8, nodesQuestionButton.y + 2, 24, BLACK);

    // Draw label and input for edges (E)
    DrawText("E:", state.edgesInputRect.x - 25, state.edgesInputRect.y + 5, 20, WHITE);
    DrawRectangleRec(state.edgesInputRect, LIGHTGRAY);
    DrawButtonUI("", state.edgesInputRect, GetFontDefault(), LIGHTGRAY);
    DrawText(state.numEdgesStr.c_str(), state.edgesInputRect.x + 5, state.edgesInputRect.y + 5, 20, BLACK);
    if (state.edgesFocused && ((int)(state.cursorTimer * 2.5f) % 2 == 0))
        DrawText("|", state.edgesInputRect.x + 5 + MeasureText(state.numEdgesStr.c_str(), 20), state.edgesInputRect.y + 5, 20, BLACK);
    
    // Draw question mark button for edges
    Rectangle edgesQuestionButton = { state.edgesInputRect.x + state.edgesInputRect.width + 5, state.edgesInputRect.y, 30, 30 };
    DrawRectangleRounded(edgesQuestionButton, 0.3f, 8, CheckCollisionPointRec(GetMousePosition(), edgesQuestionButton) ? SKYBLUE : LIGHTGRAY);
    DrawRectangleRoundedLines(edgesQuestionButton, 0.3f, 8, DARKGRAY);
    DrawText("?", edgesQuestionButton.x + 8, edgesQuestionButton.y + 2, 24, BLACK);

    // Draw OK button next to the edges question mark button
    DrawButtonUI("OK", state.okButton, GetFontDefault(), CheckCollisionPointRec(GetMousePosition(), state.okButton) ? SKYBLUE : LIGHTGRAY);
}

void DrawExampleButtonsUI(const UIState& state) {
    DrawButtonUI("K5", state.k5Button, GetFontDefault(), WHITE);
    DrawButtonUI("C6", state.c6Button, GetFontDefault(), WHITE);
    DrawButtonUI("P4", state.p4Button, GetFontDefault(), WHITE);
    DrawButtonUI("S7", state.s7Button, GetFontDefault(), WHITE);
}

void DrawEditPanelUI(const UIState& state) {
    DrawRectangle(state.editPanelX, 0, state.editPanelWidth, state.screenHeight, Fade(BLACK, 0.7f)); // Panel background

    // Vẽ các nút công cụ với highlight
    DrawButtonUI("Add Vertex", state.addVertexButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_ADD_VERTEX) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Add Edge", state.addEdgeButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_ADD_EDGE_START || state.currentTool == EditTool::TOOL_ADD_EDGE_END) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Edit Weight", state.editWeightButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_EDIT_WEIGHT) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Move Node", state.moveNodeButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_MOVE_VERTEX) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Delete Vertex", state.deleteVertexButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_DELETE_VERTEX) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Delete Edge", state.deleteEdgeButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_DELETE_EDGE) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Done", state.doneButtonRect, GetFontDefault(), WHITE);

    // Vẽ ô nhập Weight (nếu đang sửa)
    if (state.isEditingWeight) {
        DrawRectangleRec(state.weightInputRect, WHITE);
        DrawRectangleLinesEx(state.weightInputRect, 1, BLACK);
        DrawText(state.weightInputBuffer.c_str(), state.weightInputRect.x + 5, state.weightInputRect.y + 4, 20, BLACK);
        if (((int)(state.cursorTimer * 2.5f)) % 2 == 0) {
            DrawText("|", state.weightInputRect.x + 5 + MeasureText(state.weightInputBuffer.c_str(), 20), state.weightInputRect.y + 4, 20, BLACK);
        }
    }

    // Vẽ chữ gợi ý
     const char* hintText = "";
     switch(state.currentTool){
         case EditTool::TOOL_ADD_VERTEX: hintText = "Click empty space to add vertex."; break;
         case EditTool::TOOL_ADD_EDGE_START: hintText = "Click first vertex for edge."; break;
         case EditTool::TOOL_ADD_EDGE_END: hintText = "Click second vertex for edge."; break;
         case EditTool::TOOL_EDIT_WEIGHT: hintText = "Click near edge to edit weight."; break;
         case EditTool::TOOL_MOVE_VERTEX: hintText = "Click and drag a vertex."; break;
         case EditTool::TOOL_DELETE_VERTEX: hintText = "Click a vertex to delete."; break;
         case EditTool::TOOL_DELETE_EDGE: hintText = "Click near an edge to delete."; break;
         default: hintText = "Select a tool from the right panel."; break;
     }
     DrawText(hintText, 10, state.screenHeight - 60, 20, RAYWHITE); // Vị trí gần nút chính
}

void DrawErrorMessagesUI(const UIState& state) {
    // Vị trí gần các nút chính
    float errorY = state.createButton.y - 30;
    if (state.showError) {
        DrawText(state.errorMessage.c_str(), state.createButton.x, errorY, 20, RED);
        errorY -= 25; // Dịch lên cho lỗi tiếp theo
    }
    if (state.showFileError) {
        DrawText(state.fileErrorMessage.c_str(), state.createButton.x, errorY, 20, RED);
    }
    // Lỗi MST đã vẽ trong DrawMSTMenuUI
}
void DrawButton(const char* text, Rectangle button, Font font, const char* buttonMessage)
{
    DrawRectangleLines(button.x, button.y, button.width, button.height, BLACK);
    Vector2 textSize = MeasureTextEx(font, text, 20, 1); 
    DrawTextEx(font, text, { button.x + button.width / 2 - textSize.x / 2, button.y + button.height / 2 - textSize.y / 2 }, 20, 1, BLACK); 
}
