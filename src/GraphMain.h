#ifndef GRAPHMAIN_H
#define GRAPHMAIN_H

#include "raylib.h"
#include <vector>
#include <string>
#include "GraphApp.h" // Giả sử file này chứa định nghĩa Edge, Slider, EditTool

struct UIState {
    // --- Flags ---
    bool isCreating;
    bool isShowingExamples;
    bool isShowingExamplesActive; // Để highlight nút Examples
    bool isFileActive;            // Để highlight nút File
    bool isEditingGraph;
    bool showMSTMenu;
    bool usePrim;
    bool useKruskal;
    bool mstDoneDrawing;
    bool showWeightInfo;
    bool isMSTDrawingPaused;
    bool showError;
    bool showMSTError;
    bool showFileError;
    bool nodesFocused;
    bool edgesFocused;
    bool isEditingWeight;
    bool isDraggingVScrollbar; // Thêm trạng thái kéo thanh cuộn dọc
    bool isDraggingHScrollbar; // Thêm trạng thái kéo thanh cuộn ngang
    bool showPseudocodeVScrollbar;
    bool showPseudocodeHScrollbar;


    // --- Data ---
    std::string numNodesStr;
    std::string numEdgesStr;
    std::string weightInputBuffer;
    std::string errorMessage;
    std::string mstErrorMessage;
    std::string fileErrorMessage;
    int totalMSTWeight;
    int currentHighlightLine;
    EditTool currentTool;
    int selectedNodeIndex;  // Cần để highlight nút/đường kẻ khi edit
    int selectedEdgeIndex;  // Cần để highlight cạnh khi edit weight/delete edge

    // --- Scroll ---
    float mstPseudocodeScrollY;
    float mstPseudocodeScrollX;
    float mstPseudocodeTotalHeight; // Cần để tính toán scrollbar
    float mstPseudocodeMaxWidth;    // Cần để tính toán scrollbar

    // --- UI Elements / Data ---
    Slider* speedSlider; // Con trỏ tới slider tốc độ
    const std::vector<std::string>* currentCode; // Con trỏ tới mã giả đang hiển thị

    // --- Rectangles (Quan trọng cho layout và kiểm tra va chạm UI) ---
    Rectangle createButton, randomButton, exampleButton, fileButton, editButton, mstButton;
    Rectangle nodesInputRect, edgesInputRect;
    Rectangle k5Button, c6Button, p4Button, s7Button;
    Rectangle mstMenuRect, primButton, kruskalButton, backButton;
    Rectangle prevStepButton, nextStepButton, skipButton, pauseResumeButton; // Các nút điều khiển MST
    Rectangle addVertexButtonRect, addEdgeButtonRect, editWeightButtonRect, moveNodeButtonRect;
    Rectangle deleteVertexButtonRect, deleteEdgeButtonRect, doneButtonRect, weightInputRect; // Edit panel
    Rectangle pseudocodeViewRect;         // Vùng xem mã giả (sẽ được tính toán trong hàm vẽ)
    Rectangle pseudocodeVScrollbarRect;   // Thanh cuộn dọc
    Rectangle pseudocodeVScrollHandleRect;// Tay cầm cuộn dọc
    Rectangle pseudocodeHScrollbarRect;   // Thanh cuộn ngang
    Rectangle pseudocodeHScrollHandleRect;// Tay cầm cuộn ngang

    // --- Khác ---
    Vector2 mousePos;       // Vị trí chuột hiện tại (cho hover effects)
    float cursorTimer;      // Cho con trỏ nhấp nháy
    int screenWidth;        // Kích thước màn hình (nếu cần cho layout)
    int screenHeight;
    float editPanelX;       // Vị trí panel edit
    float editPanelWidth;

    // (Có thể thêm constructor hoặc hàm init nếu muốn)
};


// --- Khai báo các hàm vẽ UI ---
void DrawButtonUI(const char* text, Rectangle button, Font font, Color tint = WHITE, Color textColor = BLACK);
void DrawButton(const char* text, Rectangle button, Font font, const char* buttonMessage);
void DrawMainControlsUI(const UIState& state);
void DrawCreateModeUI(const UIState& state);
void DrawExampleButtonsUI(const UIState& state);
void DrawEditPanelUI(const UIState& state);
void DrawErrorMessagesUI(const UIState& state);

#endif // GRAPHMAIN_H