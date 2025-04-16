#ifndef GRAPHMAIN_H
#define GRAPHMAIN_H

#include "raylib.h"
#include <vector>
#include <string>
#include "Graph.h"
#include "Button.h"

struct UIState {
    // --- Flags ---
    bool isCreating;
    bool isShowingExamples;
    bool isShowingExamplesActive;
    bool isFileActive;
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
    bool isDraggingVScrollbar;
    bool isDraggingHScrollbar;
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
    int selectedNodeIndex;
    int selectedEdgeIndex;

    // --- Scroll ---
    float mstPseudocodeScrollY;
    float mstPseudocodeScrollX;
    float mstPseudocodeTotalHeight;
    float mstPseudocodeMaxWidth;

    // --- UI Elements / Data ---
    Slider* speedSlider;
    const std::vector<std::string>* currentCode;

    // --- Rectangles (Quan trọng cho layout và kiểm tra va chạm UI) ---
    Rectangle createButton, randomButton, exampleButton, fileButton, editButton, mstButton;
    Rectangle nodesInputRect, edgesInputRect;
    Rectangle k5Button, c6Button, p4Button, s7Button;
    Rectangle mstMenuRect, primButton, kruskalButton, cancelButton;
    Rectangle prevStepButton, nextStepButton, skipButton, pauseResumeButton;
    Rectangle addVertexButtonRect, addEdgeButtonRect, editWeightButtonRect, moveNodeButtonRect;
    Rectangle deleteVertexButtonRect, deleteEdgeButtonRect, doneButtonRect, weightInputRect;
    Rectangle pseudocodeViewRect;
    Rectangle pseudocodeVScrollbarRect;
    Rectangle pseudocodeVScrollHandleRect;
    Rectangle pseudocodeHScrollbarRect;
    Rectangle pseudocodeHScrollHandleRect;
    Rectangle nodesQuestionButton;
    Rectangle edgesQuestionButton;
    Rectangle okButton;

    // --- Khác ---
    Vector2 mousePos;
    float cursorTimer;
    int screenWidth;
    int screenHeight;
    float editPanelX;
    float editPanelWidth;
};

class GraphMain {
public:
    GraphMain();
    ~GraphMain();
    void Run();
    bool ShouldClose();
    
private:
    bool shouldClose;
    bool backToMainMenu;
    Texture2D backButtonTexture;
    bool backButtonClicked;
    
    const float screenWidth = 1600;
    const float screenHeight = 900;

    // State Variables
    bool isCreating = false; bool isRandomizing = false; bool isShowingExamples = false;
    bool isCreatingActive = false; bool isRandomizingActive = false; bool isShowingExamplesActive = false; 
    bool isFileActive = false; bool graphDrawn = false;
    std::vector<Edge> edges; std::vector<Vector2> nodePositions; Camera2D graphCamera = { 0 };
    std::string numNodesStr = ""; std::string numEdgesStr = ""; std::string matrixInput = "";
    std::string weightInputBuffer = ""; bool nodesFocused = false; bool edgesFocused = false;
    bool matrixInputFocused = false; bool isEditingWeight = false; bool showMatrixInput = false;
    bool showExampleButtons = false; bool showMSTMenu = false; bool isEditingGraph = false;
    EditTool currentTool = EditTool::TOOL_NONE; int selectedNodeIndex = -1; int selectedEdgeIndex = -1;
    bool isDraggingNode = false; bool usePrim = false; bool useKruskal = false;
    std::vector<Edge> mstEdges; std::vector<bool> mstEdgesDrawn; int mstEdgeIndex = 0;
    float mstDrawTimer = 0.0f; bool mstDoneDrawing = false; int totalMSTWeight = 0;
    bool showWeightInfo = false; bool isMSTDrawingPaused = true; // Bắt đầu pause
    std::vector<bool> mstNodesDrawn; std::string errorMessage = ""; std::string errorMessageInput = "";
    std::string mstErrorMessage = ""; std::string fileErrorMessage = ""; std::string outErrorMessage = "";
    bool showError = false; bool showMSTError = false; bool showFileError = false;
    int cursorColumn = 0; int cursorRow = 0; float cursorTimer = 0.0f;

