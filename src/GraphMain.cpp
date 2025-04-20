#include "GraphMain.h"
#include "Button.h"
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>

// Định nghĩa hàm DrawButton (không nằm trong class GraphMain, phù hợp với khai báo trong GraphMain.h)
void DrawButton(const char* text, Rectangle button, Font font, const char* buttonMessage) {
    DrawRectangleLines(button.x, button.y, button.width, button.height, BLACK);
    Vector2 textSize = MeasureTextEx(font, text, 20, 1);
    DrawTextEx(font, text, { button.x + button.width / 2 - textSize.x / 2, button.y + button.height / 2 - textSize.y / 2 }, 20, 1, BLACK);
}

void DrawButtonUI(const char* text, Rectangle button, Font font, Color tint, Color textColor) {
    Color finalTint = tint;
    if (CheckCollisionPointRec(GetMousePosition(), button)) {
        DrawRectangleRec(button, DARKGRAY);
    } else {
        DrawRectangleRec(button, LIGHTGRAY);
    }
    DrawRectangleLinesEx(button, 1, BLACK);
    Vector2 textSize = MeasureTextEx(font, text, 20, 1);
    DrawTextEx(font, text, { button.x + (button.width - textSize.x) / 2, button.y + (button.height - textSize.y) / 2 }, 20, 1, BLACK);
}

void DrawMainControlsUI(const UIState& state) {
    DrawButtonUI("Create", state.createButton, GetFontDefault(), state.isCreating ? ORANGE : WHITE);
    DrawButtonUI("Random", state.randomButton, GetFontDefault(), WHITE);
    DrawButtonUI("Examples", state.exampleButton, GetFontDefault(), state.isShowingExamplesActive ? ORANGE : WHITE);
    DrawButtonUI("File", state.fileButton, GetFontDefault(), state.isFileActive ? ORANGE : WHITE);
    DrawButtonUI("Edit", state.editButton, GetFontDefault(), state.isEditingGraph ? ORANGE : WHITE);
    DrawButtonUI("MST", state.mstButton, GetFontDefault(), state.showMSTMenu ? ORANGE : WHITE);
}

void DrawCreateModeUI(const UIState& state) {
    DrawText("N:", state.nodesInputRect.x - 25, state.nodesInputRect.y + 5, 20, BLACK);
    DrawRectangleRec(state.nodesInputRect, LIGHTGRAY);
    DrawButtonUI("", state.nodesInputRect, GetFontDefault(), LIGHTGRAY);
    DrawText(state.numNodesStr.c_str(), state.nodesInputRect.x + 5, state.nodesInputRect.y + 5, 20, BLACK);
    if (state.nodesFocused && ((int)(state.cursorTimer * 2.5f) % 2 == 0))
        DrawText("|", state.nodesInputRect.x + 5 + MeasureText(state.numNodesStr.c_str(), 20), state.nodesInputRect.y + 5, 20, BLACK);

    Rectangle nodesQuestionButton = { state.nodesInputRect.x + state.nodesInputRect.width + 5, state.nodesInputRect.y, 30, 30 };
    DrawRectangleRounded(nodesQuestionButton, 0.3f, 8, CheckCollisionPointRec(GetMousePosition(), nodesQuestionButton) ? SKYBLUE : LIGHTGRAY);
    DrawRectangleRoundedLines(nodesQuestionButton, 0.3f, 8, DARKGRAY);
    DrawText("?", nodesQuestionButton.x + 8, nodesQuestionButton.y + 2, 24, BLACK);

    DrawText("E:", state.edgesInputRect.x - 25, state.edgesInputRect.y + 5, 20, BLACK);
    DrawRectangleRec(state.edgesInputRect, LIGHTGRAY);
    DrawButtonUI("", state.edgesInputRect, GetFontDefault(), LIGHTGRAY);
    DrawText(state.numEdgesStr.c_str(), state.edgesInputRect.x + 5, state.edgesInputRect.y + 5, 20, BLACK);
    if (state.edgesFocused && ((int)(state.cursorTimer * 2.5f) % 2 == 0))
        DrawText("|", state.edgesInputRect.x + 5 + MeasureText(state.numEdgesStr.c_str(), 20), state.edgesInputRect.y + 5, 20, BLACK);

    Rectangle edgesQuestionButton = { state.edgesInputRect.x + state.edgesInputRect.width + 5, state.edgesInputRect.y, 30, 30 };
    DrawRectangleRounded(edgesQuestionButton, 0.3f, 8, CheckCollisionPointRec(GetMousePosition(), edgesQuestionButton) ? SKYBLUE : LIGHTGRAY);
    DrawRectangleRoundedLines(edgesQuestionButton, 0.3f, 8, DARKGRAY);
    DrawText("?", edgesQuestionButton.x + 8, edgesQuestionButton.y + 2, 24, BLACK);

    DrawButtonUI("OK", state.okButton, GetFontDefault(), CheckCollisionPointRec(GetMousePosition(), state.okButton) ? SKYBLUE : LIGHTGRAY);
}

void DrawExampleButtonsUI(const UIState& state) {
    DrawButtonUI("K5", state.k5Button, GetFontDefault(), WHITE);
    DrawButtonUI("C6", state.c6Button, GetFontDefault(), WHITE);
    DrawButtonUI("P4", state.p4Button, GetFontDefault(), WHITE);
    DrawButtonUI("S7", state.s7Button, GetFontDefault(), WHITE);
}

void DrawEditPanelUI(const UIState& state) {
    DrawRectangle(state.editPanelX, 0, state.editPanelWidth, state.screenHeight, Fade(GRAY, 0.7f));

    DrawButtonUI("Add Vertex", state.addVertexButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_ADD_VERTEX) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Add Edge", state.addEdgeButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_ADD_EDGE_START || state.currentTool == EditTool::TOOL_ADD_EDGE_END) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Edit Weight", state.editWeightButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_EDIT_WEIGHT) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Move Node", state.moveNodeButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_MOVE_VERTEX) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Delete Vertex", state.deleteVertexButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_DELETE_VERTEX) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Delete Edge", state.deleteEdgeButtonRect, GetFontDefault(), (state.currentTool == EditTool::TOOL_DELETE_EDGE) ? ORANGE : LIGHTGRAY);
    DrawButtonUI("Done", state.doneButtonRect, GetFontDefault(), WHITE);

    if (state.isEditingWeight) {
        DrawRectangleRec(state.weightInputRect, WHITE);
        DrawRectangleLinesEx(state.weightInputRect, 1, BLACK);
        DrawText(state.weightInputBuffer.c_str(), state.weightInputRect.x + 5, state.weightInputRect.y + 4, 20, BLACK);
        if (((int)(state.cursorTimer * 2.5f)) % 2 == 0) {
            DrawText("|", state.weightInputRect.x + 5 + MeasureText(state.weightInputBuffer.c_str(), 20), state.weightInputRect.y + 4, 20, BLACK);
        }
    }

    const char* hintText = "";
    switch(state.currentTool) {
        case EditTool::TOOL_ADD_VERTEX: hintText = "Click empty space to add vertex."; break;
        case EditTool::TOOL_ADD_EDGE_START: hintText = "Click first vertex for edge."; break;
        case EditTool::TOOL_ADD_EDGE_END: hintText = "Click second vertex for edge."; break;
        case EditTool::TOOL_EDIT_WEIGHT: hintText = "Click near edge to edit weight."; break;
        case EditTool::TOOL_MOVE_VERTEX: hintText = "Click and drag a vertex."; break;
        case EditTool::TOOL_DELETE_VERTEX: hintText = "Click a vertex to delete."; break;
        case EditTool::TOOL_DELETE_EDGE: hintText = "Click near an edge to delete."; break;
        default: hintText = "Select a tool from the right panel."; break;
    }
    DrawText(hintText, state.createButton.x, state.createButton.y-30, 20, RED);
}

void DrawErrorMessagesUI(const UIState& state) {
    float errorY = state.createButton.y - 30;
    if (state.showError) {
        DrawText(state.errorMessage.c_str(), state.createButton.x, errorY, 20, RED);
        errorY -= 25;
    }
    if (state.showFileError) {
        DrawText(state.fileErrorMessage.c_str(), state.createButton.x, errorY, 20, RED);
    }
}

