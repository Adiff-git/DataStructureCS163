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
    Rectangle mstMenuRect, primButton, kruskalButton, backButton;
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

    
};

// Khai báo hàm DrawButton độc lập (dùng cho các nút trong MST menu)
void DrawButton(const char* text, Rectangle button, Font font, const char* buttonMessage);
void DrawButtonUI(const char* text, Rectangle button, Font font, Color tint = WHITE, Color textColor = BLACK);
    void DrawMainControlsUI(const UIState& state);
    void DrawCreateModeUI(const UIState& state);
    void DrawExampleButtonsUI(const UIState& state);
    void DrawEditPanelUI(const UIState& state);
    void DrawErrorMessagesUI(const UIState& state);
    bool RunGraphApp(Texture2D backButtonTexture, bool& backToMainMenu);
#endif