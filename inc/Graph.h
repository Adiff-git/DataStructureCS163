#ifndef GRAPHAPP_H
#define GRAPHAPP_H

#include "raylib.h"
#include "raymath.h" // <- Cần include raymath ở đây
#include "tinyfiledialogs.h"
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <utility> // For std::pair


// --- Structs and Enums ---

struct Edge {
    int from;
    int to;
    int weight;
    bool operator<(const Edge& other) const;
};

enum class EditTool {
    TOOL_NONE, TOOL_ADD_VERTEX, TOOL_ADD_EDGE_START, TOOL_ADD_EDGE_END,
    TOOL_EDIT_WEIGHT, TOOL_MOVE_VERTEX, TOOL_DELETE_VERTEX, TOOL_DELETE_EDGE
};

// --- Class Declarations ---

class Slider {
public:
    Rectangle rect; Rectangle knob; float value; float minValue; float maxValue;
    bool isDragging; std::string label; bool editValue; std::string editBuffer;

    Slider(Rectangle r, float minVal, float maxVal, std::string l);
    void update();
    float getValue() const;
    void draw();
    bool isHovered() const;
    bool isEditing() const;
    void startEditing();
    void stopEditing();
    std::string getLabel() const;
    void setValue(float actualValue);
};

// --- Function Prototypes ---

// Graph Utils
bool edgeExists(const std::vector<Edge>& edges, int from, int to);
using CompareEdgePair = std::function<bool(const std::pair<int, Edge>&, const std::pair<int, Edge>&)>;
bool compareEdgePrim(const std::pair<int, Edge>& a, const std::pair<int, Edge>& b);
bool isGraphConnected(const std::vector<Edge>& edges, int numNodes);

// MST Algorithms
std::vector<Edge> calculatePrimMST(const std::vector<Edge>& edges, int numNodes);
int findSet(std::vector<int>& parent, int i); // Helper for Kruskal
void unionSets(std::vector<int>& parent, int a, int b); // Helper for Kruskal
std::vector<Edge> calculateKruskalMST(const std::vector<Edge>& edges, int numNodes);

// Graph Generators
void generateCompleteGraph(std::vector<Edge>& edges, int numNodes);
void generateCycleGraph(std::vector<Edge>& edges, int numNodes);
void generatePathGraph(std::vector<Edge>& edges, int numNodes, std::vector<Vector2>& nodePositions, int screenWidth, int screenHeight);
void generateStarGraph(std::vector<Edge>& edges, int numNodes);
void generateRandomGraph(std::vector<Edge>& edges, int numNodes, int numEdges);

// Drawing Utils
void positionNodesInCircle(std::vector<Vector2>& nodePositions, int numNodes, float centerX, float centerY, float radius);
void drawNode(Vector2 position, int id, Color color, float radius);
void drawBezierEdge(Vector2 start, Vector2 end, float thickness, Color color);
void drawEdgeWeight(Vector2 start, Vector2 end, int weight);
// drawMSTInfo bị loại bỏ, vẽ trực tiếp trong RunGraphApp

// Camera Utils
void UpdateGraphCamera(Camera2D &camera, const std::vector<Vector2>& nodes, float nodeRadius, int screenWidth, int screenHeight, float uiLeftWidth, float uiRightWidth);

// Input/Text Utils
int GetCursorIndex(const std::string& text, int row, int col);
int GetColFromIndex(const std::string& text, int index);
Vector2 GetCursorScreenPos(const std::string& text, int row, int col, Rectangle rect, int fontSize = 20);
std::string ValidateMatrixInput(const std::string& input, std::vector<std::vector<int>>& adjacencyMatrix, int& numNodes);

// File Utils
std::string ReadTextFile(const char *filePath);
bool LoadGraphFromFile(const char* filePath, std::vector<Edge>& outEdges, std::vector<Vector2>& outNodePositions, int screenWidth, int screenHeight, std::string& outErrorMessage);

// --- Main Application Function Prototype ---
void RunGraphApp();

#endif // GRAPHAPP_H