GraphMain::GraphMain()
    : shouldClose(false), backToMainMenu(false), backButtonClicked(false),
      isCreating(false), isRandomizing(false), isShowingExamples(false),
      isCreatingActive(false), isRandomizingActive(false), isShowingExamplesActive(false),
      isFileActive(false), graphDrawn(false), nodesFocused(false), edgesFocused(false),
      matrixInputFocused(false), isEditingWeight(false), showMatrixInput(false),
      showExampleButtons(false), showMSTMenu(false), isEditingGraph(false),
      currentTool(EditTool::TOOL_NONE), selectedNodeIndex(-1), selectedEdgeIndex(-1),
      isDraggingNode(false), usePrim(false), useKruskal(false), mstEdgeIndex(0),
      mstDrawTimer(0.0f), mstDoneDrawing(false), totalMSTWeight(0), showWeightInfo(false),
      isMSTDrawingPaused(true), showError(false), showMSTError(false), showFileError(false),
      cursorColumn(0), cursorRow(0), cursorTimer(0.0f), currentHighlightLine(-1),
      mstPseudocodeScrollY(0.0f), mstPseudocodeScrollX(0.0f), mstPseudocodeTotalHeight(0.0f),
      mstPseudocodeMaxWidth(0.0f), isDraggingVScrollbar(false), isDraggingHScrollbar(false),
      scrollbarMouseOffsetY(0.0f), scrollbarMouseOffsetX(0.0f), showPseudocodeVScrollbar(false),
      showPseudocodeHScrollbar(false), speedSlider({0, 0, 150, 20}, 0.1f, 2.0f, "Speed:") {

    backButtonTexture = LoadTexture("..\\resources\\images\\BackButton.png");
    // Initialize camera
    graphCamera.target = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    graphCamera.offset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    graphCamera.rotation = 0.0f;
    graphCamera.zoom = 1.0f;

    // Initialize pseudocode
    primPseudocode = {
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
    kruskalPseudocode = {
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

    InitializeUI();
}

GraphMain::~GraphMain() {
    UnloadTexture(backButtonTexture);
}

void GraphMain::InitializeUI() {
    // Left Panel Buttons
    const float buttonWidth = 80.0f;
    const float buttonHeight = 30.0f;
    const float leftPadding = 30.0f;
    const float buttonSpacing = 10.0f;
    const float buttonY = 700.0f;

    createButton = {leftPadding, buttonY, buttonWidth, buttonHeight};
    randomButton = {createButton.x + createButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
    exampleButton = {randomButton.x + randomButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
    fileButton = {exampleButton.x + exampleButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
    editButton = {fileButton.x + fileButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
    mstButton = {editButton.x + editButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};

    // Create Mode Inputs
    nodesInputRect = { 30, 650, 100, 30 };
    edgesInputRect = { nodesInputRect.x + nodesInputRect.width + 80, 650, 100, 30 };
    nodesQuestionButton = { nodesInputRect.x + nodesInputRect.width + 15, nodesInputRect.y, 20, 20 };
    edgesQuestionButton = { edgesInputRect.x + edgesInputRect.width + 15, edgesInputRect.y, 20, 20 };
    okButton = { edgesQuestionButton.x + edgesQuestionButton.width + 15, edgesInputRect.y, 50, 30 };

    // Example Buttons
    k5Button = { 30, 650, 60, 30 };
    c6Button = { k5Button.x + k5Button.width + 10, 650, 60, 30 };
    p4Button = { c6Button.x + c6Button.width + 10, 650, 60, 30 };
    s7Button = { p4Button.x + p4Button.width + 10, 650, 60, 30 };

    // MST Menu
    float mstMenuWidth = screenWidth * 0.85f;
    float mstMenuHeight = screenHeight * 0.9f;
    float mstMenuX = (screenWidth - mstMenuWidth) / 2.0f;
    float mstMenuY = (screenHeight - mstMenuHeight) / 2.0f;
    mstMenuRect = { mstMenuX, mstMenuY, mstMenuWidth, mstMenuHeight };

    const int mstButtonWidth = 80;
    const int mstButtonHeight = 30;
    const int mstButtonSpacing = 10;
    const float mstButtonsY = mstMenuRect.y + mstMenuRect.height - mstButtonHeight - 10;
    primButton = {mstMenuRect.x + 10, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
    kruskalButton = {primButton.x + mstButtonWidth + mstButtonSpacing, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
    cancelButton = {kruskalButton.x + mstButtonWidth + mstButtonSpacing, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};

    // Edit Mode Elements
    const float editPanelWidth = 130.0f;
    const float editPanelX = screenWidth - editPanelWidth;
    addVertexButtonRect = { editPanelX + 10, 10, editPanelWidth - 20, 30 };
    addEdgeButtonRect = { editPanelX + 10, 50, editPanelWidth - 20, 30 };
    editWeightButtonRect = { editPanelX + 10, 90, editPanelWidth - 20, 30 };
    moveNodeButtonRect = { editPanelX + 10, 130, editPanelWidth - 20, 30 };
    deleteVertexButtonRect = { editPanelX + 10, 170, editPanelWidth - 20, 30 };
    deleteEdgeButtonRect = { editPanelX + 10, 210, editPanelWidth - 20, 30 };
    doneButtonRect = { editPanelX + 10, 250, editPanelWidth - 20, 30 };
    weightInputRect = {0, 0, 60, 25};
}

void GraphMain::Run() {
    UpdateGraph();

    BeginDrawing();
    ClearBackground(WHITE);
    DrawGraph();
    DrawUI();
    Rectangle backButtonBounds = { 20, 20, 65, 65 };
    DrawBackButton(backButtonTexture, backButtonBounds, backButtonClicked);
    if (CheckCollisionPointRec(GetMousePosition(), backButtonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        backToMainMenu = true;
    }
    EndDrawing();

    if (backToMainMenu) {
        shouldClose = true;
    }
}

void GraphMain::UpdateGraph()
{
    Vector2 mousePos = GetMousePosition();
    Vector2 worldMousePos = GetScreenToWorld2D(mousePos, graphCamera); // Mouse in graph space
    bool clickedOnUI = false;
    float wheel = GetMouseWheelMove();

    // Check Left Panel Buttons (if not in a modal state like MST menu or Matrix input)
     if (!showMSTMenu && !showMatrixInput && !isEditingGraph) {
         if (CheckCollisionPointRec(mousePos, createButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             clickedOnUI = true;
             isCreating = false;
             isRandomizing = false;
             isShowingExamples = false;
             showMatrixInput = false;
         
             // Reset input fields
             numNodesStr = "";
             numEdgesStr = "";
             matrixInput = ""; // Reset matrix input field
             nodesFocused = false;
             edgesFocused = false;
             matrixInputFocused = false; // Add this
         
             // Reset UI states
             showMSTMenu = false;
             isEditingGraph = false;
             currentTool = EditTool::TOOL_NONE;
             showExampleButtons = false; // Add this
         
             // Reset error messages and flags
             showFileError = false;
             fileErrorMessage = "";
             showError = false; // General/Create error
             errorMessage = "";
             showMSTError = false;
             mstErrorMessage = "";
             errorMessageInput = ""; // Add this
         
             // Reset Activity Flags
             isCreatingActive = false;
             isRandomizingActive = false;
             isShowingExamplesActive = false;
             isFileActive = false;
         
             // Reset Editing state
             selectedNodeIndex = -1;
             selectedEdgeIndex = -1;
             isDraggingNode = false;
             isEditingWeight = false;
             weightInputBuffer = "";
             mstPseudocodeScrollY = 0.0f; // Reset cuộn
             edges.clear(); nodePositions.clear(); // Clear graph data
             isCreating = true; // Enter create mode (input fields)
             isCreatingActive = true;
             showMSTError = false;
             showMSTMenu = false;
             isShowingExamples = false;
             showMatrixInput = false;
             graphDrawn = false;
             matrixInput = ""; // Reset matrix input field
             showMSTMenu = false;
             isEditingGraph = false;
             currentTool = EditTool::TOOL_NONE;
             showFileError = false;
             fileErrorMessage = "";
             showMSTError = false;
             mstErrorMessage = "";
             errorMessageInput = "";
         }
         else if (CheckCollisionPointRec(mousePos, randomButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             clickedOnUI = true;
             isCreating = false;
             isRandomizing = false;
             isShowingExamples = false;
             showMatrixInput = false;
             // graphDrawn = false; // Reset graphDrawn ONLY if appropriate for all calls
         
             // Reset input fields
             numNodesStr = "";
             numEdgesStr = "";
             matrixInput = ""; // Reset matrix input field
             nodesFocused = false;
             edgesFocused = false;
             matrixInputFocused = false; // Add this
         
             // Reset UI states
             showMSTMenu = false;
             isEditingGraph = false;
             currentTool = EditTool::TOOL_NONE;
             showExampleButtons = false; // Add this
         
             // Reset error messages and flags
             showFileError = false;
             fileErrorMessage = "";
             showError = false; // General/Create error
             errorMessage = "";
             showMSTError = false;
             mstErrorMessage = "";
             errorMessageInput = ""; // Add this
         
             // Reset Activity Flags
             isCreatingActive = false;
             isRandomizingActive = false;
             isShowingExamplesActive = false;
             isFileActive = false;
         
             // Reset Editing state
             selectedNodeIndex = -1;
             selectedEdgeIndex = -1;
             isDraggingNode = false;
             isEditingWeight = false;
             weightInputBuffer = "";
             edges.clear(); nodePositions.clear(); // Clear graph data

             // Generate random graph immediately
             int r_numNodes = GetRandomValue(5, 12); // Example range
             int r_maxEdges = r_numNodes * (r_numNodes - 1) / 2;
             int r_minEdges = (r_numNodes > 1) ? r_numNodes - 1 : 0; // Min edges for connectivity
             int r_numEdges = (r_minEdges >= r_maxEdges) ? r_maxEdges : GetRandomValue(r_minEdges, r_maxEdges);

             generateRandomGraph(edges, r_numNodes, r_numEdges);
             if (r_numNodes > 0) {
                float layoutRadius = std::min(screenWidth - uiLeftWidth - uiRightWidth, screenHeight - 230.0f) / 3.0f; // Adjusted for UI
                positionNodesInCircle(nodePositions, r_numNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
                graphDrawn = true;
                UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, uiLeftWidth, uiRightWidth);
            }
             isRandomizing = false; // Reset flag after generation
             showMatrixInput = false;
             matrixInput = ""; // Reset matrix input field
             showMSTMenu = false;
             isEditingGraph = false;
             currentTool = EditTool::TOOL_NONE;
             showFileError = false;
             fileErrorMessage = "";
             showMSTError = false;
             mstErrorMessage = "";
             errorMessageInput = "";
         }
         else if (CheckCollisionPointRec(mousePos, exampleButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             clickedOnUI = true;
             isCreating = false;
             isRandomizing = false;
             isShowingExamples = false;
             showMatrixInput = false;
             // graphDrawn = false; // Reset graphDrawn ONLY if appropriate for all calls
         
             // Reset input fields
             numNodesStr = "";
             numEdgesStr = "";
             matrixInput = ""; // Reset matrix input field
             nodesFocused = false;
             edgesFocused = false;
             matrixInputFocused = false; // Add this
         
             // Reset UI states
             showMSTMenu = false;
             isEditingGraph = false;
             currentTool = EditTool::TOOL_NONE;
             showExampleButtons = false; // Add this
         
             // Reset error messages and flags
             showFileError = false;
             fileErrorMessage = "";
             showError = false; // General/Create error
             errorMessage = "";
             showMSTError = false;
             mstErrorMessage = "";
             errorMessageInput = ""; // Add this
         
             // Reset Activity Flags
             isCreatingActive = false;
             isRandomizingActive = false;
             isShowingExamplesActive = false;
             isFileActive = false;
         
             // Reset Editing state
             selectedNodeIndex = -1;
             selectedEdgeIndex = -1;
             isDraggingNode = false;
             isEditingWeight = false;
             weightInputBuffer = "";
             edges.clear(); nodePositions.clear(); // Clear graph data
             showExampleButtons = !showExampleButtons; // Toggle visibility
             isShowingExamplesActive = showExampleButtons;
             isCreating = false;
         }
         else if (CheckCollisionPointRec(mousePos, fileButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             clickedOnUI = true;
             isCreating = false;
             isRandomizing = false;
             isShowingExamples = false;
             showMatrixInput = false;
             // graphDrawn = false; // Reset graphDrawn ONLY if appropriate for all calls
         
             // Reset input fields
             numNodesStr = "";
             numEdgesStr = "";
             matrixInput = ""; // Reset matrix input field
             nodesFocused = false;
             edgesFocused = false;
             matrixInputFocused = false; // Add this
         
             // Reset UI states
             showMSTMenu = false;
             isEditingGraph = false;
             currentTool = EditTool::TOOL_NONE;
             showExampleButtons = false; // Add this
         
             // Reset error messages and flags
             showFileError = false;
             fileErrorMessage = "";
             showError = false; // General/Create error
             errorMessage = "";
             showMSTError = false;
             mstErrorMessage = "";
             errorMessageInput = ""; // Add this
         
             // Reset Activity Flags
             isCreatingActive = false;
             isRandomizingActive = false;
             isShowingExamplesActive = false;
             isFileActive = false;
         
             // Reset Editing state
             selectedNodeIndex = -1;
             selectedEdgeIndex = -1;
             isDraggingNode = false;
             isEditingWeight = false;
             weightInputBuffer = "";
             edges.clear(); nodePositions.clear(); // Clear graph data
             isFileActive = true; // Indicate file operation
             isCreating = false;
             showExampleButtons = false;

             // --- TinyFileDialog Logic ---
             const char * filterPatterns[1] = { "*.txt" };
             const char * selectedFilePath = tinyfd_openFileDialog(
                 "Select Graph File (Adjacency Matrix)", "", 1, filterPatterns, "Text Files (*.txt)", 0);

             if (selectedFilePath != NULL) {
                 std::string loadError;
                 bool success = LoadGraphFromFile(selectedFilePath, edges, nodePositions, screenWidth, screenHeight, loadError);
                 if (success) {
                     graphDrawn = true;
                     if (!nodePositions.empty()) {
                          UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 0.0f, 0.0f);
                     }
                 } else {
                     errorMessageInput = ""; // Clear errors
                     graphDrawn = false; // Ensure no drawing if load fails
                     fileErrorMessage = loadError;
                     showFileError = true;
                     isFileActive = false; 
                     
                 }
             } else {
                isFileActive = false; // Reset flag
             }
         }
          else if (CheckCollisionPointRec(mousePos, editButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
              clickedOnUI = true;
              if (graphDrawn && !nodePositions.empty()) {
                  isCreating = false;
                  isRandomizing = false;
                  isShowingExamples = false;
                  showMatrixInput = false;
                  // graphDrawn = false; // Reset graphDrawn ONLY if appropriate for all calls
              
                  // Reset input fields
                  numNodesStr = "";
                  numEdgesStr = "";
                  matrixInput = ""; // Reset matrix input field
                  nodesFocused = false;
                  edgesFocused = false;
                  matrixInputFocused = false; // Add this
              
                  // Reset UI states
                  showMSTMenu = false;
                  isEditingGraph = false;
                  currentTool = EditTool::TOOL_NONE;
                  showExampleButtons = false; // Add this
              
                  // Reset error messages and flags
                  showFileError = false;
                  fileErrorMessage = "";
                  showError = false; // General/Create error
                  errorMessage = "";
                  showMSTError = false;
                  mstErrorMessage = "";
                  errorMessageInput = ""; // Add this
              
                  // Reset Activity Flags
                  isCreatingActive = false;
                  isRandomizingActive = false;
                  isShowingExamplesActive = false;
                  isFileActive = false;
              
                  // Reset Editing state
                  selectedNodeIndex = -1;
                  selectedEdgeIndex = -1;
                  isDraggingNode = false;
                  isEditingWeight = false;
                  weightInputBuffer = "";
                  graphDrawn = true;
                  isEditingGraph = true;
                  currentTool = EditTool::TOOL_NONE; // Start with no tool selected
                  selectedNodeIndex = -1;
                  selectedEdgeIndex = -1;
                  isDraggingNode = false;
                  isEditingWeight = false;
                  showExampleButtons = false;
                  isCreating = false;
              } else {
                  isCreating = false;
                  isRandomizing = false;
                  isShowingExamples = false;
                  showMatrixInput = false;
                  // graphDrawn = false; // Reset graphDrawn ONLY if appropriate for all calls
              
                  // Reset input fields
                  numNodesStr = "";
                  numEdgesStr = "";
                  matrixInput = ""; // Reset matrix input field
                  nodesFocused = false;
                  edgesFocused = false;
                  matrixInputFocused = false; // Add this
              
                  // Reset UI states
                  showMSTMenu = false;
                  isEditingGraph = false;
                  currentTool = EditTool::TOOL_NONE;
                  showExampleButtons = false; // Add this
              
                  // Reset error messages and flags
                  showFileError = false;
                  fileErrorMessage = "";
                  showError = false; // General/Create error
                  errorMessage = "";
                  showMSTError = false;
                  mstErrorMessage = "";
                  errorMessageInput = ""; // Add this
              
                  // Reset Activity Flags
                  isCreatingActive = false;
                  isRandomizingActive = false;
                  isShowingExamplesActive = false;
                  isFileActive = false;
              
                  // Reset Editing state
                  selectedNodeIndex = -1;
                  selectedEdgeIndex = -1;
                  isDraggingNode = false;
                  isEditingWeight = false;
                  weightInputBuffer = "";
                  edges.clear(); nodePositions.clear(); // Clear just in case
                  showError = true;
                  errorMessage = "Create or load a graph first to edit.";
                  isEditingGraph = false; // Make sure edit mode is off
              }
          }
         else if (CheckCollisionPointRec(mousePos, mstButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             clickedOnUI = true;
             if (graphDrawn && !nodePositions.empty()) {
                 isCreating = false;
                 isRandomizing = false;
                 isShowingExamples = false;
                 showMatrixInput = false;
                 // graphDrawn = false; // Reset graphDrawn ONLY if appropriate for all calls
             
                 // Reset input fields
                 numNodesStr = "";
                 numEdgesStr = "";
                 matrixInput = ""; // Reset matrix input field
                 nodesFocused = false;
                 edgesFocused = false;
                 matrixInputFocused = false; // Add this
             
                 // Reset UI states
                 showMSTMenu = false;
                 isEditingGraph = false;
                 currentTool = EditTool::TOOL_NONE;
                 showExampleButtons = false; // Add this
             
                 // Reset error messages and flags
                 showFileError = false;
                 fileErrorMessage = "";
                 showError = false; // General/Create error
                 errorMessage = "";
                 showMSTError = false;
                 mstErrorMessage = "";
                 errorMessageInput = ""; // Add this
             
                 // Reset Activity Flags
                 isCreatingActive = false;
                 isRandomizingActive = false;
                 isShowingExamplesActive = false;
                 isFileActive = false;
             
                 // Reset Editing state
                 selectedNodeIndex = -1;
                 selectedEdgeIndex = -1;
                 isDraggingNode = false;
                 isEditingWeight = false;
                 weightInputBuffer = "";
                 graphDrawn = true;
                 showMSTMenu = true;
                 usePrim = false; useKruskal = false; // Reset algorithm choice
                 mstEdges.clear(); mstEdgesDrawn.clear(); mstNodesDrawn.clear();
                 mstEdgeIndex = 0; mstDrawTimer = 0.0f; mstDoneDrawing = false;
                 totalMSTWeight = 0; showWeightInfo = false; isMSTDrawingPaused = false;
                 isCreating = false;
                 showExampleButtons = false;
             } else {
                 isCreating = false;
                 isRandomizing = false;
                 isShowingExamples = false;
                 showMatrixInput = false;
                 // graphDrawn = false; // Reset graphDrawn ONLY if appropriate for all calls
             
                 // Reset input fields
                 numNodesStr = "";
                 numEdgesStr = "";
                 matrixInput = ""; // Reset matrix input field
                 nodesFocused = false;
                 edgesFocused = false;
                 matrixInputFocused = false; // Add this
             
                 // Reset UI states
                 showMSTMenu = false;
                 isEditingGraph = false;
                 currentTool = EditTool::TOOL_NONE;
                 showExampleButtons = false; // Add this
             
                 // Reset error messages and flags
                 showFileError = false;
                 fileErrorMessage = "";
                 showError = false; // General/Create error
                 errorMessage = "";
                 showMSTError = false;
                 mstErrorMessage = "";
                 errorMessageInput = ""; // Add this
             
                 // Reset Activity Flags
                 isCreatingActive = false;
                 isRandomizingActive = false;
                 isShowingExamplesActive = false;
                 isFileActive = false;
             
                 // Reset Editing state
                 selectedNodeIndex = -1;
                 selectedEdgeIndex = -1;
                 isDraggingNode = false;
                 isEditingWeight = false;
                 weightInputBuffer = "";
                 edges.clear(); nodePositions.clear(); // Clear just in case
                 // showMSTError = true;
                 showError = true;
                 errorMessage = "Create or load a graph first for MST.";
                 // mstErrorMessage = "Create or load a graph first for MST.";
                 showMSTMenu = false; // Make sure MST menu is off
             }
         }
     }

    // Check Example Buttons (if visible)
     if (showExampleButtons && !clickedOnUI) {
         int exampleNodes = 0;
         auto generateAndUpdate = [&](int n, void (*genFunc)(std::vector<Edge>&, int)) {
             clickedOnUI = true;
             genFunc(edges, n);
             exampleNodes = n;
             nodePositions.resize(exampleNodes);
             float layoutRadius = std::min(screenWidth, screenHeight) / 3.0f;
             positionNodesInCircle(nodePositions, exampleNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
             graphDrawn = true;
             showExampleButtons = false;
             isShowingExamplesActive = false;
             UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 0.0f, 0.0f);
         };
          auto generatePathAndUpdate = [&](int n) { // Special case for path layout
             clickedOnUI = true;
             generatePathGraph(edges, n, nodePositions, screenWidth, screenHeight); // nodePositions updated by func
             exampleNodes = n;
             // nodePositions already set by generatePathGraph
             graphDrawn = true;
             showExampleButtons = false;
             isShowingExamplesActive = false;
             UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 0.0f, 0.0f);
         };


         if (CheckCollisionPointRec(mousePos, k5Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
         {
            generateAndUpdate(5, generateCompleteGraph);
            nodePositions.resize(5);
            float layoutRadius = std::min(screenWidth - uiLeftWidth - uiRightWidth, screenHeight - 230.0f) / 3.0f;
            positionNodesInCircle(nodePositions, 5, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
            graphDrawn = true;
            UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, uiLeftWidth, uiRightWidth);
         }
         else if (CheckCollisionPointRec(mousePos, c6Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
         {
            generateAndUpdate(6, generateCycleGraph);
            nodePositions.resize(6);
            float layoutRadius = std::min(screenWidth - uiLeftWidth - uiRightWidth, screenHeight - 230.0f) / 3.0f;
            positionNodesInCircle(nodePositions, 6, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
            graphDrawn = true;
            UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, uiLeftWidth, uiRightWidth);
         }
         else if (CheckCollisionPointRec(mousePos, p4Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
         {
            generatePathAndUpdate(4); // Use path layout version
            nodePositions.resize(4);
            float layoutRadius = std::min(screenWidth - uiLeftWidth - uiRightWidth, screenHeight - 230.0f) / 3.0f;
            positionNodesInCircle(nodePositions, 4, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
            graphDrawn = true;
            UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, uiLeftWidth, uiRightWidth);
         }
         else if (CheckCollisionPointRec(mousePos, s7Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
         {
           generateAndUpdate(7, generateStarGraph);
           nodePositions.resize(7);
           float layoutRadius = std::min(screenWidth - uiLeftWidth - uiRightWidth, screenHeight - 230.0f) / 3.0f;
           positionNodesInCircle(nodePositions, 5, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
           graphDrawn = true;
           UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, uiLeftWidth, uiRightWidth);
         } 
     }

    // Check Create Mode Inputs (if active)
     if (isCreating && !clickedOnUI) {
         bool nodesClicked = CheckCollisionPointRec(mousePos, nodesInputRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
         bool edgesClicked = CheckCollisionPointRec(mousePos, edgesInputRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

         if (nodesClicked) { nodesFocused = true; edgesFocused = false; clickedOnUI = true; }
         if (edgesClicked) { nodesFocused = false; edgesFocused = true; clickedOnUI = true; }
         if (!nodesClicked && !edgesClicked && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             nodesFocused = false; edgesFocused = false; // Lose focus on click outside
         }

         if (nodesFocused) {
             int key = GetCharPressed();
             if (key >= '0' && key <= '9' && numNodesStr.length() < 3) numNodesStr += (char)key;
             if (IsKeyPressed(KEY_BACKSPACE) && !numNodesStr.empty()) numNodesStr.pop_back();
         }
         if (edgesFocused) {
             int key = GetCharPressed();
             if (key >= '0' && key <= '9' && numEdgesStr.length() < 5) numEdgesStr += (char)key;
             if (IsKeyPressed(KEY_BACKSPACE) && !numEdgesStr.empty()) numEdgesStr.pop_back();
         }
         if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || CheckCollisionPointRec(mousePos, okButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
             clickedOnUI = true; // Treat enter as UI interaction
             int numNodes = 0, numEdges = -1;
             try {
                 if (!numNodesStr.empty()) numNodes = std::stoi(numNodesStr);
                 if (!numEdgesStr.empty()) numEdges = std::stoi(numEdgesStr);

                 int maxEdges = (numNodes * (numNodes - 1)) / 2;
                 int minEdges = (numNodes > 1) ? numNodes - 1 : 0;

                 if (numNodes <= 0 || numEdges < 0) {
                     showError = true; errorMessage = "Nodes must be > 0, Edges must be >= 0.";
                 } else if (numEdges < minEdges && numNodes > 1) {
                     showError = true; errorMessage = "Not enough edges for a connected graph.";
                 } else if (numEdges > maxEdges) {
                     showError = true; errorMessage = "Too many edges for this many nodes.";
                 } else {
                     // Generate graph
                     generateRandomGraph(edges, numNodes, numEdges);
                     // Check connectivity again after generation (generateRandomGraph tries, but double check)
                     if (numNodes > 1 && !isGraphConnected(edges, numNodes)) {
                          showError = true; errorMessage = "Could not generate a connected graph.";
                          edges.clear(); // Clear failed attempt
                          nodePositions.clear();
                          graphDrawn = false;
                     } else {
                         nodePositions.resize(numNodes);
                         float layoutRadius = std::min(screenWidth - uiLeftWidth - uiRightWidth, screenHeight - 230.0f) / 3.0f;
                         positionNodesInCircle(nodePositions, numNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
                         graphDrawn = true;
                         isCreating = false; // Exit create mode
                         isCreatingActive = false;
                         showError = false;
                         UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, uiLeftWidth, uiRightWidth);
                     }
                 }
             } catch (const std::exception& e) {
                 showError = true; errorMessage = "Invalid number input.";
             }
         }
         if (CheckCollisionPointRec(mousePos, nodesQuestionButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            clickedOnUI = true;
            int randomNodes = GetRandomValue(5, 20); // Same range as random graph
            numNodesStr = std::to_string(randomNodes);
            nodesFocused = true;
            edgesFocused = false;
        }
        if (CheckCollisionPointRec(mousePos, edgesQuestionButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            clickedOnUI = true;
            try {
                int numNodes = numNodesStr.empty() ? 0 : std::stoi(numNodesStr);
                if (numNodes > 1) {
                    int maxEdges = (numNodes * (numNodes - 1)) / 2;
                    int minEdges = numNodes - 1;
                    int randomEdges = GetRandomValue(minEdges, maxEdges);
                    numEdgesStr = std::to_string(randomEdges);
                    edgesFocused = true;
                    nodesFocused = false;
                } else {
                    numEdgesStr = "";
                    errorMessage = "Please enter number of nodes first.";
                    showError = true;
                }
            } catch (...) {
                errorMessage = "Invalid number of nodes.";
                showError = true;
            }
        }
         
     }

    // Check Edit Mode Interactions (if active)
     if (isEditingGraph && !clickedOnUI) {
         bool clickedOnEditUI = false;
          // Check Right Panel (Edit Tools)
          Rectangle editButtons[] = { addVertexButtonRect, addEdgeButtonRect, editWeightButtonRect, moveNodeButtonRect, deleteVertexButtonRect, deleteEdgeButtonRect, doneButtonRect };
          for(const auto& btn : editButtons) {
              if (CheckCollisionPointRec(mousePos, btn)) {
                  clickedOnEditUI = true;
                  break;
              }
          }
          // Also consider the weight input box as UI
          if (isEditingWeight && CheckCollisionPointRec(mousePos, weightInputRect)) {
              clickedOnEditUI = true;
          }

          if (clickedOnEditUI) {
             clickedOnUI = true; // Click on edit panel is UI interaction

             // Handle Edit Tool Button Clicks
             if (CheckCollisionPointRec(mousePos, addVertexButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentTool = EditTool::TOOL_ADD_VERTEX;
             else if (CheckCollisionPointRec(mousePos, addEdgeButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { currentTool = EditTool::TOOL_ADD_EDGE_START; selectedNodeIndex = -1; } // Reset selection
             else if (CheckCollisionPointRec(mousePos, editWeightButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { currentTool = EditTool::TOOL_EDIT_WEIGHT; selectedEdgeIndex = -1; } // Reset selection
              else if (CheckCollisionPointRec(mousePos, moveNodeButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { currentTool = EditTool::TOOL_MOVE_VERTEX; selectedNodeIndex = -1; isDraggingNode = false;}
              else if (CheckCollisionPointRec(mousePos, deleteVertexButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentTool = EditTool::TOOL_DELETE_VERTEX;
              else if (CheckCollisionPointRec(mousePos, deleteEdgeButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentTool = EditTool::TOOL_DELETE_EDGE;
             else if (CheckCollisionPointRec(mousePos, doneButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                isEditingGraph = false;
                currentTool = EditTool::TOOL_NONE;
                if (!nodePositions.empty()) {
                    UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, uiLeftWidth, uiRightWidth);
                }
             }

             // Reset states when changing tools
             if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                  isEditingWeight = false;
                  isDraggingNode = false;
                  // Keep selectedNodeIndex if switching between ADD_EDGE_START/END? No, reset generally safer.
                  selectedNodeIndex = -1; // Reset unless specific tool logic needs it
                  selectedEdgeIndex = -1;
             }

          } // end if clickedOnEditUI

         // --- Handle Graph Interactions (if not clicking Edit UI) ---
         if (!clickedOnUI) { // Only process graph clicks if no UI was clicked
             if (isEditingWeight) {
                 int key = GetCharPressed();
                 if (key >= '0' && key <= '9' && weightInputBuffer.length() < 5) weightInputBuffer += (char)key;
                 if (IsKeyPressed(KEY_BACKSPACE) && !weightInputBuffer.empty()) weightInputBuffer.pop_back();

                 if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                     int newWeight = 1;
                     try { newWeight = std::max(1, std::stoi(weightInputBuffer)); } catch(...) {} // Default to 1 on error, ensure positive

                     if (currentTool == EditTool::TOOL_ADD_EDGE_END && selectedNodeIndex != -1 && selectedEdgeIndex != -1 /*using as temp node2 index*/) {
                          int node1Idx = selectedNodeIndex;
                          int node2Idx = selectedEdgeIndex; // Retrieve second node index
                         if (!edgeExists(edges, node1Idx + 1, node2Idx + 1)) {
                             edges.push_back({node1Idx + 1, node2Idx + 1, newWeight});
                             TraceLog(LOG_INFO, "EDIT: Added edge %d-%d (%d)", node1Idx+1, node2Idx+1, newWeight);
                         }
                          currentTool = EditTool::TOOL_ADD_EDGE_START; // Go back to selecting first node
                     } else if (currentTool == EditTool::TOOL_EDIT_WEIGHT && selectedEdgeIndex != -1 && selectedEdgeIndex < edges.size()) {
                          edges[selectedEdgeIndex].weight = newWeight;
                          TraceLog(LOG_INFO, "EDIT: Changed edge %d weight to %d", selectedEdgeIndex, newWeight);
                     }
                     // Reset after successful edit/add
                     isEditingWeight = false; weightInputBuffer = ""; selectedNodeIndex = -1; selectedEdgeIndex = -1;
                 }
                 // Cancel weight edit if clicked outside input box
                 if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePos, weightInputRect)) {
                     isEditingWeight = false; weightInputBuffer = "";
                     if (currentTool == EditTool::TOOL_ADD_EDGE_END) currentTool = EditTool::TOOL_ADD_EDGE_START; // Reset add edge
                     selectedNodeIndex = -1; selectedEdgeIndex = -1;
                 }
             } // end if isEditingWeight
             else { // Not editing weight, handle other graph interactions
                 // --- Node Dragging ---
                 if (isDraggingNode && currentTool == EditTool::TOOL_MOVE_VERTEX && selectedNodeIndex != -1) {
                     if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                         nodePositions[selectedNodeIndex] = worldMousePos; // Update position while dragging
                     }
                     if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                          isDraggingNode = false;
                         // Keep selectedNodeIndex = -1; ? Or allow multi-drag? Reset for now.
                         selectedNodeIndex = -1;
                         TraceLog(LOG_INFO, "EDIT: Finished dragging node %d", selectedNodeIndex + 1);
                         // Keep currentTool as TOOL_MOVE_VERTEX
                     }
                 }
                  // --- Mouse Click Processing ---
                 else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                     int clickedNodeIdx = -1;
                     for (int i = 0; i < nodePositions.size(); ++i) {
                         if (CheckCollisionPointCircle(worldMousePos, nodePositions[i], 20.0f)) {
                             clickedNodeIdx = i;
                             break;
                         }
                     }

                     int clickedEdgeIdx = -1;
                     if (clickedNodeIdx == -1 && (currentTool == EditTool::TOOL_EDIT_WEIGHT || currentTool == EditTool::TOOL_DELETE_EDGE)) {
                          // Find closest edge midpoint (simple check)
                          float minDistSq = 40.0f * 40.0f; // Increased tolerance
                         for (int i = 0; i < edges.size(); ++i) {
                             int fromIdx = edges[i].from - 1;
                             int toIdx = edges[i].to - 1;
                             if (fromIdx >= 0 && fromIdx < nodePositions.size() && toIdx >= 0 && toIdx < nodePositions.size()) {
                                 Vector2 mid = Vector2Lerp(nodePositions[fromIdx], nodePositions[toIdx], 0.5f);
                                 float distSq = Vector2DistanceSqr(worldMousePos, mid);
                                  // Check if click is roughly along the line segment
                                 if (distSq < minDistSq && CheckCollisionPointLine(worldMousePos, nodePositions[fromIdx], nodePositions[toIdx], 15.0f/graphCamera.zoom)) { // Check line collision
                                     clickedEdgeIdx = i;
                                     minDistSq = distSq;
                                 }
                             }
                         }
                     }

                     // --- Apply Tool Logic Based on Click ---
                     switch(currentTool) {
                         case EditTool::TOOL_ADD_VERTEX:
                             if (clickedNodeIdx == -1 && clickedEdgeIdx == -1) {
                                 nodePositions.push_back(worldMousePos);
                                 TraceLog(LOG_INFO, "EDIT: Added vertex %zu", nodePositions.size());
                                 // Optional: Update camera if graph expands significantly?
                             }
                             break;
                         case EditTool::TOOL_ADD_EDGE_START:
                             if (clickedNodeIdx != -1) {
                                 selectedNodeIndex = clickedNodeIdx;
                                 currentTool = EditTool::TOOL_ADD_EDGE_END; // Move to next step
                                 TraceLog(LOG_INFO, "EDIT: Selected start node %d", selectedNodeIndex + 1);
                             }
                             break;
                         case EditTool::TOOL_ADD_EDGE_END:
                             if (clickedNodeIdx != -1) {
                                 if (clickedNodeIdx != selectedNodeIndex) {
                                     // Check if edge exists
                                     if (!edgeExists(edges, selectedNodeIndex + 1, clickedNodeIdx + 1)) {
                                          // Start weight input
                                         isEditingWeight = true;
                                         weightInputBuffer = "1";
                                         selectedEdgeIndex = clickedNodeIdx; // Temporarily store second node index here
                                         // Calculate input position
                                          Vector2 p1_screen = GetWorldToScreen2D(nodePositions[selectedNodeIndex], graphCamera);
                                          Vector2 p2_screen = GetWorldToScreen2D(nodePositions[clickedNodeIdx], graphCamera);
                                          Vector2 mid_screen = Vector2Lerp(p1_screen, p2_screen, 0.5f);
                                         weightInputRect.x = mid_screen.x - weightInputRect.width / 2.0f;
                                         weightInputRect.y = mid_screen.y - weightInputRect.height / 2.0f;
                                         TraceLog(LOG_INFO, "EDIT: Selected end node %d, enter weight.", clickedNodeIdx + 1);
                                     } else {
                                          TraceLog(LOG_INFO, "EDIT: Edge already exists.");
                                         currentTool = EditTool::TOOL_ADD_EDGE_START; // Reset
                                         selectedNodeIndex = -1;
                                     }
                                 } else { // Clicked same node again
                                     currentTool = EditTool::TOOL_ADD_EDGE_START; // Cancel start node
                                     selectedNodeIndex = -1;
                                     TraceLog(LOG_INFO, "EDIT: Cancelled edge start.");
                                 }
                             } else { // Clicked empty space
                                 currentTool = EditTool::TOOL_ADD_EDGE_START; // Cancel start node
                                 selectedNodeIndex = -1;
                                 TraceLog(LOG_INFO, "EDIT: Cancelled edge creation.");
                             }
                             break;
                         case EditTool::TOOL_EDIT_WEIGHT:
                             if (clickedEdgeIdx != -1 || clickedNodeIdx ==-1) {
                                 isEditingWeight = true;
                                 selectedEdgeIndex = clickedEdgeIdx;
                                 weightInputBuffer = std::to_string(edges[clickedEdgeIdx].weight);
                                 // Calculate input position
                                  Vector2 p1_screen = GetWorldToScreen2D(nodePositions[edges[clickedEdgeIdx].from - 1], graphCamera);
                                  Vector2 p2_screen = GetWorldToScreen2D(nodePositions[edges[clickedEdgeIdx].to - 1], graphCamera);
                                  Vector2 mid_screen = Vector2Lerp(p1_screen, p2_screen, 0.5f);
                                 weightInputRect.x = mid_screen.x - weightInputRect.width / 2.0f;
                                 weightInputRect.y = mid_screen.y - weightInputRect.height / 2.0f;
                                 TraceLog(LOG_INFO, "EDIT: Selected edge %d for weight edit.", clickedEdgeIdx);
                             } else {
                                  selectedEdgeIndex = -1; // Clicked elsewhere, deselect edge
                             }
                             break;
                          case EditTool::TOOL_MOVE_VERTEX:
                             if (clickedNodeIdx != -1) {
                                 isDraggingNode = true;
                                 selectedNodeIndex = clickedNodeIdx;
                                 TraceLog(LOG_INFO, "EDIT: Started dragging node %d", clickedNodeIdx + 1);
                             } else {
                                  isDraggingNode = false; // Stop dragging if clicked elsewhere
                                  // selectedNodeIndex = -1; // Deselect node? Keep tool active.
                             }
                             break;
                         case EditTool::TOOL_DELETE_VERTEX:
                             if (clickedNodeIdx != -1) {
                                 int deletedNodeId = clickedNodeIdx + 1;
                                 TraceLog(LOG_INFO, "EDIT: Deleting vertex %d", deletedNodeId);

                                 // 1. Remove connected edges
                                 edges.erase(std::remove_if(edges.begin(), edges.end(),
                                     [deletedNodeId](const Edge& edge){ return edge.from == deletedNodeId || edge.to == deletedNodeId; }),
                                     edges.end());

                                 // 2. Remove node position
                                 nodePositions.erase(nodePositions.begin() + clickedNodeIdx);

                                 // 3. Renumber edges
                                 for (Edge& edge : edges) {
                                     if (edge.from > deletedNodeId) edge.from--;
                                     if (edge.to > deletedNodeId) edge.to--;
                                 }
                                 selectedNodeIndex = -1; // Deselect
                                  // Keep currentTool = TOOL_DELETE_VERTEX
                             }
                             break;
                         case EditTool::TOOL_DELETE_EDGE:
                             if (clickedEdgeIdx != -1) {
                                 TraceLog(LOG_INFO, "EDIT: Deleting edge %d (%d-%d)", clickedEdgeIdx, edges[clickedEdgeIdx].from, edges[clickedEdgeIdx].to);
                                 edges.erase(edges.begin() + clickedEdgeIdx);
                                 selectedEdgeIndex = -1; // Deselect
                                  // Keep currentTool = TOOL_DELETE_EDGE
                             }
                             break;
                         case EditTool::TOOL_NONE:
                         default:
                             break; // Do nothing
                     } // end switch(currentTool)
                 } // end else (if not dragging) / end if MouseButtonPressed
             } // end else (if not editing weight)
         } // end if (!clickedOnUI) - Graph interactions
     } // end if isEditingGraph

    // --- Update MST Animation (if active) ---
     if (showMSTMenu && (usePrim || useKruskal) && !mstDoneDrawing && mstEdgesDrawn.size() == mstEdges.size()) {
        if (!isMSTDrawingPaused) {
            mstDrawTimer += GetFrameTime();
            float drawInterval = speedSlider.getValue();

            if (mstDrawTimer >= drawInterval) {
                if (mstEdgeIndex < mstEdges.size()) {
                    if (usePrim) {
                        switch (currentHighlightLine) {
                            case 3: // While loop
                                currentHighlightLine = 4; // extract_min
                                break;
                            case 4: // extract_min
                                currentHighlightLine = 5; // check Visited
                                break;
                            case 5: // check Visited
                                if (!mstEdgesDrawn[mstEdgeIndex]) {
                                    mstEdgesDrawn[mstEdgeIndex] = true;
                                    totalMSTWeight += mstEdges[mstEdgeIndex].weight;
                                    if (mstEdges[mstEdgeIndex].from > 0 && mstEdges[mstEdgeIndex].from <= mstNodesDrawn.size())
                                        mstNodesDrawn[mstEdges[mstEdgeIndex].from - 1] = true;
                                    if (mstEdges[mstEdgeIndex].to > 0 && mstEdges[mstEdgeIndex].to <= mstNodesDrawn.size())
                                        mstNodesDrawn[mstEdges[mstEdgeIndex].to - 1] = true;
                                    currentHighlightLine = 6; // add edge
                                }
                                break;
                            case 6: // add edge
                                currentHighlightLine = 7; // add v
                                break;
                            case 7: // add v
                                currentHighlightLine = 8; // add edges to PQ
                                break;
                            case 8: // add edges to PQ
                                mstEdgeIndex++;
                                currentHighlightLine = 3; // quay lại while
                                break;
                            default:
                                currentHighlightLine = 3;
                                break;
                        }
                    } else if (useKruskal) {
                        switch (currentHighlightLine) {
                            case 4: // for each edge
                                currentHighlightLine = 5; // findSet
                                break;
                            case 5: // findSet
                                if (!mstEdgesDrawn[mstEdgeIndex]) {
                                    mstEdgesDrawn[mstEdgeIndex] = true;
                                    totalMSTWeight += mstEdges[mstEdgeIndex].weight;
                                    if (mstEdges[mstEdgeIndex].from > 0 && mstEdges[mstEdgeIndex].from <= mstNodesDrawn.size())
                                        mstNodesDrawn[mstEdges[mstEdgeIndex].from - 1] = true;
                                    if (mstEdges[mstEdgeIndex].to > 0 && mstEdges[mstEdgeIndex].to <= mstNodesDrawn.size())
                                        mstNodesDrawn[mstEdges[mstEdgeIndex].to - 1] = true;
                                    currentHighlightLine = 6; // add edge
                                }
                                break;
                            case 6: // add edge
                                currentHighlightLine = 7; // union
                                break;
                            case 7: // union
                                mstEdgeIndex++;
                                currentHighlightLine = 4; // tiếp tục for
                                break;
                            default:
                                currentHighlightLine = 4;
                                break;
                        }
                    }

                    mstDrawTimer = 0.0f;
                    if (mstEdgeIndex == mstEdges.size()) {
                        mstDoneDrawing = true;
                        currentHighlightLine = 9; // Return MST
                    }
                } else {
                    mstDoneDrawing = true;
                    currentHighlightLine = 9; // Return MST
                }
            }
        }
        cursorTimer += GetFrameTime();
        if (cursorTimer >= 0.8f) cursorTimer = 0.0f;
    }
    else if (showWeightInfo && CheckCollisionPointRec(mousePos, prevStepButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (mstEdgeIndex > 0) {
            isMSTDrawingPaused = true;
            mstDoneDrawing = false;
            mstEdgeIndex--;
            mstEdgesDrawn[mstEdgeIndex] = false;
            totalMSTWeight -= mstEdges[mstEdgeIndex].weight;

            mstNodesDrawn.assign(nodePositions.size(), false);
            for (int i = 0; i < mstEdgeIndex; ++i) {
                if (mstEdgesDrawn[i]) {
                    if (mstEdges[i].from > 0 && mstEdges[i].from <= mstNodesDrawn.size())
                        mstNodesDrawn[mstEdges[i].from - 1] = true;
                    if (mstEdges[i].to > 0 && mstEdges[i].to <= mstNodesDrawn.size())
                        mstNodesDrawn[mstEdges[i].to - 1] = true;
                }
            }
            if (usePrim) {
                currentHighlightLine = 3; // Quay lại while loop
            } else if (useKruskal) {
                currentHighlightLine = 4; // Quay lại for loop
            }
        }
    }
    else if (showWeightInfo && CheckCollisionPointRec(mousePos, nextStepButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (mstEdgeIndex < mstEdges.size()) {
            isMSTDrawingPaused = true;
            if (!mstEdgesDrawn[mstEdgeIndex]) {
                mstEdgesDrawn[mstEdgeIndex] = true;
                totalMSTWeight += mstEdges[mstEdgeIndex].weight;
                if (mstEdges[mstEdgeIndex].from > 0 && mstEdges[mstEdgeIndex].from <= mstNodesDrawn.size())
                    mstNodesDrawn[mstEdges[mstEdgeIndex].from - 1] = true;
                if (mstEdges[mstEdgeIndex].to > 0 && mstEdges[mstEdgeIndex].to <= mstNodesDrawn.size())
                    mstNodesDrawn[mstEdges[mstEdgeIndex].to - 1] = true;
                if (usePrim) {
                    currentHighlightLine = 6; // add edge
                } else if (useKruskal) {
                    currentHighlightLine = 6; // add edge
                }
            }
            mstEdgeIndex++;
            mstDrawTimer = 0.0f;
            if (mstEdgeIndex == mstEdges.size()) {
                mstDoneDrawing = true;
                currentHighlightLine = 9; // Return MST
            }
        }
    }
    else if (showWeightInfo && CheckCollisionPointRec(mousePos, skipButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!mstDoneDrawing) {
            isMSTDrawingPaused = true;
            for (int i = mstEdgeIndex; i < mstEdges.size(); ++i) {
                if (!mstEdgesDrawn[i]) {
                    mstEdgesDrawn[i] = true;
                    totalMSTWeight += mstEdges[i].weight;
                    if (mstEdges[i].from > 0 && mstEdges[i].from <= mstNodesDrawn.size())
                        mstNodesDrawn[mstEdges[i].from - 1] = true;
                    if (mstEdges[i].to > 0 && mstEdges[i].to <= mstNodesDrawn.size())
                        mstEdges[i].to = 1;
                }
            }
            mstEdgeIndex = mstEdges.size();
            mstDoneDrawing = true;
            mstDrawTimer = 0.0f;
            currentHighlightLine = 9; // Return MST
        }
    }
    
    if (showMSTMenu) {
        if (CheckCollisionPointRec(mousePos, mstMenuRect)) {
            Vector2 screenMousePos = GetMousePosition();
            clickedOnUI = true; // Interactions inside MST menu are UI
            speedSlider.update(); // Update slider logic
            // Handle slider value editing click
            Rectangle sliderValueRect = { speedSlider.rect.x + speedSlider.rect.width + 10, speedSlider.rect.y, 60, 16 };
            if (!speedSlider.isEditing() && CheckCollisionPointRec(mousePos, sliderValueRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                speedSlider.startEditing();
            }
             // Stop slider editing if clicking outside slider or pressing Enter
            if (speedSlider.isEditing() && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !speedSlider.isHovered() && !CheckCollisionPointRec(mousePos, sliderValueRect)))) {
                speedSlider.stopEditing();
            }

            if (showPseudocodeVScrollbar && !isDraggingHScrollbar) {
               if (!isDraggingVScrollbar && CheckCollisionPointRec(mousePos, pseudocodeVScrollHandleRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                   isDraggingVScrollbar = true;
                   scrollbarMouseOffsetY = mousePos.y - pseudocodeVScrollHandleRect.y;
               }
               if (showPseudocodeVScrollbar && !isDraggingHScrollbar) {
                   if (!isDraggingVScrollbar && CheckCollisionPointRec(screenMousePos, pseudocodeVScrollHandleRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                       isDraggingVScrollbar = true;
                       scrollbarMouseOffsetY = screenMousePos.y - pseudocodeVScrollHandleRect.y;
                   }
                   if (isDraggingVScrollbar && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                       float trackHeight = pseudocodeVScrollbarRect.height - pseudocodeVScrollHandleRect.height;
                       float scrollMax = mstPseudocodeTotalHeight - pseudocodeViewRect.height;
                       float newHandleY = screenMousePos.y - scrollbarMouseOffsetY;
                       newHandleY = Clamp(newHandleY, pseudocodeVScrollbarRect.y, pseudocodeVScrollbarRect.y + trackHeight);
                       float scrollRatio = scrollMax > 0 ? (newHandleY - pseudocodeVScrollbarRect.y) / trackHeight : 0.0f;
                       mstPseudocodeScrollY = scrollRatio * scrollMax;
                       mstPseudocodeScrollY = Clamp(mstPseudocodeScrollY, 0.0f, scrollMax);
                   }
                   if (isDraggingVScrollbar && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                       isDraggingVScrollbar = false;
                   }
                }
           }
            // Check Button Clicks
            if (CheckCollisionPointRec(mousePos, primButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
               if (!nodePositions.empty()) {
                   mstEdges = calculatePrimMST(edges, nodePositions.size());
                   if (mstEdges.size() < nodePositions.size() - 1 && nodePositions.size() > 1) {
                       mstErrorMessage = "MST not found (graph might be disconnected).";
                       showMSTError = true;
                       usePrim = false; useKruskal = false; mstEdges.clear();
                       currentHighlightLine = -1;
                   } else {
                       usePrim = true; useKruskal = false; showMSTError = false;
                       mstEdgesDrawn.assign(mstEdges.size(), false);
                       mstNodesDrawn.assign(nodePositions.size(), false);
                       mstEdgeIndex = 0; mstDrawTimer = 0.0f; mstDoneDrawing = false;
                       totalMSTWeight = 0; showWeightInfo = true; isMSTDrawingPaused = false;
                       currentHighlightLine = 1; // Bắt đầu với bước khởi tạo
                   }
               }
           } else if (CheckCollisionPointRec(mousePos, kruskalButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
               if (!nodePositions.empty()) {
                   mstEdges = calculateKruskalMST(edges, nodePositions.size());
                   if (mstEdges.size() < nodePositions.size() - 1 && nodePositions.size() > 1) {
                       mstErrorMessage = "MST not found (graph might be disconnected).";
                       showMSTError = true;
                       usePrim = false; useKruskal = false; mstEdges.clear();
                       currentHighlightLine = -1;
                   } else {
                       usePrim = false; useKruskal = true; showMSTError = false;
                       mstEdgesDrawn.assign(mstEdges.size(), false);
                       mstNodesDrawn.assign(nodePositions.size(), false);
                       mstEdgeIndex = 0; mstDrawTimer = 0.0f; mstDoneDrawing = false;
                       totalMSTWeight = 0; showWeightInfo = true; isMSTDrawingPaused = false;
                       currentHighlightLine = 1; // Bắt đầu với bước khởi tạo
                   }
               }
           } else if (CheckCollisionPointRec(mousePos, cancelButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                showMSTMenu = false; // Exit MST menu
                showMSTError = false; // Clear error on exit
                currentHighlightLine = -1; // Reset highlight
            } else if (showWeightInfo && CheckCollisionPointRec(mousePos, pauseResumeButton)) { // Dùng pauseResumeButton mới
               if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                   isMSTDrawingPaused = !isMSTDrawingPaused;
               }
            }
            else if (showWeightInfo && CheckCollisionPointRec(mousePos, prevStepButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
               if (mstEdgeIndex > 0) { // Chỉ lùi lại nếu có bước trước đó
                   isMSTDrawingPaused = true; // Tạm dừng khi nhấn Prev
                   mstDoneDrawing = false;   // Chắc chắn không phải đã xong
                   mstEdgeIndex--;           // Lùi chỉ số cạnh
                   mstEdgesDrawn[mstEdgeIndex] = false; // Bỏ đánh dấu cạnh này
                   totalMSTWeight -= mstEdges[mstEdgeIndex].weight; // Trừ trọng số

                   // Tính toán lại trạng thái mstNodesDrawn
                   mstNodesDrawn.assign(nodePositions.size(), false); // Reset lại
                   for (int i = 0; i < mstEdgeIndex; ++i) { // Duyệt qua các cạnh *đã* được vẽ (đến trước cạnh vừa bỏ)
                       if (mstEdgesDrawn[i]) {
                           if (mstEdges[i].from > 0 && mstEdges[i].from <= mstNodesDrawn.size())
                               mstNodesDrawn[mstEdges[i].from - 1] = true;
                           if (mstEdges[i].to > 0 && mstEdges[i].to <= mstNodesDrawn.size())
                               mstNodesDrawn[mstEdges[i].to - 1] = true;
                       }
                   }
                   // Nếu mstEdgeIndex về 0, và thuật toán Prim bắt đầu từ node 0, thì node 0 vẫn nên được đánh dấu
                   if (mstEdgeIndex == 0 && usePrim && !mstNodesDrawn.empty()) {
                       // mstNodesDrawn[0] = true; // Hoặc không cần nếu logic vẽ xử lý node đầu tiên riêng
                   }
                   currentHighlightLine = -1; 
               }
           }
           else if (showWeightInfo && CheckCollisionPointRec(mousePos, nextStepButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
               if (mstEdgeIndex < mstEdges.size()) { // Chỉ tiến tới nếu chưa hết cạnh
                   isMSTDrawingPaused = true; // Tạm dừng khi nhấn Next
                   // Thực hiện ngay bước tiếp theo
                   if (!mstEdgesDrawn[mstEdgeIndex]) { // Chỉ cộng weight nếu chưa vẽ
                       mstEdgesDrawn[mstEdgeIndex] = true;
                       totalMSTWeight += mstEdges[mstEdgeIndex].weight;
                       if (mstEdges[mstEdgeIndex].from > 0 && mstEdges[mstEdgeIndex].from <= mstNodesDrawn.size())
                           mstNodesDrawn[mstEdges[mstEdgeIndex].from - 1] = true;
                       if (mstEdges[mstEdgeIndex].to > 0 && mstEdges[mstEdgeIndex].to <= mstNodesDrawn.size())
                           mstNodesDrawn[mstEdges[mstEdgeIndex].to - 1] = true;
                   }
                   mstEdgeIndex++;
                   mstDrawTimer = 0.0f; // Reset timer
                   if (mstEdgeIndex == mstEdges.size()) {
                       mstDoneDrawing = true;
                   }
                   if (usePrim) currentHighlightLine = 5; // Index của dòng "6. Add edge..." trong primPseudocode
                   else if (useKruskal) currentHighlightLine = 5; // Index của dòng "6. Add edge..." trong kruskalPseudocode
                   else currentHighlightLine = -1;
               }
           }
           else if (showWeightInfo && CheckCollisionPointRec(mousePos, skipButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
              if (!mstDoneDrawing) {
                   isMSTDrawingPaused = true; // Tạm dừng
                   // Hoàn thành các bước còn lại
                   for (int i = mstEdgeIndex; i < mstEdges.size(); ++i) {
                       if (!mstEdgesDrawn[i]) {
                           mstEdgesDrawn[i] = true;
                           totalMSTWeight += mstEdges[i].weight;
                           if (mstEdges[i].from > 0 && mstEdges[i].from <= mstNodesDrawn.size())
                               mstNodesDrawn[mstEdges[i].from - 1] = true;
                           if (mstEdges[i].to > 0 && mstEdges[i].to <= mstNodesDrawn.size())
                               mstNodesDrawn[mstEdges[i].to - 1] = true;
                       }
                   }
                   mstEdgeIndex = mstEdges.size(); // Đi đến cuối
                   mstDoneDrawing = true;
                   mstDrawTimer = 0.0f;
                   if (!mstEdges.empty()){ // Chỉ highlight nếu có cạnh nào được thêm
                       if (usePrim) currentHighlightLine = 5;
                       else if (useKruskal) currentHighlightLine = 5;
                       else currentHighlightLine = -1;
                   } else {
                       currentHighlightLine = -1; // Hoặc highlight dòng cuối (return)
                   }
               }
           }
        }
         // Allow clicking outside MST menu to close it? Maybe not, use Back button.
         else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showMSTMenu = false; // Click outside closes
         }
    } 

     if (showMSTError) {
        DrawText(mstErrorMessage.c_str(), mstMenuRect.x + 10, mstButtonsY - 25, 18, RED); // Position above buttons
    }

    // Draw General/File Error Messages (if any) - Draw last over UI elements
     if (showError) {
         DrawText(errorMessage.c_str(), 130, screenHeight - 70, 20, RED); // Position near bottom-left
     }
     if (showFileError) {
          DrawText(fileErrorMessage.c_str(), 130, screenHeight - 90, 20, RED); // Position near bottom-left
     }
}

void GraphMain::DrawGraph()
{
    // --- Draw Graph Area ---
    if (graphDrawn && !showMSTMenu) { // Draw main graph if not in MST menu
        BeginMode2D(graphCamera);

        // Draw edges first
        for (size_t i = 0; i < edges.size(); ++i) {
            const auto& edge = edges[i];
            int fromIdx = edge.from - 1;
            int toIdx = edge.to - 1;

            if (fromIdx >= 0 && fromIdx < nodePositions.size() && toIdx >= 0 && toIdx < nodePositions.size()) {
                Vector2 startPos = nodePositions[fromIdx];
                Vector2 endPos = nodePositions[toIdx];
                Color edgeColor = DARKBLUE;
                float thickness = 2.0f / graphCamera.zoom; // Adjust thickness based on zoom

                // Highlight for editing
                if (isEditingGraph) {
                     if (currentTool == EditTool::TOOL_EDIT_WEIGHT && (int)i == selectedEdgeIndex) {
                         edgeColor = YELLOW; thickness *= 2.0f;
                     }
                     if (currentTool == EditTool::TOOL_DELETE_EDGE && CheckCollisionPointLine(worldMousePos, startPos, endPos, 15.0f/graphCamera.zoom)) {
                         edgeColor = RED; // Highlight edge under cursor when delete tool active
                     }
                }

                drawBezierEdge(startPos, endPos, thickness, edgeColor);
                drawEdgeWeight(startPos, endPos, edge.weight);
            }
        }

        // Draw nodes on top
        for (size_t i = 0; i < nodePositions.size(); ++i) {
            Color nodeColor = ORANGE;
            // Highlight node under cursor for delete tool
            if (isEditingGraph && currentTool == EditTool::TOOL_DELETE_VERTEX && CheckCollisionPointCircle(worldMousePos, nodePositions[i], 20.0f)) {
                nodeColor = RED;
            }
            drawNode(nodePositions[i], i + 1, nodeColor, 20.0f);
        }

        // Draw editing highlights/previews
        if (isEditingGraph) {
            // Highlight selected node (for move/add edge start)
             if (selectedNodeIndex != -1 && selectedNodeIndex < nodePositions.size()) {
                  DrawCircleLines(nodePositions[selectedNodeIndex].x, nodePositions[selectedNodeIndex].y, 22.0f / graphCamera.zoom, YELLOW);
             }
             // Draw line preview when adding edge
             if (currentTool == EditTool::TOOL_ADD_EDGE_END && selectedNodeIndex != -1) {
                  DrawLineEx(nodePositions[selectedNodeIndex], worldMousePos, 2.0f / graphCamera.zoom, YELLOW);
             }
            // Draw vertex add preview
             if (currentTool == EditTool::TOOL_ADD_VERTEX) {
                  // Check if mouse is in graph area (not over UI)
                  float leftUI = 120.0f; float rightUI = editPanelWidth;
                  if (mousePos.x > leftUI && mousePos.x < (screenWidth - rightUI)) {
                      bool overlaps = false;
                      for(const auto& pos : nodePositions) if(CheckCollisionPointCircle(worldMousePos, pos, 20.0f)) overlaps = true;
                      if (!overlaps) {
                          DrawCircleV(worldMousePos, 20.0f, Fade(SKYBLUE, 0.5f));
                          DrawCircleLines(worldMousePos.x, worldMousePos.y, 20.0f, Fade(BLUE, 0.6f));
                      }
                  }
             }
        }
       EndMode2D();
    } // End drawing main graph 
}

void GraphMain::DrawUI()
{
    // --- CHUẨN BỊ STATE ĐỂ VẼ UI ---
    UIState currentState = {}; // Khởi tạo struct

    // Điền dữ liệu từ các biến của RunGraphApp vào currentState
    // Flags
    currentState.isCreating = isCreating;
    currentState.isShowingExamples = showExampleButtons; // Hoặc isShowingExamples nếu có biến riêng
    currentState.isShowingExamplesActive = isShowingExamplesActive;
    currentState.isFileActive = isFileActive;
    currentState.isEditingGraph = isEditingGraph;
    currentState.showMSTMenu = showMSTMenu;
    currentState.showWeightInfo = showWeightInfo;
    currentState.showError = showError;
    currentState.showFileError = showFileError;
    currentState.nodesFocused = nodesFocused;
    currentState.edgesFocused = edgesFocused;
    currentState.isEditingWeight = isEditingWeight;
    currentState.isDraggingVScrollbar = isDraggingVScrollbar;
    currentState.isDraggingHScrollbar = isDraggingHScrollbar;


    // Data
    currentState.numNodesStr = numNodesStr;
    currentState.numEdgesStr = numEdgesStr;
    currentState.weightInputBuffer = weightInputBuffer;
    currentState.errorMessage = errorMessage;
    currentState.fileErrorMessage = fileErrorMessage;
    currentState.currentHighlightLine = currentHighlightLine;
    currentState.currentTool = currentTool;
    currentState.selectedNodeIndex = selectedNodeIndex;
    currentState.selectedEdgeIndex = selectedEdgeIndex;
    // Rectangles
    currentState.createButton = createButton; currentState.randomButton = randomButton;
    currentState.exampleButton = exampleButton; currentState.fileButton = fileButton;
    currentState.editButton = editButton; currentState.mstButton = mstButton;
    currentState.nodesInputRect = nodesInputRect; currentState.edgesInputRect = edgesInputRect;
    currentState.k5Button = k5Button; currentState.c6Button = c6Button;
    currentState.p4Button = p4Button; currentState.s7Button = s7Button; 
    currentState.kruskalButton = kruskalButton; currentState.cancelButton = cancelButton;
    currentState.prevStepButton = prevStepButton; currentState.nextStepButton = nextStepButton;
    currentState.skipButton = skipButton; currentState.pauseResumeButton = pauseResumeButton;
    currentState.addVertexButtonRect = addVertexButtonRect; currentState.addEdgeButtonRect = addEdgeButtonRect;
    currentState.editWeightButtonRect = editWeightButtonRect; currentState.moveNodeButtonRect = moveNodeButtonRect;
    currentState.deleteVertexButtonRect = deleteVertexButtonRect; currentState.deleteEdgeButtonRect = deleteEdgeButtonRect;
    currentState.doneButtonRect = doneButtonRect; currentState.weightInputRect = weightInputRect;
    currentState.nodesQuestionButton = nodesQuestionButton;
    currentState.edgesQuestionButton = edgesQuestionButton;
    currentState.okButton = okButton; // Add OK button to UIState
    // Rect cho scrollbar/view được tính toán lại trong hàm vẽ, nhưng ta cần truyền các rect handle/track đã tính ở phần input
    currentState.pseudocodeVScrollbarRect = pseudocodeVScrollbarRect;
    currentState.pseudocodeVScrollHandleRect = pseudocodeVScrollHandleRect;
    currentState.pseudocodeHScrollbarRect = pseudocodeHScrollbarRect;
    currentState.pseudocodeHScrollHandleRect = pseudocodeHScrollHandleRect;
    // currentState.pseudocodeViewRect = pseudocodeViewRect; // Tính lại trong DrawMSTMenuUI
    DrawMainControlsUI(currentState);
    // Draw Create Input Fields (if active)
    if (currentState.isCreating) DrawCreateModeUI(currentState);
    if (currentState.isShowingExamples) DrawExampleButtonsUI(currentState); // Dùng isShowingExamples từ state
    if (currentState.isEditingGraph) DrawEditPanelUI(currentState);
    if (showMSTMenu) {
        DrawRectangleRec(mstMenuRect, LIGHTGRAY);
        DrawRectangleLinesEx(mstMenuRect, 2, BLACK);
        DrawText("Minimum Spanning Tree", mstMenuRect.x + 10, mstMenuRect.y + 10, 20, BLACK);
       
        float titleHeight = 30;
        float bottomButtonHeight = mstButtonHeight + 10;
        float availableHeight = mstMenuRect.height - titleHeight - bottomButtonHeight - 20;
        float controlPanelWidth = 220;
        float panelPadding = 10;

        Rectangle graphArea = {
            mstMenuRect.x + panelPadding,
            mstMenuRect.y + titleHeight + panelPadding,
            mstMenuRect.width - controlPanelWidth - 3 * panelPadding,
            availableHeight
        };
        DrawRectangleLinesEx(graphArea, 1, BLUE); // Visualize graph area
        
        Rectangle controlPanelRect = {
            graphArea.x + graphArea.width + panelPadding,
            graphArea.y,
            controlPanelWidth,
            availableHeight
        };
        float currentControlY = controlPanelRect.y + 10;
        int controlFontSize = 20;
        float controlItemSpacing = 15;
        const float controlButtonHeight = 30.0f;
        DrawRectangleRec(controlPanelRect, { 200, 200, 200, 150 }); // Optional background for control panel
        DrawRectangleLinesEx(controlPanelRect, 1, RED); // Visualize control panel area

        if (showWeightInfo) {
            std::string weightStr = "Total Weight: " + std::to_string(totalMSTWeight);
            Vector2 weightTextSize = MeasureTextEx(GetFontDefault(), weightStr.c_str(), controlFontSize, 1);
            float weightBoxWidth = controlPanelRect.width - 2 * panelPadding; // Box chiếm hết chiều rộng panel (trừ padding)
            float weightBoxHeight = weightTextSize.y + 10;
            Rectangle weightBox = {controlPanelRect.x + panelPadding, currentControlY, weightBoxWidth, weightBoxHeight};

            DrawRectangleRec(weightBox, WHITE);
            DrawRectangleLinesEx(weightBox, 1, BLACK);
            // Căn giữa chữ trong Box
            DrawText(weightStr.c_str(),
                     weightBox.x + (weightBoxWidth - weightTextSize.x) / 2,
                     weightBox.y + 5,
                     controlFontSize, BLACK);
            currentControlY += weightBoxHeight + controlItemSpacing;
        }

        speedSlider.rect.x = controlPanelRect.x + panelPadding;
        speedSlider.rect.y = currentControlY;
        speedSlider.rect.width = controlPanelRect.width - 2 * panelPadding;
         // Cập nhật lại knob và vẽ (có thể cần sửa Slider::draw để label ở trên)
        speedSlider.knob.x = Clamp(speedSlider.rect.x + (speedSlider.rect.width - speedSlider.knob.width) * speedSlider.value, speedSlider.rect.x, speedSlider.rect.x + speedSlider.rect.width - speedSlider.knob.width);
        speedSlider.knob.y = speedSlider.rect.y - 5;
        speedSlider.draw(); // Vẽ slider
        currentControlY += speedSlider.rect.height + 30; // Tăng Y đủ cho cả label slider

        // 3. Các nút điều khiển (Pause/Resume, Prev, Next, Skip) - Xếp dọc
         if (showWeightInfo) {
             float stepCtrlButtonWidth = controlPanelRect.width - 2*panelPadding; // Các nút này chiếm hết chiều rộng panel
             float buttonInternalSpacing = 8.0f;

             // Pause/Resume
             const char* pauseText = isMSTDrawingPaused ? "Resume" : "Pause";
             pauseResumeButton = {controlPanelRect.x + panelPadding, currentControlY, stepCtrlButtonWidth, controlButtonHeight};
             DrawRectangleRec(pauseResumeButton, WHITE); DrawRectangleLinesEx(pauseResumeButton, 1, CheckCollisionPointRec(mousePos, pauseResumeButton)?RED:BLACK);
             Vector2 pauseTextSize = MeasureTextEx(GetFontDefault(), pauseText, controlFontSize, 1);
             DrawText(pauseText, pauseResumeButton.x + (pauseResumeButton.width - pauseTextSize.x)/2, pauseResumeButton.y + (pauseResumeButton.height - pauseTextSize.y)/2, controlFontSize, BLACK);
             currentControlY += controlButtonHeight + buttonInternalSpacing;

             // Prev
             prevStepButton = {controlPanelRect.x + panelPadding, currentControlY, stepCtrlButtonWidth, controlButtonHeight};
             DrawRectangleRec(prevStepButton, WHITE); DrawRectangleLinesEx(prevStepButton, 1, CheckCollisionPointRec(mousePos, prevStepButton)?RED:BLACK);
             Vector2 prevTextSize = MeasureTextEx(GetFontDefault(), "Prev", controlFontSize, 1);
             DrawText("Prev", prevStepButton.x + (prevStepButton.width - prevTextSize.x)/2, prevStepButton.y + (prevStepButton.height - prevTextSize.y)/2, controlFontSize, (mstEdgeIndex > 0) ? BLACK : GRAY);
             currentControlY += controlButtonHeight + buttonInternalSpacing;

             // Next
             nextStepButton = {controlPanelRect.x + panelPadding, currentControlY, stepCtrlButtonWidth, controlButtonHeight};
             DrawRectangleRec(nextStepButton, WHITE); DrawRectangleLinesEx(nextStepButton, 1, CheckCollisionPointRec(mousePos, nextStepButton)?RED:BLACK);
             Vector2 nextTextSize = MeasureTextEx(GetFontDefault(), "Next", controlFontSize, 1);
             DrawText("Next", nextStepButton.x + (nextStepButton.width - nextTextSize.x)/2, nextStepButton.y + (nextStepButton.height - nextTextSize.y)/2, controlFontSize, (mstEdgeIndex < mstEdges.size()) ? BLACK : GRAY);
             currentControlY += controlButtonHeight + buttonInternalSpacing;

             // Skip
             skipButton = {controlPanelRect.x + panelPadding, currentControlY, stepCtrlButtonWidth, controlButtonHeight};
             DrawRectangleRec(skipButton, WHITE); DrawRectangleLinesEx(skipButton, 1, CheckCollisionPointRec(mousePos, skipButton)?RED:BLACK);
             Vector2 skipTextSize = MeasureTextEx(GetFontDefault(), "Skip", controlFontSize, 1);
             DrawText("Skip", skipButton.x + (skipButton.width - skipTextSize.x)/2, skipButton.y + (skipButton.height - skipTextSize.y)/2, controlFontSize, mstDoneDrawing ? GRAY : BLACK);
             currentControlY += controlButtonHeight + controlItemSpacing; // Khoảng cách cuối cùng
         }
       // Tính toán pseudocodeViewRect
         pseudocodeViewRect = {
           controlPanelRect.x + panelPadding,
           currentControlY,
           controlPanelRect.width - 2 * panelPadding - 12,
           controlPanelRect.y + controlPanelRect.height - currentControlY - panelPadding - 12
         };
         DrawRectangle(pseudocodeViewRect.x - 2, pseudocodeViewRect.y - 2, pseudocodeViewRect.width + 4, pseudocodeViewRect.height + 4, Fade(BLACK, 0.1f)); // Nền mờ nhẹ (tùy chọn)
         DrawRectangleLinesEx(pseudocodeViewRect, 1, GRAY); // Viền (tùy chọn)

         int numNodesToDraw = nodePositions.size();
         std::vector<Vector2> mstDrawNodePositions(numNodesToDraw);
         if (numNodesToDraw > 0 && mstDrawNodePositions.size() == numNodesToDraw) {
             // Logic tính scale/offset DỰA TRÊN graphArea mới
             Vector2 minBoundsOrig = nodePositions[0], maxBoundsOrig = nodePositions[0];
             for(const auto& p : nodePositions) {
                 minBoundsOrig.x = std::min(minBoundsOrig.x, p.x); minBoundsOrig.y = std::min(minBoundsOrig.y, p.y);
                 maxBoundsOrig.x = std::max(maxBoundsOrig.x, p.x); maxBoundsOrig.y = std::max(maxBoundsOrig.y, p.y);
             }
             float graphWidthOrig = std::max(1.0f, maxBoundsOrig.x - minBoundsOrig.x);
             float graphHeightOrig = std::max(1.0f, maxBoundsOrig.y - minBoundsOrig.y);
             Vector2 graphCenterOrig = { minBoundsOrig.x + graphWidthOrig / 2.0f, minBoundsOrig.y + graphHeightOrig / 2.0f };
             float scaleX = (graphArea.width - 40) / graphWidthOrig; float scaleY = (graphArea.height - 40) / graphHeightOrig;
             float scale = std::min(scaleX, scaleY) * 0.9f; scale = std::max(0.01f, scale);
             Vector2 graphAreaCenter = { graphArea.x + graphArea.width / 2.0f, graphArea.y + graphArea.height / 2.0f };
             Vector2 offset = { graphAreaCenter.x - graphCenterOrig.x * scale, graphAreaCenter.y - graphCenterOrig.y * scale };
             for(int i=0; i < numNodesToDraw; ++i) { mstDrawNodePositions[i] = { offset.x + nodePositions[i].x * scale, offset.y + nodePositions[i].y * scale }; }

             // Vẽ Cạnh
             if (usePrim || useKruskal || mstDoneDrawing) { // Draw faded edges once an algorithm runs or finishes
                for (const auto& edge : edges) {
                    bool isMSTEdge = false;
                    for(size_t i = 0; i < mstEdgesDrawn.size(); ++i) { // Check against drawn MST edges
                        if (mstEdgesDrawn[i]) {
                             const auto& mstEdge = mstEdges[i];
                            if ((edge.from == mstEdge.from && edge.to == mstEdge.to) || (edge.from == mstEdge.to && edge.to == mstEdge.from)) {
                                isMSTEdge = true;
                                break;
                            }
                        }
                    }
                     // Also check edge being currently drawn if animation is running
                     if (!mstDoneDrawing && mstEdgeIndex > 0 && mstEdgeIndex <= mstEdges.size()) {
                         const auto& currentMSTEdge = mstEdges[mstEdgeIndex - 1];
                         if ((edge.from == currentMSTEdge.from && edge.to == currentMSTEdge.to) || (edge.from == currentMSTEdge.to && edge.to == currentMSTEdge.from)) {
                               // If the edge is the one being drawn *right now*, draw it highlighted below
                         }
                     }


                    if (!isMSTEdge) { // Only draw if it's not a confirmed MST edge
                        int fromIdx = edge.from - 1;
                        int toIdx = edge.to - 1;
                        if (fromIdx >= 0 && fromIdx < numNodesToDraw && toIdx >= 0 && toIdx < numNodesToDraw) {
                            drawBezierEdge(mstDrawNodePositions[fromIdx], mstDrawNodePositions[toIdx], 1.0f, Fade(DARKBLUE, 0.2f)); // Faded
                             // Optionally draw faded weight? Maybe too cluttered.
                             // drawEdgeWeight(mstDrawNodePositions[fromIdx], mstDrawNodePositions[toIdx], edge.weight); // Faded?
                        }
                    }
                }
            }
             for (size_t i = 0; i < mstEdgesDrawn.size(); ++i) {
                 if (mstEdgesDrawn[i]) {
                      const auto& mstEdge = mstEdges[i];
                      int fromIdx = mstEdge.from - 1;
                      int toIdx = mstEdge.to - 1;
                      if (fromIdx >= 0 && fromIdx < numNodesToDraw && toIdx >= 0 && toIdx < numNodesToDraw) {
                      drawBezierEdge(mstDrawNodePositions[fromIdx], mstDrawNodePositions[toIdx], 2.0f, RED); // Highlighted MST edge
                      drawEdgeWeight(mstDrawNodePositions[fromIdx], mstDrawNodePositions[toIdx], mstEdge.weight);
                    }
                }
             }
             // Vẽ Nút
             for(int i=0; i < numNodesToDraw; ++i) {
                Color nodeColor = (mstNodesDrawn.empty() || !mstNodesDrawn[i]) ? LIGHTGRAY : ORANGE; // Gray if not yet included
                if (!mstDoneDrawing && mstEdgeIndex > 0 && mstEdgeIndex <= mstEdges.size()) {
                const auto& currentMSTEdge = mstEdges[mstEdgeIndex - 1];
                if (i == currentMSTEdge.from - 1 || i == currentMSTEdge.to - 1) {
                        nodeColor = SKYBLUE; // Highlight nodes of edge being drawn
                    }
                   }
                 drawNode(mstDrawNodePositions[i], i + 1, nodeColor, 15.0f); // Smaller nodes in MST view
                }
         }
         DrawRectangleRec(primButton, usePrim ? ORANGE: WHITE); DrawButton("Prim", primButton, GetFontDefault(), "Prim"); 
         DrawRectangleRec(kruskalButton, useKruskal ? ORANGE : WHITE);  DrawButton("Kruskal", kruskalButton, GetFontDefault(), "Kruskal"); 
         DrawRectangleRec(cancelButton, WHITE); DrawButton("Cancel", cancelButton, GetFontDefault(), "Cancel");
        std::vector<std::string>* codeToShow = nullptr;
        if (usePrim) codeToShow = &primPseudocode;
        else if (useKruskal) codeToShow = &kruskalPseudocode;
        
        if (codeToShow) {
            int pseudoFontSize = 16;
            float pseudoLineHeight = (float)pseudoFontSize * 1.25f;

            // Tính toán kích thước nội dung
            mstPseudocodeTotalHeight = codeToShow->size() * pseudoLineHeight;
            mstPseudocodeMaxWidth = 0.0f;
            for (const auto& line : *codeToShow) {
                Vector2 textSize = MeasureTextEx(GetFontDefault(), line.c_str(), pseudoFontSize, 1);
                mstPseudocodeMaxWidth = std::max(mstPseudocodeMaxWidth, textSize.x + 10.0f);
            }

            // Vẽ pseudocode với scroll
            BeginScissorMode((int)pseudocodeViewRect.x, (int)pseudocodeViewRect.y, (int)pseudocodeViewRect.width, (int)pseudocodeViewRect.height);
            DrawRectangleRec(pseudocodeViewRect, Fade(BLACK, 0.1f));
            float textOffsetY = pseudocodeViewRect.y + 5 - mstPseudocodeScrollY;
            for (int i = 0; i < codeToShow->size(); ++i) {
                if (textOffsetY >= pseudocodeViewRect.y - pseudoLineHeight && textOffsetY < pseudocodeViewRect.y + pseudocodeViewRect.height) {
                    Color textColor = BLACK;
                    if (i == currentHighlightLine) {
                        DrawRectangle(pseudocodeViewRect.x - mstPseudocodeScrollX, textOffsetY - 2, mstPseudocodeMaxWidth, pseudoLineHeight, Fade(YELLOW, 0.5f));
                        textColor = DARKBLUE;
                    }
                    DrawText((*codeToShow)[i].c_str(), pseudocodeViewRect.x + 5 - mstPseudocodeScrollX, textOffsetY, pseudoFontSize, textColor);
                }
                textOffsetY += pseudoLineHeight;
            }
            EndScissorMode();

            // Thanh cuộn ngang
            showPseudocodeHScrollbar = mstPseudocodeMaxWidth > pseudocodeViewRect.width;
            if (showPseudocodeHScrollbar) {
                float viewRatio = pseudocodeViewRect.width / mstPseudocodeMaxWidth;
                float handleWidth = pseudocodeViewRect.width * viewRatio;
                handleWidth = std::max(handleWidth, 20.0f);
                float trackWidth = pseudocodeViewRect.width;
                float scrollMax = mstPseudocodeMaxWidth - pseudocodeViewRect.width;
                float handleX = pseudocodeViewRect.x + (scrollMax > 0 ? (mstPseudocodeScrollX / scrollMax) * (trackWidth - handleWidth) : 0);

                pseudocodeHScrollbarRect = { pseudocodeViewRect.x, pseudocodeViewRect.y + pseudocodeViewRect.height, trackWidth, 12 };
                pseudocodeHScrollHandleRect = { handleX, pseudocodeViewRect.y + pseudocodeViewRect.height, handleWidth, 12 };

                DrawRectangleRec(pseudocodeHScrollbarRect, LIGHTGRAY);
                DrawRectangleRec(pseudocodeHScrollHandleRect, isDraggingHScrollbar ? DARKGRAY : GRAY);
            } else {
                mstPseudocodeScrollX = 0.0f;
            }
        }
        if (CheckCollisionPointRec(mousePos, mstMenuRect)) {
            Vector2 screenMousePos = GetMousePosition();
            clickedOnUI = true;
            speedSlider.update();

            // Handle slider value editing click
            Rectangle sliderValueRect = { speedSlider.rect.x + speedSlider.rect.width + 10, speedSlider.rect.y, 60, 16 };
            if (!speedSlider.isEditing() && CheckCollisionPointRec(mousePos, sliderValueRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                speedSlider.startEditing();
            }
            if (speedSlider.isEditing() && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !speedSlider.isHovered() && !CheckCollisionPointRec(mousePos, sliderValueRect)))) {
                speedSlider.stopEditing();
            }

            // Xử lý kéo thanh cuộn dọc
            if (showPseudocodeVScrollbar && !isDraggingHScrollbar) {
                if (!isDraggingVScrollbar && CheckCollisionPointRec(screenMousePos, pseudocodeVScrollHandleRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    isDraggingVScrollbar = true;
                    scrollbarMouseOffsetY = screenMousePos.y - pseudocodeVScrollHandleRect.y;
                }
                if (isDraggingVScrollbar && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float trackHeight = std::max(1.0f, pseudocodeVScrollbarRect.height - pseudocodeVScrollHandleRect.height);
                    float scrollMax = std::max(0.0f, mstPseudocodeTotalHeight - pseudocodeViewRect.height);
                    float newHandleY = screenMousePos.y - scrollbarMouseOffsetY;
                    newHandleY = Clamp(newHandleY, pseudocodeVScrollbarRect.y, pseudocodeVScrollbarRect.y + trackHeight);
                    float scrollRatio = trackHeight > 0 ? (newHandleY - pseudocodeVScrollbarRect.y) / trackHeight : 0.0f;
                    mstPseudocodeScrollY = scrollRatio * scrollMax;
                    mstPseudocodeScrollY = Clamp(mstPseudocodeScrollY, 0.0f, scrollMax);
                }
                if (isDraggingVScrollbar && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    isDraggingVScrollbar = false;
                }
            }

            // Xử lý kéo thanh cuộn ngang
            if (showPseudocodeHScrollbar && !isDraggingVScrollbar) {
                if (!isDraggingHScrollbar && CheckCollisionPointRec(screenMousePos, pseudocodeHScrollHandleRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    isDraggingHScrollbar = true;
                    scrollbarMouseOffsetX = screenMousePos.x - pseudocodeHScrollHandleRect.x;
                }
                if (isDraggingHScrollbar && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float trackWidth = std::max(1.0f, pseudocodeHScrollbarRect.width - pseudocodeHScrollHandleRect.width);
                    float scrollMax = std::max(0.0f, mstPseudocodeMaxWidth - pseudocodeViewRect.width); // Sửa lỗi từ height thành width
                    float newHandleX = screenMousePos.x - scrollbarMouseOffsetX;
                    newHandleX = Clamp(newHandleX, pseudocodeHScrollbarRect.x, pseudocodeHScrollbarRect.x + trackWidth);
                    float scrollRatio = trackWidth > 0 ? (newHandleX - pseudocodeHScrollbarRect.x) / trackWidth : 0.0f;
                    mstPseudocodeScrollX = scrollRatio * scrollMax;
                    mstPseudocodeScrollX = Clamp(mstPseudocodeScrollX, 0.0f, scrollMax);
                }
                if (isDraggingHScrollbar && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    isDraggingHScrollbar = false;
                }
            }
        }
     }

}
bool GraphMain::ShouldClose() {
    return shouldClose || WindowShouldClose();
}