    // Pseudo-code vectors
    std::vector<std::string> primPseudocode = {
        "Prim's Algorithm:",
        "1. Initialize MST = {}, Visited = {node 1}, PQ = {}",
        "2. Add all edges from node 1 to PQ",
        "3. While PQ not empty and MST size < N-1:",
        "4.   Extract edge(u, v) with min weight from PQ",
        "5.   If v already in Visited: skip edge",
        "6.   Add edge(u, v) to MST",
        "7.   Add v to Visited",
        "8.   Add all edges from v to unvisited nodes to PQ",
        "9. Return MST"
    };
    std::vector<std::string> kruskalPseudocode = {
        "Kruskal's Algorithm:",
        "1. Initialize MST = {}",
        "2. Initialize DSU for N nodes",
        "3. Sort all edges by weight",
        "4. For each edge(u, v) in sorted order:",
        "5.   If findSet(u) != findSet(v):",
        "6.     Add edge(u, v) to MST",
        "7.     Union sets of u and v",
        "8.     If MST size == N-1: break",
        "9. Return MST"
    };
    int currentHighlightLine = -1; // Index của dòng mã giả cần highlight (-1 là không có)

    // MST Scroll variables
    float mstPseudocodeScrollY = 0.0f;         // Vertical scroll offset
    float mstPseudocodeScrollX = 0.0f;         // Horizontal scroll offset
    float mstPseudocodeTotalHeight = 0.0f;     // Calculated total content height
    float mstPseudocodeMaxWidth = 0.0f;        // Calculated max content width
    bool isDraggingVScrollbar = false;        // Dragging Vertical Scrollbar
    bool isDraggingHScrollbar = false;        // Dragging Horizontal Scrollbar
    float scrollbarMouseOffsetY = 0.0f;      // Mouse offset for V-Scroll
    float scrollbarMouseOffsetX = 0.0f;      // Mouse offset for H-Scroll
    Rectangle pseudocodeVScrollbarRect = { 0 }; // Vertical scrollbar track
    Rectangle pseudocodeVScrollHandleRect = { 0 };// Vertical scrollbar handle
    Rectangle pseudocodeHScrollbarRect = { 0 }; // Horizontal scrollbar track
    Rectangle pseudocodeHScrollHandleRect = { 0 };// Horizontal scrollbar handle
    bool showPseudocodeVScrollbar = false;     // Flag to show V-Scrollbar
    bool showPseudocodeHScrollbar = false;     // Flag to show H-Scrollbar
    Rectangle pseudocodeViewRect = { 0 };
    // -------------------------------------------------
    
     // Left Panel Buttons
     const float buttonWidth = 80.0f;
     const float buttonHeight = 30.0f;
     const float bottomPadding = 10.0f; // Khoảng cách từ cạnh dưới màn hình
     const float leftPadding = 30.0f;   // Khoảng cách từ cạnh trái màn hình
     const float buttonSpacing = 10.0f; // Khoảng cách giữa các nút
     const float buttonY = 700.0f;
     // Định nghĩa vị trí các nút theo hàng ngang từ trái sang phải
     // 30 , 700, 80, 30
     Rectangle createButton = {leftPadding, buttonY, buttonWidth, buttonHeight};
     Rectangle randomButton = {createButton.x + createButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle exampleButton = {randomButton.x + randomButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle fileButton = {exampleButton.x + exampleButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle editButton = {fileButton.x + fileButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle mstButton = {editButton.x + editButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     // Create Mode Inputs
     Rectangle nodesInputRect = { 30, 650, 100, 30 };
     Rectangle edgesInputRect = { nodesInputRect.x + nodesInputRect.width + 80, 650, 100, 30 };
     Rectangle nodesQuestionButton = { nodesInputRect.x + nodesInputRect.width + 15, nodesInputRect.y, 20, 20 };
     Rectangle edgesQuestionButton = { edgesInputRect.x + edgesInputRect.width + 15, edgesInputRect.y, 20, 20 };
     Rectangle okButton = { edgesQuestionButton.x + edgesQuestionButton.width + 15, edgesInputRect.y, 50, 30 }; // Position OK 
     // Example Buttons (positions relative to exampleButton)
     Rectangle k5Button = { 30, 650, 60, 30 };
     Rectangle c6Button = { k5Button.x + k5Button.width + 10, 650, 60, 30 };
     Rectangle p4Button = { c6Button.x + c6Button.width + 10, 650, 60, 30 };
     Rectangle s7Button = { p4Button.x + p4Button.width + 10, 650, 60, 30 };
 
     // MST Menu Elements
     float mstMenuWidth = screenWidth * 0.85f;
     float mstMenuHeight = screenHeight * 0.9f;
     float mstMenuX = (screenWidth - mstMenuWidth) / 2.0f;
     float mstMenuY = (screenHeight - mstMenuHeight) / 2.0f;
     Rectangle mstMenuRect = { mstMenuX, mstMenuY, mstMenuWidth, mstMenuHeight }; // Định nghĩa mới!
     
     const int mstButtonWidth = 80;
     const int mstButtonHeight = 30;
     const int mstButtonSpacing = 10;
     const float stepButtonWidth = 60.0f;
     const float mstButtonsY = mstMenuRect.y + mstMenuRect.height - mstButtonHeight - 10;
     Rectangle primButton = {mstMenuRect.x + 10, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
     Rectangle kruskalButton = {primButton.x + mstButtonWidth + mstButtonSpacing, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
     Rectangle cancelButton = {kruskalButton.x + mstButtonWidth + mstButtonSpacing, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
     Rectangle prevStepButton = { 0 };
     Rectangle nextStepButton = { 0 };
     Rectangle skipButton = { 0 };
     Rectangle pauseResumeButton = { 0 }; // Sẽ tính toán vị trí khi vẽ
     Rectangle weightBox = { 0 };
     Slider speedSlider = Slider({0, 0, 150, 20}, 0.1f, 2.0f, "Speed:");
     // Slider speedSlider = Slider({mstMenuRect.x + mstMenuRect.width - 160, mstMenuRect.y + 45, 150, 20}, 0.1f, 2.0f, "Speed:"); // Position near top right
 
     // Edit Mode Elements (Right Panel)
     const float editPanelWidth = 130.0f;
     const float editPanelX = screenWidth - editPanelWidth;
     Rectangle addVertexButtonRect = { editPanelX + 10, 10, editPanelWidth - 20, 30 };
     Rectangle addEdgeButtonRect = { editPanelX + 10, 50, editPanelWidth - 20, 30 };
     Rectangle editWeightButtonRect = { editPanelX + 10, 90, editPanelWidth - 20, 30 };
     Rectangle moveNodeButtonRect = { editPanelX + 10, 130, editPanelWidth - 20, 30 };
     Rectangle deleteVertexButtonRect = { editPanelX + 10, 170, editPanelWidth - 20, 30 };
     Rectangle deleteEdgeButtonRect = { editPanelX + 10, 210, editPanelWidth - 20, 30 };
     Rectangle doneButtonRect = { editPanelX + 10, screenHeight - 50, editPanelWidth - 20, 30 };
     Rectangle weightInputRect = {0, 0, 60, 25}; // Position calculated dynamically

     // UI dimensions
     const float uiLeftWidth = 120.0f; // Left panel with buttons
     const float uiRightWidth = 130.0f; // Right edit panel
     Vector2 mousePos = GetMousePosition();
     Vector2 worldMousePos = GetScreenToWorld2D(mousePos, graphCamera); // Mouse in graph space
     bool clickedOnUI = false;
     float wheel = GetMouseWheelMove();
 

     void InitializeUI();
     void UpdateGraph();
     void DrawGraph();
     void DrawUI();
};

// Khai báo hàm DrawButton độc lập (dùng cho các nút trong MST menu)
void DrawButton(const char* text, Rectangle button, Font font, const char* buttonMessage);
void DrawButtonUI(const char* text, Rectangle button, Font font, Color tint = WHITE, Color textColor = BLACK);
void DrawMainControlsUI(const UIState& state);
void DrawCreateModeUI(const UIState& state);
void DrawExampleButtonsUI(const UIState& state);
void DrawEditPanelUI(const UIState& state);
void DrawErrorMessagesUI(const UIState& state);
#endif