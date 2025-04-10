#include "raylib.h"
#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include "raymath.h"
#include <random>
#include <iomanip>
#include "tinyfiledialogs.h"
#include <fstream>
#include <functional>
#include <stdexcept>
#include <utility> // For std::pair

struct Edge {
    int from;
    int to;
    int weight;

    // Used by std::sort
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

bool edgeExists(const std::vector<Edge>& edges, int from, int to) {
    for (const auto& edge : edges) {
        if ((edge.from == from && edge.to == to) || (edge.from == to && edge.to == from)) {
            return true;
        }
    }
    return false;
}

using CompareEdgePair = std::function<bool(const std::pair<int, Edge>&, const std::pair<int, Edge>&)>;

bool compareEdgePrim(const std::pair<int, Edge>& a, const std::pair<int, Edge>& b) {
    return a.first > b.first; // Min-heap based on weight (a.first)
}

bool isGraphConnected(const std::vector<Edge>& edges, int numNodes) {
    if (numNodes <= 1) return true;

    std::vector<bool> visited(numNodes, false);
    std::vector<int> stack;

    // Use node 0 (index) as the starting point
    if (numNodes > 0) {
        stack.push_back(0);
        visited[0] = true;
    } else {
        return true; // Empty graph is trivially connected (or handle as error)
    }


    int visitedCount = 0; // Count visited nodes

    while (!stack.empty()) {
        int currentNodeIndex = stack.back();
        stack.pop_back();
        visitedCount++;

        // Iterate through edges to find neighbors
        for (const auto& edge : edges) {
            int neighborIndex = -1;
            // Adjust node IDs (1-based) to 0-based indices
            int fromIndex = edge.from - 1;
            int toIndex = edge.to - 1;

            // Check bounds before accessing visited array
            if (fromIndex == currentNodeIndex && toIndex >= 0 && toIndex < numNodes && !visited[toIndex]) {
                neighborIndex = toIndex;
            } else if (toIndex == currentNodeIndex && fromIndex >= 0 && fromIndex < numNodes && !visited[fromIndex]) {
                neighborIndex = fromIndex;
            }

            if (neighborIndex != -1) {
                 visited[neighborIndex] = true;
                 stack.push_back(neighborIndex);
            }
        }
         // Optimization: If a node is popped, mark it visited and increment count
        // Check if all nodes are reachable ONLY after the loop finishes
    }

     // Check if all nodes were visited
    for (int i = 0; i < numNodes; ++i) {
        if (!visited[i]) {
             return false; // Found an unvisited node
        }
    }


    return true; // All nodes are visited
}

std::vector<Edge> calculatePrimMST(const std::vector<Edge>& edges, int numNodes) {
    std::vector<Edge> mstEdges;
   if (numNodes <= 0) return mstEdges; // Handle empty or invalid graph

   std::vector<bool> visited(numNodes, false);
   // Use the function pointer type defined in Graph.h
   std::priority_queue<std::pair<int, Edge>, std::vector<std::pair<int, Edge>>, CompareEdgePair> pq(compareEdgePrim);


   // Start from node 0 (index)
   visited[0] = true;
   int nodesInMST = 1; // Count nodes added to MST

   // Add initial edges connected to the starting node (0)
   for (const auto& edge : edges) {
        // Adjust 1-based IDs to 0-based indices
       int fromIdx = edge.from - 1;
       int toIdx = edge.to - 1;

        // Check validity of indices before accessing 'visited'
       if (fromIdx >= 0 && fromIdx < numNodes && toIdx >= 0 && toIdx < numNodes) {
           if (fromIdx == 0 && !visited[toIdx]) {
                pq.push({edge.weight, edge});
           } else if (toIdx == 0 && !visited[fromIdx]) {
                pq.push({edge.weight, edge});
           }
       }
   }

    while (!pq.empty() && nodesInMST < numNodes) { // Loop until MST is complete or PQ is empty
        int weight = pq.top().first;
        Edge currentEdge = pq.top().second;
        pq.pop();

       // Adjust 1-based IDs to 0-based indices
       int fromIdx = currentEdge.from - 1;
       int toIdx = currentEdge.to - 1;

        // Check validity of indices
       if (fromIdx < 0 || fromIdx >= numNodes || toIdx < 0 || toIdx >= numNodes) {
            continue; // Skip invalid edge data
       }


       // Determine which node is newly added to the visited set
       int nextNodeIdx = -1;
       if (visited[fromIdx] && !visited[toIdx]) {
           nextNodeIdx = toIdx;
       } else if (!visited[fromIdx] && visited[toIdx]) {
            nextNodeIdx = fromIdx;
       } else {
           continue; // Both visited or both unvisited (shouldn't happen with start logic)
       }


       visited[nextNodeIdx] = true;
       mstEdges.push_back(currentEdge);
       nodesInMST++;


       // Add new edges from the newly added node
        for (const auto& edge : edges) {
           // Adjust 1-based IDs to 0-based indices
           int edgeFromIdx = edge.from - 1;
           int edgeToIdx = edge.to - 1;

           // Check index validity
           if (edgeFromIdx < 0 || edgeFromIdx >= numNodes || edgeToIdx < 0 || edgeToIdx >= numNodes) {
                continue;
           }

            if (edgeFromIdx == nextNodeIdx && !visited[edgeToIdx]) {
                pq.push({edge.weight, edge});
           } else if (edgeToIdx == nextNodeIdx && !visited[edgeFromIdx]) {
                pq.push({edge.weight, edge});
           }
       }
   }
   // Optional: Check if nodesInMST == numNodes to verify MST could be formed (graph was connected)
   return mstEdges;
}

int findSet(std::vector<int>& parent, int i) {
    if (parent[i] == i)
        return i;
    // Path compression
    return parent[i] = findSet(parent, parent[i]);
}

void unionSets(std::vector<int>& parent, int a, int b) {
    int rootA = findSet(parent, a);
    int rootB = findSet(parent, b);
    if (rootA != rootB)
        parent[rootA] = rootB; // Simple union without rank/size optimization
}

std::vector<Edge> calculateKruskalMST(const std::vector<Edge>& edges, int numNodes) {
    std::vector<Edge> mstEdges;
     if (numNodes <= 0) return mstEdges;

    std::vector<int> parent(numNodes);
    for (int i = 0; i < numNodes; ++i)
        parent[i] = i; // Initialize each node as its own parent

    std::vector<Edge> sortedEdges = edges;
    // Sort edges by weight (using Edge::operator<)
    std::sort(sortedEdges.begin(), sortedEdges.end());

    int edgesInMST = 0;
    for (const auto& edge : sortedEdges) {
        // Adjust 1-based IDs to 0-based indices
        int fromIdx = edge.from - 1;
        int toIdx = edge.to - 1;

        // Check index validity
        if (fromIdx < 0 || fromIdx >= numNodes || toIdx < 0 || toIdx >= numNodes) {
            continue; // Skip invalid edge data
        }


        if (findSet(parent, fromIdx) != findSet(parent, toIdx)) {
            mstEdges.push_back(edge);
            unionSets(parent, fromIdx, toIdx);
             edgesInMST++;
             if (edgesInMST == numNodes - 1) { // Optimization: stop when MST is complete
                 break;
             }
        }
    }
    // Optional: Check if edgesInMST == numNodes - 1
    return mstEdges;
}


void generateCompleteGraph(std::vector<Edge>& edges, int numNodes)
{
        edges.clear();
        if (numNodes < 2) return;
        for (int i = 1; i <= numNodes; ++i) {
            for (int j = i + 1; j <= numNodes; ++j) {
                // Assign random weights for variety, or keep as 1
                 edges.push_back({ i, j, GetRandomValue(1, 10) });
            }
        }
}

void generateCycleGraph(std::vector<Edge>& edges, int numNodes) {
    edges.clear();
     if (numNodes < 3) return; // Cycle needs at least 3 nodes
    for (int i = 1; i < numNodes; ++i) {
        edges.push_back({ i, i + 1, GetRandomValue(1, 10) });
    }
    edges.push_back({ numNodes, 1, GetRandomValue(1, 10) }); // Connect last to first
}

void generatePathGraph(std::vector<Edge>& edges, int numNodes, std::vector<Vector2>& nodePositions, int screenWidth, int screenHeight) {
    edges.clear();
     if (numNodes < 1) return;

    nodePositions.resize(numNodes);

    // Create the path edges
    for (int i = 1; i < numNodes; ++i) {
        edges.push_back({ i, i + 1, GetRandomValue(1, 10) });
    }

    // Simple linear positioning (can be replaced with more sophisticated layouts)
    float spacing = 150.0f;
    float startX = (screenWidth - (numNodes > 1 ? (numNodes - 1) * spacing : 0)) / 2.0f;
    float centerY = screenHeight / 2.0f;

    for (int i = 0; i < numNodes; ++i) {
        nodePositions[i] = { startX + i * spacing, centerY };
    }
}

void generateStarGraph(std::vector<Edge>& edges, int numNodes) {
    edges.clear();
    if (numNodes < 2) return; // Star needs at least 2 nodes (center + 1)
    int centerNode = 1;
    for (int i = 2; i <= numNodes; ++i) {
        edges.push_back({ centerNode, i, GetRandomValue(1, 10) });
    }
}

void generateRandomGraph(std::vector<Edge>& edges, int numNodes, int numEdges) {
    edges.clear();
     if (numNodes <= 0) return;

    // Ensure minimum edges for potential connectivity
     int maxPossibleEdges = (numNodes * (numNodes - 1)) / 2;
    numEdges = std::min(numEdges, maxPossibleEdges); // Cap edges
     numEdges = std::max(numEdges, 0); // Ensure non-negative

    // Use C++ <random> for better randomness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> nodeDist(1, numNodes);
    std::uniform_int_distribution<> weightDist(1, 10);

    // Strategy: Create a spanning tree first (e.g., random tree or path) to ensure connectivity, then add remaining edges randomly.

    // 1. Create a random spanning tree using a variation of randomized Prim/Kruskal or just connect randomly.
    std::vector<int> nodes;
     for(int i = 1; i <= numNodes; ++i) nodes.push_back(i);
    std::shuffle(nodes.begin(), nodes.end(), gen); // Random order

     std::vector<bool> connected(numNodes + 1, false);
     int connectedCount = 0;
     if (numNodes > 0) {
         connected[nodes[0]] = true;
         connectedCount = 1;
     }

     // Connect nodes in the shuffled order to form a tree-like structure
     for (size_t i = 1; i < nodes.size(); ++i) {
         int u = nodes[i];
         // Connect 'u' to a random node already in the connected component
        std::uniform_int_distribution<> connectedNodeDist(0, i - 1);
         int v_idx = connectedNodeDist(gen);
         int v = nodes[v_idx];

         if (!edgeExists(edges, u, v)) { // Should not exist yet, but check anyway
             edges.push_back({std::min(u, v), std::max(u, v), weightDist(gen)});
         }
         connected[u] = true;
         connectedCount++;
     }


    // 2. Add remaining random edges until numEdges is reached
     int currentEdges = edges.size();
     int attempts = 0; // Prevent infinite loop if graph is dense
     int maxAttempts = maxPossibleEdges * 2; // Heuristic limit

     while (currentEdges < numEdges && attempts < maxAttempts) {
        int u = nodeDist(gen);
        int v = nodeDist(gen);
        attempts++;

         if (u != v) {
            int from = std::min(u, v);
            int to = std::max(u, v);
             if (!edgeExists(edges, from, to)) {
                 edges.push_back({ from, to, weightDist(gen) });
                 currentEdges++;
                 attempts = 0; // Reset attempts after success
            }
        }
    }
    if (!isGraphConnected(edges, numNodes)) {
        std::cerr << "Warning: Generated random graph might not be connected (should not happen)." << std::endl;
    }
}
// bool isGraphConnected(const std::vector<Edge>& edges, int numNodes); // Declare it if needed

class Slider {
    public:
        Rectangle rect;
        Rectangle knob;
        float value; // Normalized value (0.0 - 1.0)
        float minValue;
        float maxValue;
        bool isDragging;
        std::string label;
        bool editValue;
        std::string editBuffer;
    
        Slider(Rectangle r, float minVal, float maxVal, std::string l);
    
        void update();
        float getValue() const; // Returns the actual value within min/max range
        void draw();
        bool isHovered() const;
        bool isEditing() const;
        void startEditing();
        void stopEditing();
        std::string getLabel() const;
    
        // Add a method to set the value externally (e.g., from loaded settings)
        void setValue(float actualValue);
};

Slider::Slider(Rectangle r, float minVal, float maxVal, std::string l)
    : rect(r), minValue(minVal), maxValue(maxVal), value(0.5f), // Default normalized value
      isDragging(false), label(std::move(l)), editValue(false), editBuffer("") {
    // Initial knob position based on default value
    knob = { rect.x + (rect.width - 15) * value, rect.y - 5, 15, rect.height + 10 };
}

void Slider::update() {
    Vector2 mousePos = GetMousePosition();

    // Check for starting drag
    if (!isDragging && CheckCollisionPointRec(mousePos, knob) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isDragging = true;
    }

    // Handle dragging
    if (isDragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        float newKnobX = mousePos.x - knob.width / 2.0f;
        // Clamp knob position within the slider track bounds
        float clampedKnobX = Clamp(newKnobX, rect.x, rect.x + rect.width - knob.width);
        knob.x = clampedKnobX;
        // Update normalized value based on knob position
        if ((rect.width - knob.width) > 0) { // Avoid division by zero
            value = (knob.x - rect.x) / (rect.width - knob.width);
        } else {
            value = 0.0f; // Or 0.5f if width is zero
        }
        value = Clamp(value, 0.0f, 1.0f); // Ensure value stays within [0, 1]
    }

    // Stop dragging
    if (isDragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isDragging = false;
    }

    // --- Handle Direct Value Editing ---
    Rectangle valueTextRect = { rect.x + rect.width + 10, rect.y, 60, (float)GetFontDefault().baseSize }; // Approx area of value text

    // Start editing on click
    if (!editValue && CheckCollisionPointRec(mousePos, valueTextRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        startEditing();
    }

    // Handle input while editing
    if (editValue) {
         SetMouseCursor(MOUSE_CURSOR_IBEAM);
         int key = GetCharPressed();
         while(key > 0) {
            // Allow numbers, decimal point (only one), and minus sign (at start)
            if (((key >= '0' && key <= '9') ||
                 (key == '.' && editBuffer.find('.') == std::string::npos) ||
                 (key == '-' && editBuffer.empty())) &&
                 (editBuffer.length() < 10)) // Limit length
            {
                 editBuffer += (char)key;
            }
             key = GetCharPressed(); // Check next char in queue
         }


        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (!editBuffer.empty()) {
                editBuffer.pop_back();
            }
        }

        // Finish editing
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePos, valueTextRect))) {
             stopEditing();
             SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    } else {
         // Optional: Change cursor on hover over value text
         if (CheckCollisionPointRec(mousePos, valueTextRect)) {
             SetMouseCursor(MOUSE_CURSOR_IBEAM);
         } else {
             // Check if hovering slider knob or bar before setting default
             if (!isDragging && !CheckCollisionPointRec(mousePos, knob) && !CheckCollisionPointRec(mousePos, rect)) {
                 // SetMouseCursor(MOUSE_CURSOR_DEFAULT); // May interfere with other UI elements
             }
         }
    }
}


float Slider::getValue() const {
    // Lerp between minValue and maxValue based on normalized value
    return minValue + (maxValue - minValue) * value;
}

void Slider::draw() {
    // Draw label
    DrawText(label.c_str(), rect.x, rect.y - 20, 16, RAYWHITE);

    // Draw slider track
    DrawRectangleRec(rect, LIGHTGRAY);
     // Optional: Draw filled part of the track
     DrawRectangle(rect.x, rect.y, (knob.x + knob.width / 2.0f) - rect.x, rect.height, Fade(RED, 0.5f));

    // Draw knob
    DrawRectangleRec(knob, isDragging ? DARKGRAY : RED); // Change color when dragging

    // Draw value text
    std::stringstream ss;
     ss << std::fixed << std::setprecision(2) << getValue();
     std::string valueStr = ss.str();

    // Draw the value text area - make it clickable
    Rectangle valueTextRect = { rect.x + rect.width + 10, rect.y, (float)MeasureText(valueStr.c_str(), 16) + 10, 16.0f }; // Slightly wider for click target

    if (editValue) {
        // Draw editing box instead of plain text
         DrawRectangleRec(valueTextRect, WHITE);
         DrawRectangleLinesEx(valueTextRect, 1, BLACK);
         DrawText(editBuffer.c_str(), valueTextRect.x + 5, valueTextRect.y , 16, BLACK);
          // Draw blinking cursor
         if (((int)(GetTime() * 2.0f)) % 2 == 0) {
              DrawText("|", valueTextRect.x + 5 + MeasureText(editBuffer.c_str(), 16), valueTextRect.y, 16, BLACK);
         }

    } else {
        // Draw normal value text
         DrawText(valueStr.c_str(), valueTextRect.x + 5, valueTextRect.y, 16, RAYWHITE);
         // Optional: Highlight on hover
         if (CheckCollisionPointRec(GetMousePosition(), valueTextRect)) {
             DrawRectangleLinesEx(valueTextRect, 1, YELLOW);
         }
    }
}


bool Slider::isHovered() const {
    // Consider hovering over knob or label as well?
    return CheckCollisionPointRec(GetMousePosition(), rect) || CheckCollisionPointRec(GetMousePosition(), knob);
}

bool Slider::isEditing() const {
    return editValue;
}

void Slider::startEditing() {
    editValue = true;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << getValue();
    editBuffer = ss.str();
}

void Slider::stopEditing() {
     editValue = false;
     SetMouseCursor(MOUSE_CURSOR_DEFAULT);
     float newValueFromBuffer = getValue(); // Default to current value if parse fails
    try {
        newValueFromBuffer = std::stof(editBuffer);
         // Clamp the input value to the slider's range
         newValueFromBuffer = Clamp(newValueFromBuffer, minValue, maxValue);
         setValue(newValueFromBuffer); // Use setValue to update normalized value and knob

    } catch (const std::invalid_argument& e) {
         // Handle invalid input (e.g., "abc") - keep the old value
         TraceLog(LOG_WARNING, "SLIDER: Invalid number format in edit buffer: %s", editBuffer.c_str());
    } catch (const std::out_of_range& e) {
         // Handle input out of float range - clamp to min/max
         TraceLog(LOG_WARNING, "SLIDER: Number out of range in edit buffer: %s", editBuffer.c_str());
         // Decide whether to clamp or revert - clamping seems more user-friendly
         try { // Need nested try-catch for potentially huge numbers
             double largeVal = std::stod(editBuffer); // Use double for intermediate check
             newValueFromBuffer = Clamp((float)largeVal, minValue, maxValue);
              setValue(newValueFromBuffer);
         } catch (...) {
              // Revert if even double parsing fails
         }

    }
    // Update edit buffer to reflect the final value (or revert if needed)
    // std::stringstream ss;
    // ss << std::fixed << std::setprecision(2) << getValue();
    // editBuffer = ss.str(); // Update buffer to match potentially clamped value
}

std::string Slider::getLabel() const {
    return label;
}

// Add the implementation for setValue
void Slider::setValue(float actualValue) {
    // Clamp the incoming value to the allowed range
    actualValue = Clamp(actualValue, minValue, maxValue);
    // Calculate the normalized value
    if (maxValue - minValue != 0) {
         value = (actualValue - minValue) / (maxValue - minValue);
    } else {
         value = 0.0f; // Or 0.5f if min == max
    }
     value = Clamp(value, 0.0f, 1.0f); // Ensure normalization is clamped

    // Update knob position based on the new normalized value
    knob.x = rect.x + (rect.width - knob.width) * value;
    // Update edit buffer if currently editing
    if (editValue) {
         std::stringstream ss;
         ss << std::fixed << std::setprecision(2) << actualValue;
         editBuffer = ss.str();
    }
}

void positionNodesInCircle(std::vector<Vector2>& nodePositions, int numNodes, float centerX, float centerY, float radius) {
    if (numNodes <= 0) return;
    nodePositions.resize(numNodes);

    // Use C++ <random> for better randomness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distRadius(0.9f, 1.1f); // Adjust radius slightly
    std::uniform_real_distribution<> distAngle(-0.1f, 0.1f); // Adjust angle slightly

    for (int i = 0; i < numNodes; ++i) {
        float baseAngle = 2.0f * PI * i / numNodes;
        float angle = baseAngle + distAngle(gen);
        float currentRadius = radius * distRadius(gen);
        nodePositions[i] = {
            centerX + currentRadius * cosf(angle),
            centerY + currentRadius * sinf(angle)
        };
    }
}

void drawNode(Vector2 position, int id, Color color, float radius) {
    DrawCircleV(position, radius, color);
    DrawCircleV(position, radius * 0.9f, WHITE); // Inner circle slightly smaller

    std::string idText = std::to_string(id);
    int fontSize = (int)(radius * 0.8f); // Font size relative to radius
    float textWidth = MeasureText(idText.c_str(), fontSize);

    DrawText(idText.c_str(),
             (int)(position.x - textWidth / 2.0f),
             (int)(position.y - fontSize / 2.0f), // Center text vertically
             fontSize,
             DARKGRAY);
}

void drawBezierEdge(Vector2 start, Vector2 end, float thickness, Color color) {
    // Actual Bezier implementation would require control points.
    // Example: Vector2 control = Vector2Lerp(start, end, 0.5f); control.y -= 50; // Simple control point
    // DrawLineBezier(start, end, thickness, color); // Using Raylib's Bezier function
    DrawLineEx(start, end, thickness, color); // Keep it simple for now
}

void drawEdgeWeight(Vector2 start, Vector2 end, int weight) {
    Vector2 midPoint = Vector2Lerp(start, end, 0.5f); // Use Lerp for midpoint
    std::string weightText = std::to_string(weight);
    int fontSize = 14; // Consider making font size dynamic based on zoom?
    Vector2 textSize = MeasureTextEx(GetFontDefault(), weightText.c_str(), fontSize, 1); // Use MeasureTextEx

    // Calculate position to center the text background
    Vector2 textPosition = {
        midPoint.x - textSize.x / 2.0f,
        midPoint.y - textSize.y / 2.0f
    };

    // Padding for the background rectangle
    float padding = 2.0f;
    Rectangle bgRect = {
        textPosition.x - padding,
        textPosition.y - padding,
        textSize.x + 2 * padding,
        textSize.y + 2 * padding
    };

    DrawRectangleRec(bgRect, WHITE); // Draw background first
    DrawText(weightText.c_str(), (int)textPosition.x, (int)textPosition.y, fontSize, BLACK); // Draw text on top
}

void drawMSTInfo(int totalWeight, bool isMSTDrawingPaused, Rectangle pauseResumeButton, Rectangle menuRect) {
    float textX = menuRect.x + 10;
    float textY = menuRect.y + 40; // Move down below the title
    int fontSize = 20;

    // Box for Total Weight
    std::string weightStr = "Total Weight: " + std::to_string(totalWeight);
    Vector2 weightTextSize = MeasureTextEx(GetFontDefault(), weightStr.c_str(), fontSize, 1);
    Rectangle weightBox = {textX, textY, weightTextSize.x + 10, weightTextSize.y + 10}; // Fit box to text

    DrawRectangleRec(weightBox, WHITE);
    DrawRectangleLinesEx(weightBox, 1, BLACK);
    DrawText(weightStr.c_str(), weightBox.x + 5, weightBox.y + 5, fontSize, BLACK);

    // Position Pause/Resume button below the weight box
    // Ensure pauseResumeButton passed in has correct X, Width, Height
    Rectangle actualPauseButton = pauseResumeButton; // Copy to modify Y
    actualPauseButton.y = weightBox.y + weightBox.height + 5;

    // Draw Pause/Resume Button
    DrawRectangleRec(actualPauseButton, WHITE);
     DrawRectangleLinesEx(actualPauseButton, 1, CheckCollisionPointRec(GetMousePosition(), actualPauseButton) ? RED : BLACK); // Highlight on hover
    const char* pauseText = isMSTDrawingPaused ? "Resume" : "Pause";
    Vector2 pauseTextSize = MeasureTextEx(GetFontDefault(), pauseText, fontSize, 1);
    DrawText(pauseText,
             actualPauseButton.x + (actualPauseButton.width - pauseTextSize.x) / 2, // Center text
             actualPauseButton.y + (actualPauseButton.height - pauseTextSize.y) / 2,
             fontSize, BLACK);
}

void UpdateGraphCamera(Camera2D &camera, const std::vector<Vector2>& nodes, float nodeRadius, int screenWidth, int screenHeight, float uiLeftWidth, float uiRightWidth) {
    // Default camera state
    Vector2 defaultTarget = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    Vector2 defaultOffset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    float defaultZoom = 1.0f;

   if (nodes.empty()) {
       camera.target = defaultTarget;
       camera.offset = defaultOffset;
       camera.zoom = defaultZoom;
       return;
   }

   // 1. Define the viewport available for the graph
   float padding = 50.0f; // Padding around the graph within the viewport
   Rectangle viewport = {
       uiLeftWidth + padding,
       padding,
       screenWidth - uiLeftWidth - uiRightWidth - 2 * padding,
       screenHeight - 2 * padding
   };

   // Ensure viewport dimensions are positive
   viewport.width = std::max(1.0f, viewport.width);
   viewport.height = std::max(1.0f, viewport.height);

   // 2. Calculate the bounding box of the graph nodes
   Vector2 minBounds = nodes[0];
   Vector2 maxBounds = nodes[0];

   for (const auto& nodePos : nodes) {
       minBounds.x = std::min(minBounds.x, nodePos.x);
       minBounds.y = std::min(minBounds.y, nodePos.y);
       maxBounds.x = std::max(maxBounds.x, nodePos.x);
       maxBounds.y = std::max(maxBounds.y, nodePos.y);
   }

   // Expand bounds by node radius
   minBounds = Vector2SubtractValue(minBounds, nodeRadius);
   maxBounds = Vector2AddValue(maxBounds, nodeRadius);

   float graphWidth = maxBounds.x - minBounds.x;
   float graphHeight = maxBounds.y - minBounds.y;

   // Handle cases with zero or very small dimensions
   if (graphWidth < nodeRadius * 2) graphWidth = nodeRadius * 2;
   if (graphHeight < nodeRadius * 2) graphHeight = nodeRadius * 2;


   // 3. Calculate required zoom level
   float targetViewportRatio = 0.85f; // How much of the viewport the graph should fill
   float targetWidth = viewport.width * targetViewportRatio;
   float targetHeight = viewport.height * targetViewportRatio;

   float zoomX = (graphWidth > 0) ? (targetWidth / graphWidth) : defaultZoom;
   float zoomY = (graphHeight > 0) ? (targetHeight / graphHeight) : defaultZoom;
   float zoom = std::min(zoomX, zoomY);

   // Apply zoom limits
   const float minZoom = 0.1f;
   const float maxZoom = 2.0f; // Allow slightly more zoom in
   zoom = Clamp(zoom, minZoom, maxZoom);


   // 4. Calculate camera target (center of the graph's bounding box)
   Vector2 graphCenter = {
       minBounds.x + graphWidth / 2.0f,
       minBounds.y + graphHeight / 2.0f
   };

   // 5. Calculate camera offset (center of the viewport)
   Vector2 viewportCenter = {
       viewport.x + viewport.width / 2.0f,
       viewport.y + viewport.height / 2.0f
   };

   // 6. Update the camera
   camera.target = graphCenter;
   camera.offset = viewportCenter;
   camera.zoom = zoom;
}

int GetCursorIndex(const std::string& text, int row, int col) {
    int currentIndex = 0;
    int currentRow = 0;
    int currentCol = 0;

    while (currentIndex < text.length()) {
        if (currentRow == row && currentCol == col) {
            return currentIndex;
        }

        if (text[currentIndex] == '\n') {
            currentRow++;
            currentCol = 0;
        } else {
            currentCol++;
        }
        currentIndex++;
    }

    // If row/col is beyond the text, return the end index
    return text.length();
}

int GetColFromIndex(const std::string& text, int index) {
    int col = 0;
    // Clamp index to be within valid range
    index = std::max(0, std::min((int)text.length(), index));

    // Go backwards from the character *before* the index
    for (int i = index - 1; i >= 0; --i) {
       if (text[i] == '\n') {
           break; // Stop at the beginning of the line
       }
       col++;
   }
   return col;
}

Vector2 GetCursorScreenPos(const std::string& text, int row, int col, Rectangle rect, int fontSize) {
    float currentX = rect.x + 5; // Start position with padding
    float currentY = rect.y + 5;
    int currentRow = 0;
    int currentCol = 0;

    int textIndex = 0;
    while (textIndex < text.length()) {
        if (currentRow == row && currentCol == col) {
            // Found the target position
            return {currentX, currentY};
        }

        if (text[textIndex] == '\n') {
             currentY += (float)fontSize * 1.1f; // Move down for newline (adjust line spacing)
             currentX = rect.x + 5; // Reset X to start of line
            currentRow++;
            currentCol = 0;
        } else {
            // Measure the current character and advance X
             // Need to handle potential multi-byte UTF-8 chars if supporting them
             // For basic ASCII/single-byte:
             // float charWidth = MeasureText(TextFormat("%c", text[textIndex]), fontSize);
             // For Raylib's default font handling (might handle some UTF-8):
             int codepoint = 0;
             int bytesProcessed = 0;
             // Get the next codepoint and its size in bytes
             codepoint = GetCodepoint(&text[textIndex], &bytesProcessed);
             // Get glyph info for the codepoint
             GlyphInfo glyph = GetGlyphInfo(GetFontDefault(), codepoint);
             // Advance X by the glyph's advanceX, scaled if needed, or just width
             // Using glyph.advanceX is generally better for text layout
             currentX += (glyph.advanceX == 0) ? glyph.image.width : glyph.advanceX;
             // Add spacing between characters
             currentX += GetFontDefault().glyphPadding;


             currentCol++;
             // Important: Increment textIndex by the number of bytes processed for the codepoint
             textIndex += bytesProcessed -1; // -1 because the loop adds 1 later
        }
         textIndex++;
    }

    // If the loop finishes, the cursor is at the end of the text
    // If the last char was a newline, currentX/Y are already correct for the start of the next line
    // Otherwise, currentX/Y point to the position *after* the last character
    return {currentX, currentY};
}

std::string ValidateMatrixInput(const std::string& input, std::vector<std::vector<int>>& adjacencyMatrix, int& numNodes) {
    adjacencyMatrix.clear();
    numNodes = 0;
    std::stringstream ss(input);
    std::string line;
    int lineNum = 0;

    // 1. Read number of nodes
    lineNum++;
    if (!std::getline(ss, line)) {
        return "Error (Line " + std::to_string(lineNum) + "): Input is empty.";
    }
    // Trim leading/trailing whitespace from the first line
    line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
    line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

    if (line.empty()) {
         return "Error (Line " + std::to_string(lineNum) + "): First line (node count) is empty.";
    }


    try {
        numNodes = std::stoi(line);
    } catch (const std::invalid_argument& e) {
        return "Error (Line " + std::to_string(lineNum) + "): Node count is not a valid integer.";
    } catch (const std::out_of_range& e) {
        return "Error (Line " + std::to_string(lineNum) + "): Node count is out of integer range.";
    }

    if (numNodes <= 0) {
        return "Error (Line " + std::to_string(lineNum) + "): Node count must be positive.";
    }

    // 2. Read the matrix rows
    int rowCount = 0;
    while (rowCount < numNodes && std::getline(ss, line)) {
        lineNum++;
        // Trim whitespace for the current matrix row line
         line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
         line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

         // Allow empty lines to be skipped if needed, or enforce strict rows?
         // Let's enforce strict rows for now. If a line is empty, it's an error.
         if (line.empty() && rowCount < numNodes) {
              // This check might be too strict if blank lines are expected between rows.
              // If allowing blank lines, add a continue here.
              // For now, treat blank lines within the matrix data as an error.
              return "Error (Line " + std::to_string(lineNum) + "): Empty line found within matrix data.";
         }
         // Skip truly empty lines if desired (e.g., after the matrix)
         // if (line.empty()) continue;


        std::vector<int> row;
        std::stringstream lineStream(line);
        int value;
        char separator; // To potentially consume separators like ',' or spaces

        while (lineStream >> value) {
             row.push_back(value);
            // Optionally consume trailing separators to handle formats like "1, 2, 3" or "1 2 3"
            // Peek at the next character
            while (lineStream.peek() == ',' || lineStream.peek() == ' ' || lineStream.peek() == '\t') {
                 lineStream.ignore(); // Consume the separator
             }

        }

        // Check if the line contained anything other than numbers and separators
        if (!lineStream.eof()) {
             std::string remaining;
             lineStream >> remaining; // Try to read the non-numeric part
             // Allow trailing comments starting with '#' or '//'?
             // For now, strict check: only numbers and allowed separators
             return "Error (Line " + std::to_string(lineNum) + "): Invalid characters found: '" + remaining.substr(0,10) + "...'"; // Show first few invalid chars
        }


        // Check column count consistency
        if (row.size() != numNodes) {
            return "Error (Line " + std::to_string(lineNum) + "): Row " + std::to_string(rowCount + 1) +
                   " has " + std::to_string(row.size()) + " columns, expected " + std::to_string(numNodes) + ".";
        }

        adjacencyMatrix.push_back(row);
        rowCount++;
    }

    // Check if enough rows were read
    if (rowCount != numNodes) {
        return "Error: Expected " + std::to_string(numNodes) + " rows in the matrix, but found " + std::to_string(rowCount) + ".";
    }

    // Optional: Check for symmetry (if it's an undirected graph)
    for (int i = 0; i < numNodes; ++i) {
        for (int j = i + 1; j < numNodes; ++j) {
            if (adjacencyMatrix[i][j] != adjacencyMatrix[j][i]) {
                 // Just a warning for now, could be an error depending on requirements
                 // return "Warning: Matrix is not symmetric (value at [" + std::to_string(i+1) + "," + std::to_string(j+1) + "] != [" + std::to_string(j+1) + "," + std::to_string(i+1) + "]).";
             }
        }
         // Check diagonal (should be 0 for simple graphs)
         if (adjacencyMatrix[i][i] != 0) {
             // return "Warning: Non-zero diagonal element found at [" + std::to_string(i+1) + "," + std::to_string(i+1) + "].";
         }
    }


    return ""; // Success!
}

std::string ReadTextFile(const char *filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Error: Could not open file: " << filePath << std::endl;
        return ""; // Return empty string on error
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf(); // Read entire file into buffer
    fileStream.close();
    return buffer.str();
}

bool LoadGraphFromFile(const char* filePath,
    std::vector<Edge>& outEdges,
    std::vector<Vector2>& outNodePositions,
    int screenWidth, int screenHeight,
    std::string& outErrorMessage)
{
    outEdges.clear();
    outNodePositions.clear();
    outErrorMessage = "";

    // 1. Read file content
    std::string fileContent = ReadTextFile(filePath);
    if (fileContent.empty()) {
    // ReadTextFile already prints to cerr, but set error message too
    outErrorMessage = "Error: File is empty or could not be read: " + std::string(filePath);
    return false;
    }

    // 2. Validate the content as an adjacency matrix
    std::vector<std::vector<int>> adjacencyMatrix;
    int numNodes = 0;
    outErrorMessage = ValidateMatrixInput(fileContent, adjacencyMatrix, numNodes);

    if (!outErrorMessage.empty()) {
    // Prepend file info to the validation error message
    outErrorMessage = "Error in file '" + std::string(filePath) + "':\n" + outErrorMessage;
    return false; // Invalid matrix format
    }

    // 3. Matrix is valid, create the edge list
    for (int i = 0; i < numNodes; ++i) {
        // Iterate only through the upper triangle for undirected graphs
        for (int j = i + 1; j < numNodes; ++j) {
          int weight = adjacencyMatrix[i][j];
          if (weight != 0) {
          if (weight <= 0) {
  // Decide policy: error, warning, default weight?
  // Let's use a default weight of 1 and maybe log a warning.
  // std::cerr << "Warning: Non-positive weight (" << weight << ") found between nodes "
  //           << (i + 1) << " and " << (j + 1) << " in file '" << filePath
  //           << "'. Using weight 1." << std::endl;
        weight = 1;
        }
        outEdges.push_back({ i + 1, j + 1, weight });
      }
// If the graph could be directed, you'd check adjacencyMatrix[j][i] separately.
    }
}

// 4. Check connectivity (optional but good)
    if (numNodes > 1 && !isGraphConnected(outEdges, numNodes)) {
    outErrorMessage = "Warning: Graph loaded from '" + std::string(filePath) + "' is not connected.";
    return false; 
    }

    if (numNodes > 0) {
    float layoutRadius = std::min(screenWidth, screenHeight) / 3.0f; // Adjusted radius
    positionNodesInCircle(outNodePositions, numNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
    }
return true; 
}

enum class EditTool { // Use enum class for stronger typing
    TOOL_NONE,
    TOOL_ADD_VERTEX,
    TOOL_ADD_EDGE_START,
    TOOL_ADD_EDGE_END,
    TOOL_EDIT_WEIGHT,
    TOOL_MOVE_VERTEX,
    TOOL_DELETE_VERTEX,
    TOOL_DELETE_EDGE
};

void ResetStates(bool &isCreating, bool &isRandomizing, bool &isShowingExamples,
    bool &showMatrixInput, bool &graphDrawn,
    std::string &numNodesStr, std::string &numEdgesStr, std::string &matrixInput,
    bool &nodesFocused, bool &edgesFocused,
    bool &showMSTMenu, bool &isEditingGraph, EditTool& currentTool,
    bool& showFileError, std::string& fileErrorMessage,
    bool& showError, std::string& errorMessage,
    bool& showMSTError, std::string& mstErrorMessage);
    
bool isCreating = false; // Keep original definitions here
bool isRandomizing = false;
bool isShowingExamples = false; // Flag if "Examples" buttons are active

bool isCreatingActive = false;       // Flag if "Create" input fields are active
bool isRandomizingActive = false;    // Flag if "Random" was just clicked
bool isShowingExamplesActive = false; // Flag if "Examples" buttons are shown
bool isInputActive = false;          // Flag if "Input" mode (matrix) is active
bool isFileActive = false;           // Flag if "File" operation is in progress

bool graphDrawn = false;
std::vector<Edge> edges;
std::vector<Vector2> nodePositions;
Camera2D graphCamera = { 0 };

// UI State / Input Buffers
std::string numNodesStr = "";
std::string numEdgesStr = "";
std::string matrixInput = ""; // For matrix input window
std::string weightInputBuffer = ""; // For editing edge weights

// Focus flags
bool nodesFocused = false;
bool edgesFocused = false;
bool matrixInputFocused = false;
bool isEditingWeight = false; // If the weight input box is active

// Window/Mode states
bool showMatrixInput = false;
bool showExampleButtons = false; // If example buttons (K5, C6...) are visible
bool showMSTMenu = false;
bool isEditingGraph = false; // Master flag for graph editing mode

// Editing Tool State
EditTool currentTool = EditTool::TOOL_NONE;
int selectedNodeIndex = -1; // Index of node selected for move/edge start/delete
int selectedEdgeIndex = -1; // Index of edge selected for weight edit/delete
bool isDraggingNode = false;

// MST State
bool usePrim = false;
bool useKruskal = false;
std::vector<Edge> mstEdges;
std::vector<bool> mstEdgesDrawn; // Track which MST edges are visually drawn
int mstEdgeIndex = 0;
float mstDrawTimer = 0.0f;
bool mstDoneDrawing = false;
int totalMSTWeight = 0;
bool showWeightInfo = false;
bool isMSTDrawingPaused = false;
std::vector<bool> mstNodesDrawn; // Track which nodes are part of the drawn MST

// Error message states
std::string errorMessage = "";      // General/Create errors
std::string errorMessageInput = ""; // Matrix input errors
std::string mstErrorMessage = "";   // MST related errors
std::string fileErrorMessage = "";  // File loading errors
std::string outErrorMessage = "";
bool showError = false;
bool showMSTError = false;
bool showFileError = false;

// Cursor state for matrix input
int cursorColumn = 0;
int cursorRow = 0;
float cursorTimer = 0.0f;

// Forward declaration if ResetStates is defined in this file instead of AppState.cpp
// void ResetStates(...);

// --- Main Function ---
int main() {
    const int screenWidth = 1400;
    const int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "Graph MST Visualizer");
    SetTargetFPS(60);

    // --- Initialize UI Elements ---
    // Left Panel Buttons
    Rectangle createButton = {10, 10, 100, 30};
    Rectangle randomButton = {10, createButton.y + createButton.height + 10, 100, 30};
    Rectangle exampleButton = {10, randomButton.y + randomButton.height + 10, 100, 30};
    Rectangle inputButton = {10, exampleButton.y + exampleButton.height + 10, 100, 30};
    Rectangle fileButton = {10, inputButton.y + inputButton.height + 10, 100, 30};
    Rectangle editButton = {10, fileButton.y + fileButton.height + 10, 100, 30};
    Rectangle mstButton = {10, editButton.y + editButton.height + 10, 100, 30};

    // Create Mode Inputs
    Rectangle nodesInputRect = { createButton.x + createButton.width + 30, 10, 100, 30 };
    Rectangle edgesInputRect = { nodesInputRect.x + nodesInputRect.width + 30, 10, 100, 30 };

    // Example Buttons (positions relative to exampleButton)
    Rectangle k5Button = { exampleButton.x + exampleButton.width + 10, exampleButton.y, 60, 30 };
    Rectangle c6Button = { k5Button.x + k5Button.width + 10, exampleButton.y, 60, 30 };
    Rectangle p4Button = { c6Button.x + c6Button.width + 10, exampleButton.y, 60, 30 };
    Rectangle s7Button = { p4Button.x + p4Button.width + 10, exampleButton.y, 60, 30 };

    // Matrix Input Window Elements
    Rectangle inputWindowRect = {screenWidth / 4.0f, screenHeight / 4.0f, screenWidth / 2.0f, screenHeight / 2.0f};
    Rectangle matrixInputAreaRect = {inputWindowRect.x + 10, inputWindowRect.y + 40, inputWindowRect.width - 20, inputWindowRect.height - 90};
    Rectangle submitMatrixButton = {inputWindowRect.x + 10, inputWindowRect.y + inputWindowRect.height - 40, 80, 30};
    Rectangle closeMatrixButton = {submitMatrixButton.x + submitMatrixButton.width + 10, submitMatrixButton.y, 80, 30};

    // MST Menu Elements
    Rectangle mstMenuRect = {screenWidth / 8.0f, screenHeight / 8.0f, screenWidth * 3.0f / 4.0f, screenHeight * 3.0f / 4.0f};
    const int mstButtonWidth = 80;
    const int mstButtonHeight = 30;
    const int mstButtonSpacing = 10;
    const float mstButtonsY = mstMenuRect.y + mstMenuRect.height - mstButtonHeight - 10;
    Rectangle primButton = {mstMenuRect.x + 10, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
    Rectangle kruskalButton = {primButton.x + mstButtonWidth + mstButtonSpacing, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
    Rectangle backButton = {kruskalButton.x + mstButtonWidth + mstButtonSpacing, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
    // Define pause button relative position for drawMSTInfo
    Rectangle pauseResumeButtonDef = {mstMenuRect.x + 10, mstMenuRect.y + 75, 100, 30}; // drawMSTInfo calculates final Y

    Slider speedSlider = Slider({mstMenuRect.x + mstMenuRect.width - 160, mstMenuRect.y + 45, 150, 20}, 0.1f, 2.0f, "Speed:"); // Position near top right

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


    // Initialize Camera
    graphCamera.target = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    graphCamera.offset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    graphCamera.rotation = 0.0f;
    graphCamera.zoom = 1.0f;

    // --- Main Game Loop ---
    while (!WindowShouldClose()) {
        // --- Input Handling & State Updates ---
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, graphCamera); // Mouse in graph space

        // ** Handle UI Interactions FIRST **
        // This prevents graph interactions when clicking on UI elements

        bool clickedOnUI = false;

        // Check Left Panel Buttons (if not in a modal state like MST menu or Matrix input)
         if (!showMSTMenu && !showMatrixInput && !isEditingGraph) {
             if (CheckCollisionPointRec(mousePos, createButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 clickedOnUI = true;
                 ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                 edges.clear(); nodePositions.clear(); // Clear graph data
                 isCreating = true; // Enter create mode (input fields)
                 isCreatingActive = true;
             }
             else if (CheckCollisionPointRec(mousePos, randomButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 clickedOnUI = true;
                 ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                 edges.clear(); nodePositions.clear(); // Clear graph data

                 // Generate random graph immediately
                 int r_numNodes = GetRandomValue(5, 12); // Example range
                 int r_maxEdges = r_numNodes * (r_numNodes - 1) / 2;
                 int r_minEdges = (r_numNodes > 1) ? r_numNodes - 1 : 0; // Min edges for connectivity
                 int r_numEdges = (r_minEdges >= r_maxEdges) ? r_maxEdges : GetRandomValue(r_minEdges, r_maxEdges);

                 generateRandomGraph(edges, r_numNodes, r_numEdges);
                 if (r_numNodes > 0) {
                     float layoutRadius = std::min(screenWidth, screenHeight) / 3.0f;
                     positionNodesInCircle(nodePositions, r_numNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
                     graphDrawn = true;
                     UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 120.0f, 0.0f); // Update camera for new graph
                 }
                 isRandomizing = false; // Reset flag after generation
             }
             else if (CheckCollisionPointRec(mousePos, exampleButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 clickedOnUI = true;
                 ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                 edges.clear(); nodePositions.clear(); // Clear graph data
                 showExampleButtons = !showExampleButtons; // Toggle visibility
                 isShowingExamplesActive = showExampleButtons;
                 isCreating = false;
             }
             else if (CheckCollisionPointRec(mousePos, inputButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 clickedOnUI = true;
                 ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                 edges.clear(); nodePositions.clear(); // Clear graph data
                 showMatrixInput = true;
                 matrixInput = ""; // Clear previous input
                 errorMessageInput = ""; // Clear errors
                 matrixInputFocused = true; // Focus the input area
                 cursorRow = 0; cursorColumn = 0;
                 isInputActive = true;
                 isCreating = false;
                 showExampleButtons = false;
             }
             else if (CheckCollisionPointRec(mousePos, fileButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 clickedOnUI = true;
                 ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                              UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 120.0f, 0.0f);
                         }
                     } else {
                         graphDrawn = false; // Ensure no drawing if load fails
                         fileErrorMessage = loadError;
                         showFileError = true;
                     }
                 } else {
                     // User cancelled
                 }
                 isFileActive = false; // Reset flag
                 // --- End TinyFileDialog ---
             }
              else if (CheckCollisionPointRec(mousePos, editButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                  clickedOnUI = true;
                  if (graphDrawn && !nodePositions.empty()) {
                      // Enter Edit Mode
                      ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                      // Keep graphDrawn = true
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
                      // Show error: Cannot edit without a graph
                       ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                       edges.clear(); nodePositions.clear(); // Clear just in case
                      showError = true;
                      errorMessage = "Create or load a graph first to edit.";
                      isEditingGraph = false; // Make sure edit mode is off
                  }
              }
             else if (CheckCollisionPointRec(mousePos, mstButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 clickedOnUI = true;
                 if (graphDrawn && !nodePositions.empty()) {
                     // Enter MST Menu Mode
                     ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                     // Keep graph data, graphDrawn = true
                     graphDrawn = true;
                     showMSTMenu = true;
                     usePrim = false; useKruskal = false; // Reset algorithm choice
                     mstEdges.clear(); mstEdgesDrawn.clear(); mstNodesDrawn.clear();
                     mstEdgeIndex = 0; mstDrawTimer = 0.0f; mstDoneDrawing = false;
                     totalMSTWeight = 0; showWeightInfo = false; isMSTDrawingPaused = false;
                     isCreating = false;
                     showExampleButtons = false;
                 } else {
                     // Show error: Cannot run MST without a graph
                     ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                     edges.clear(); nodePositions.clear(); // Clear just in case
                     showMSTError = true;
                     mstErrorMessage = "Create or load a graph first for MST.";
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
                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 120.0f, 0.0f);
             };
              auto generatePathAndUpdate = [&](int n) { // Special case for path layout
                 clickedOnUI = true;
                 generatePathGraph(edges, n, nodePositions, screenWidth, screenHeight); // nodePositions updated by func
                 exampleNodes = n;
                 // nodePositions already set by generatePathGraph
                 graphDrawn = true;
                 showExampleButtons = false;
                 isShowingExamplesActive = false;
                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 120.0f, 0.0f);
             };


             if (CheckCollisionPointRec(mousePos, k5Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) generateAndUpdate(5, generateCompleteGraph);
             else if (CheckCollisionPointRec(mousePos, c6Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) generateAndUpdate(6, generateCycleGraph);
             else if (CheckCollisionPointRec(mousePos, p4Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) generatePathAndUpdate(4); // Use path layout version
             else if (CheckCollisionPointRec(mousePos, s7Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) generateAndUpdate(7, generateStarGraph);
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

             if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
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
                             // Success
                             nodePositions.resize(numNodes);
                             float layoutRadius = std::min(screenWidth, screenHeight) / 3.0f;
                             positionNodesInCircle(nodePositions, numNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
                             graphDrawn = true;
                             isCreating = false; // Exit create mode
                             isCreatingActive = false;
                             showError = false;
                             UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 120.0f, 0.0f);
                         }
                     }
                 } catch (const std::exception& e) {
                     showError = true; errorMessage = "Invalid number input.";
                 }
             }
         }

        // Check Matrix Input Window (if visible)
         if (showMatrixInput && !clickedOnUI) {
             // Check interactions within the input window (buttons, text area)
             if (CheckCollisionPointRec(mousePos, inputWindowRect)) {
                 clickedOnUI = true; // Click inside window is UI interaction

                 if (CheckCollisionPointRec(mousePos, matrixInputAreaRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                     matrixInputFocused = true;
                     // TODO: Calculate cursorRow/Col based on click position (more complex)
                 } else if (CheckCollisionPointRec(mousePos, submitMatrixButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                     std::vector<std::vector<int>> adjMatrix;
                     int nodesFromFile = 0;
                     errorMessageInput = ValidateMatrixInput(matrixInput, adjMatrix, nodesFromFile);
                     if (errorMessageInput.empty()) {
                         // Validation success -> Create graph
                          LoadGraphFromFile("temp_matrix_string", edges, nodePositions, screenWidth, screenHeight, errorMessageInput); // Use LoadGraph logic slightly adapted

                          // Re-use LoadGraphFromFile logic by simulating file read
                         std::string loadError; // Temporary error string for LoadGraph logic
                         std::vector<Edge> tempEdges;
                         std::vector<Vector2> tempNodePos;

                          // --- Re-implement core LoadGraph logic here directly ---
                          // (Since we already validated and have the matrix)
                          edges.clear();
                          nodePositions.clear();
                          outErrorMessage = ""; // Use the local error message

                          int numNodes = adjMatrix.size(); // Already validated

                          // Create edge list from validated matrix
                          for (int i = 0; i < numNodes; ++i) {
                              for (int j = i + 1; j < numNodes; ++j) {
                                  int weight = adjMatrix[i][j];
                                  if (weight != 0) {
                                      if (weight <= 0) weight = 1; // Default weight
                                      edges.push_back({ i + 1, j + 1, weight });
                                  }
                              }
                          }

                          // Check connectivity
                          if (numNodes > 1 && !isGraphConnected(edges, numNodes)) {
                              errorMessageInput = "Warning: Input matrix results in a disconnected graph.";
                              // Decide if this prevents graph drawing - for now, allow it but show warning
                          }

                          // Position nodes
                          if (numNodes > 0) {
                              float layoutRadius = std::min(screenWidth, screenHeight) / 3.0f;
                              positionNodesInCircle(nodePositions, numNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
                          }
                         // --- End re-implemented logic ---


                         if (errorMessageInput.find("Error") == std::string::npos) { // Check if only warnings occurred
                             graphDrawn = true;
                             showMatrixInput = false; // Close window on success
                             matrixInputFocused = false;
                             isInputActive = false;
                             if (!nodePositions.empty()) {
                                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 120.0f, 0.0f);
                             }
                         } else {
                             // Error occurred during graph creation (e.g., connectivity if strict)
                             graphDrawn = false;
                             edges.clear();
                             nodePositions.clear();
                         }

                     }
                     // else: errorMessageInput already contains the validation error
                 } else if (CheckCollisionPointRec(mousePos, closeMatrixButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                     showMatrixInput = false;
                     matrixInputFocused = false;
                     isInputActive = false;
                     errorMessageInput = ""; // Clear errors on close
                 }

                 // Handle text input if focused
                 if (matrixInputFocused) {
                     int key = GetKeyPressed(); // Use GetKeyPressed for navigation keys
                      int charKey = GetCharPressed(); // Use GetCharPressed for printable chars

                     // Printable characters
                     while (charKey > 0) {
                         if ((charKey >= 32 && charKey <= 126) || charKey == '\n') { // Allow basic chars + newline
                             matrixInput.insert(GetCursorIndex(matrixInput, cursorRow, cursorColumn), 1, (char)charKey);
                             if (charKey == '\n') {
                                 cursorRow++;
                                 cursorColumn = 0;
                             } else {
                                 cursorColumn++;
                             }
                         }
                          charKey = GetCharPressed(); // Process next char in queue
                     }


                     // Special keys
                     if (IsKeyPressed(KEY_BACKSPACE)) {
                         int delIndex = GetCursorIndex(matrixInput, cursorRow, cursorColumn);
                         if (delIndex > 0) {
                             if (matrixInput[delIndex - 1] == '\n') {
                                 // Move cursor to end of previous line
                                 cursorRow--;
                                 cursorColumn = GetColFromIndex(matrixInput, delIndex-1); // Col of char before \n
                             } else {
                                 cursorColumn--;
                             }
                             matrixInput.erase(delIndex - 1, 1);
                         }
                     } else if (IsKeyPressed(KEY_DELETE)) {
                          int delIndex = GetCursorIndex(matrixInput, cursorRow, cursorColumn);
                          if (delIndex < matrixInput.length()) {
                              matrixInput.erase(delIndex, 1);
                              // Cursor position doesn't change relative to surrounding text
                          }
                     } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                         matrixInput.insert(GetCursorIndex(matrixInput, cursorRow, cursorColumn), 1, '\n');
                         cursorRow++;
                         cursorColumn = 0;
                     } else if (IsKeyPressed(KEY_LEFT)) {
                         if (cursorColumn > 0) cursorColumn--;
                         else if (cursorRow > 0) {
                             cursorRow--;
                             cursorColumn = GetColFromIndex(matrixInput, GetCursorIndex(matrixInput, cursorRow, INT_MAX)); // Go to end of prev line
                         }
                     } else if (IsKeyPressed(KEY_RIGHT)) {
                         int currentIndex = GetCursorIndex(matrixInput, cursorRow, cursorColumn);
                         if (currentIndex < matrixInput.length()) {
                             if (matrixInput[currentIndex] == '\n') {
                                 cursorRow++;
                                 cursorColumn = 0;
                             } else {
                                 cursorColumn++;
                             }
                         }
                     } else if (IsKeyPressed(KEY_UP)) {
                         if (cursorRow > 0) {
                             cursorRow--;
                             // Try to maintain column, clamped by new line length
                             cursorColumn = std::min(cursorColumn, GetColFromIndex(matrixInput, GetCursorIndex(matrixInput, cursorRow, INT_MAX)));
                         }
                     } else if (IsKeyPressed(KEY_DOWN)) {
                          // Check if next row exists
                          bool nextRowExists = false;
                          int currentIdx = GetCursorIndex(matrixInput, cursorRow, cursorColumn);
                          for (int i = currentIdx; i < matrixInput.length(); ++i) {
                              if (matrixInput[i] == '\n') {
                                  // Found the end of the current line, check if there's more text
                                  if (i + 1 < matrixInput.length()) {
                                       nextRowExists = true;
                                  }
                                  break; // Stop searching after first newline
                              }
                          }
                           // Also true if currently on the last line with no trailing newline
                           if (!nextRowExists && currentIdx == matrixInput.length()) {
                                // Check if the last char wasn't newline
                               if (!matrixInput.empty() && matrixInput.back() != '\n') nextRowExists = false; // No row below
                               else if (matrixInput.empty()) nextRowExists = false; // No row below
                               else { // Need to check if there's a line *after* the current one
                                     int lineCount = 0;
                                     for(char c : matrixInput) if (c == '\n') lineCount++;
                                     if (cursorRow < lineCount) nextRowExists = true;
                               }

                           }


                         if (nextRowExists) { // Only move down if next row logically exists
                             cursorRow++;
                             // Try to maintain column, clamped by new line length
                             cursorColumn = std::min(cursorColumn, GetColFromIndex(matrixInput, GetCursorIndex(matrixInput, cursorRow, INT_MAX)));
                         }
                     }
                     // Clamp cursor position just in case
                     int endIndex = GetCursorIndex(matrixInput, cursorRow, INT_MAX);
                     cursorColumn = std::min(cursorColumn, GetColFromIndex(matrixInput, endIndex));

                 } // end if matrixInputFocused

             } else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 // Clicked outside the matrix input window while it was open
                 matrixInputFocused = false; // Lose focus
             }
         } // end if showMatrixInput

        // Check MST Menu Interactions (if visible)
         if (showMSTMenu && !clickedOnUI) {
             if (CheckCollisionPointRec(mousePos, mstMenuRect)) {
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


                 // Check Button Clicks
                 if (CheckCollisionPointRec(mousePos, primButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                     if (!nodePositions.empty()) {
                         mstEdges = calculatePrimMST(edges, nodePositions.size());
                         if (mstEdges.size() < nodePositions.size() - 1 && nodePositions.size() > 1) {
                            mstErrorMessage = "MST not found (graph might be disconnected).";
                            showMSTError = true;
                            usePrim = false; useKruskal = false; mstEdges.clear();
                         } else {
                            usePrim = true; useKruskal = false; showMSTError = false;
                            mstEdgesDrawn.assign(mstEdges.size(), false);
                            mstNodesDrawn.assign(nodePositions.size(), false);
                            mstEdgeIndex = 0; mstDrawTimer = 0.0f; mstDoneDrawing = false;
                            totalMSTWeight = 0; showWeightInfo = true; isMSTDrawingPaused = false;
                         }
                     }
                 } else if (CheckCollisionPointRec(mousePos, kruskalButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                     if (!nodePositions.empty()) {
                         mstEdges = calculateKruskalMST(edges, nodePositions.size());
                         if (mstEdges.size() < nodePositions.size() - 1 && nodePositions.size() > 1) {
                            mstErrorMessage = "MST not found (graph might be disconnected).";
                            showMSTError = true;
                            usePrim = false; useKruskal = false; mstEdges.clear();
                         } else {
                            usePrim = false; useKruskal = true; showMSTError = false;
                            mstEdgesDrawn.assign(mstEdges.size(), false);
                             mstNodesDrawn.assign(nodePositions.size(), false);
                            mstEdgeIndex = 0; mstDrawTimer = 0.0f; mstDoneDrawing = false;
                            totalMSTWeight = 0; showWeightInfo = true; isMSTDrawingPaused = false;
                         }
                     }
                 } else if (CheckCollisionPointRec(mousePos, backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                     showMSTMenu = false; // Exit MST menu
                     showMSTError = false; // Clear error on exit
                 } else if (showWeightInfo && CheckCollisionPointRec(mousePos, pauseResumeButtonDef) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                       // Note: pauseResumeButtonDef Y position is approximate here, drawMSTInfo calculates actual.
                       // Need to use the actual drawn button rect if available, or recalculate.
                       // Recalculate Y based on drawMSTInfo logic:
                       std::string tempWeightStr = "Total Weight: XXX";
                       Vector2 tempWeightTextSize = MeasureTextEx(GetFontDefault(), tempWeightStr.c_str(), 20, 1);
                       Rectangle tempWeightBox = {pauseResumeButtonDef.x, mstMenuRect.y + 40, tempWeightTextSize.x + 10, tempWeightTextSize.y + 10};
                       Rectangle actualPauseButton = pauseResumeButtonDef;
                       actualPauseButton.y = tempWeightBox.y + tempWeightBox.height + 5;
                       if (CheckCollisionPointRec(mousePos, actualPauseButton)) {
                            isMSTDrawingPaused = !isMSTDrawingPaused;
                       }
                 }
             }
              // Allow clicking outside MST menu to close it? Maybe not, use Back button.
              // else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
              //    showMSTMenu = false; // Click outside closes
              // }
         } // end if showMSTMenu

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
                     isEditingGraph = false; // Exit edit mode
                     currentTool = EditTool::TOOL_NONE;
                     // Optional: Recalculate camera?
                      if (!nodePositions.empty()) {
                          UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 120.0f, editPanelWidth); // Pass UI widths
                      }
                 }

                 // Reset states when changing tools
                 if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                      isEditingWeight = false;
                      isDraggingNode = false;
                      // Keep selectedNodeIndex if switching between ADD_EDGE_START/END? No, reset generally safer.
                      // selectedNodeIndex = -1; // Reset unless specific tool logic needs it
                      // selectedEdgeIndex = -1;
                 }

              } // end if clickedOnEditUI

             // --- Handle Graph Interactions (if not clicking Edit UI) ---
             if (!clickedOnUI) { // Only process graph clicks if no UI was clicked
                  // Handle Weight Input Box Logic
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
                             // selectedNodeIndex = -1;
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
                                 if (clickedEdgeIdx != -1) {
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
                 float drawInterval = speedSlider.getValue(); // Get speed from slider

                 if (mstDrawTimer >= drawInterval) {
                     if (mstEdgeIndex < mstEdges.size()) {
                         mstEdgesDrawn[mstEdgeIndex] = true;
                         // Add weight only when edge is fully drawn
                         totalMSTWeight += mstEdges[mstEdgeIndex].weight;
                         // Mark nodes as drawn
                          if(mstEdges[mstEdgeIndex].from > 0 && mstEdges[mstEdgeIndex].from <= mstNodesDrawn.size())
                               mstNodesDrawn[mstEdges[mstEdgeIndex].from - 1] = true;
                          if(mstEdges[mstEdgeIndex].to > 0 && mstEdges[mstEdgeIndex].to <= mstNodesDrawn.size())
                               mstNodesDrawn[mstEdges[mstEdgeIndex].to - 1] = true;

                         mstEdgeIndex++;
                         mstDrawTimer = 0.0f; // Reset timer for next edge

                         if (mstEdgeIndex == mstEdges.size()) {
                             mstDoneDrawing = true; // Mark as finished
                         }
                     } else {
                          mstDoneDrawing = true; // Should already be set, but just in case
                     }
                 }
             }
         }
          // Update cursor blink timer
          cursorTimer += GetFrameTime();
          if (cursorTimer >= 0.8f) cursorTimer = 0.0f; // Blink interval


        // --- Drawing ---
        BeginDrawing();
        ClearBackground(DARKGRAY); // Use a different background

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

        // --- Draw UI Panels (Outside 2D Mode) ---

        // Draw Left Panel Background
         DrawRectangle(0, 0, 120, screenHeight, Fade(BLACK, 0.5f));
        // Draw Left Panel Buttons
        DrawRectangleRec(createButton, isCreating ? ORANGE : WHITE); DrawText("Create", createButton.x + 10, createButton.y + 7, 20, BLACK);
        DrawRectangleRec(randomButton, WHITE); DrawText("Random", randomButton.x + 10, randomButton.y + 7, 20, BLACK);
        DrawRectangleRec(exampleButton, isShowingExamplesActive ? ORANGE : WHITE); DrawText("Examples", exampleButton.x + 10, exampleButton.y + 7, 20, BLACK);
        DrawRectangleRec(inputButton, isInputActive ? ORANGE : WHITE); DrawText("Input", inputButton.x + 10, inputButton.y + 7, 20, BLACK);
        DrawRectangleRec(fileButton, isFileActive ? ORANGE : WHITE); DrawText("File", fileButton.x + 10, fileButton.y + 7, 20, BLACK);
        DrawRectangleRec(editButton, isEditingGraph ? ORANGE : WHITE); DrawText("Edit", editButton.x + 10, editButton.y + 7, 20, BLACK);
        DrawRectangleRec(mstButton, showMSTMenu ? ORANGE : WHITE); DrawText("MST", mstButton.x + 10, mstButton.y + 7, 20, BLACK);

        // Draw Create Input Fields (if active)
         if (isCreating) {
             DrawText("N:", nodesInputRect.x - 25, nodesInputRect.y + 5, 20, WHITE);
             DrawRectangleRec(nodesInputRect, LIGHTGRAY);
             DrawText(numNodesStr.c_str(), nodesInputRect.x + 5, nodesInputRect.y + 5, 20, BLACK);
             if (nodesFocused && ((int)(GetTime()*2.0f)%2 == 0)) DrawText("|", nodesInputRect.x + 5 + MeasureText(numNodesStr.c_str(), 20), nodesInputRect.y + 5, 20, BLACK); // Cursor

             DrawText("E:", edgesInputRect.x - 25, edgesInputRect.y + 5, 20, WHITE);
             DrawRectangleRec(edgesInputRect, LIGHTGRAY);
             DrawText(numEdgesStr.c_str(), edgesInputRect.x + 5, edgesInputRect.y + 5, 20, BLACK);
              if (edgesFocused && ((int)(GetTime()*2.0f)%2 == 0)) DrawText("|", edgesInputRect.x + 5 + MeasureText(numEdgesStr.c_str(), 20), edgesInputRect.y + 5, 20, BLACK); // Cursor
         }

        // Draw Example Buttons (if active)
         if (showExampleButtons) {
             DrawRectangleRec(k5Button, WHITE); DrawText("K5", k5Button.x + 15, k5Button.y + 7, 20, BLACK);
             DrawRectangleRec(c6Button, WHITE); DrawText("C6", c6Button.x + 15, c6Button.y + 7, 20, BLACK);
             DrawRectangleRec(p4Button, WHITE); DrawText("P4", p4Button.x + 15, p4Button.y + 7, 20, BLACK);
             DrawRectangleRec(s7Button, WHITE); DrawText("S7", s7Button.x + 15, s7Button.y + 7, 20, BLACK);
         }

        // Draw Edit Panel (if active)
         if (isEditingGraph) {
             DrawRectangle(editPanelX, 0, editPanelWidth, screenHeight, Fade(BLACK, 0.7f)); // Panel background
             // Draw buttons with highlight for current tool
             DrawRectangleRec(addVertexButtonRect, (currentTool == EditTool::TOOL_ADD_VERTEX) ? ORANGE : LIGHTGRAY); DrawText("Add Vertex", addVertexButtonRect.x + 10, addVertexButtonRect.y + 7, 20, BLACK);
             DrawRectangleRec(addEdgeButtonRect, (currentTool == EditTool::TOOL_ADD_EDGE_START || currentTool == EditTool::TOOL_ADD_EDGE_END) ? ORANGE : LIGHTGRAY); DrawText("Add Edge", addEdgeButtonRect.x + 10, addEdgeButtonRect.y + 7, 20, BLACK);
             DrawRectangleRec(editWeightButtonRect, (currentTool == EditTool::TOOL_EDIT_WEIGHT) ? ORANGE : LIGHTGRAY); DrawText("Edit Weight", editWeightButtonRect.x + 10, editWeightButtonRect.y + 7, 20, BLACK);
             DrawRectangleRec(moveNodeButtonRect, (currentTool == EditTool::TOOL_MOVE_VERTEX) ? ORANGE : LIGHTGRAY); DrawText("Move Node", moveNodeButtonRect.x+10, moveNodeButtonRect.y+7, 20, BLACK);
             DrawRectangleRec(deleteVertexButtonRect, (currentTool == EditTool::TOOL_DELETE_VERTEX) ? ORANGE : LIGHTGRAY); DrawText("Del Vertex", deleteVertexButtonRect.x+10, deleteVertexButtonRect.y+7, 20, BLACK); // Shorter text
             DrawRectangleRec(deleteEdgeButtonRect, (currentTool == EditTool::TOOL_DELETE_EDGE) ? ORANGE : LIGHTGRAY); DrawText("Delete Edge", deleteEdgeButtonRect.x+10, deleteEdgeButtonRect.y+7, 20, BLACK);
             DrawRectangleRec(doneButtonRect, RAYWHITE); DrawText("Done", doneButtonRect.x + 30, doneButtonRect.y + 7, 20, BLACK);

             // Draw Weight Input Box (if active) - draw last to be on top
             if (isEditingWeight) {
                 DrawRectangleRec(weightInputRect, WHITE);
                 DrawRectangleLinesEx(weightInputRect, 1, BLACK);
                 DrawText(weightInputBuffer.c_str(), weightInputRect.x + 5, weightInputRect.y + 4, 20, BLACK);
                 // Blinking cursor for weight input
                 if (((int)(GetTime() * 2.0f)) % 2 == 0) {
                     DrawText("|", weightInputRect.x + 5 + MeasureText(weightInputBuffer.c_str(), 20), weightInputRect.y + 4, 20, BLACK);
                 }
             }
              // Draw Hint Text at bottom
               const char* hintText = "";
               switch(currentTool){
                   case EditTool::TOOL_ADD_VERTEX: hintText = "Click empty space to add vertex."; break;
                   case EditTool::TOOL_ADD_EDGE_START: hintText = "Click first vertex for edge."; break;
                   case EditTool::TOOL_ADD_EDGE_END: hintText = "Click second vertex for edge."; break;
                   case EditTool::TOOL_EDIT_WEIGHT: hintText = "Click near edge to edit weight."; break;
                   case EditTool::TOOL_MOVE_VERTEX: hintText = "Click and drag a vertex."; break;
                   case EditTool::TOOL_DELETE_VERTEX: hintText = "Click a vertex to delete."; break;
                   case EditTool::TOOL_DELETE_EDGE: hintText = "Click near an edge to delete."; break;
                   default: hintText = "Select a tool from the right panel."; break;
               }
               DrawText(hintText, 130, screenHeight - 30, 20, RAYWHITE); // Position hint right of left panel


         } // end if isEditingGraph


        // Draw Matrix Input Window (if active) - Draw last to be on top of graph
         if (showMatrixInput) {
             DrawRectangleRec(inputWindowRect, LIGHTGRAY);
             DrawRectangleLinesEx(inputWindowRect, 2, BLACK); // Border
             DrawText("Input Adjacency Matrix (Nodes on first line)", inputWindowRect.x + 10, inputWindowRect.y + 10, 20, BLACK);

             // Input Area
             DrawRectangleRec(matrixInputAreaRect, WHITE);
             DrawRectangleLinesEx(matrixInputAreaRect, 1, BLACK);
             // Draw the text within the input area (handle scrolling if text exceeds bounds - complex)
             DrawText(matrixInput.c_str(), matrixInputAreaRect.x + 5, matrixInputAreaRect.y + 5, 20, BLACK);

             // Draw blinking cursor for matrix input
             if (matrixInputFocused && cursorTimer < 0.4f) { // Adjust blink timing
                  Vector2 cursorPos = GetCursorScreenPos(matrixInput, cursorRow, cursorColumn, matrixInputAreaRect, 20);
                 DrawLineV(cursorPos, {cursorPos.x, cursorPos.y + 20}, BLACK); // Draw cursor line
             }


             // Buttons
             DrawRectangleRec(submitMatrixButton, WHITE); DrawText("Submit", submitMatrixButton.x + 10, submitMatrixButton.y + 7, 20, BLACK);
             DrawRectangleRec(closeMatrixButton, WHITE); DrawText("Close", closeMatrixButton.x + 10, closeMatrixButton.y + 7, 20, BLACK);

             // Draw Matrix Input Error Message (if any)
             if (!errorMessageInput.empty()) {
                  DrawText(errorMessageInput.c_str(), inputWindowRect.x + 10, inputWindowRect.y + inputWindowRect.height - 65, 18, RED); // Position error above buttons
             }
         }

        // Draw MST Menu (if active) - Draw last to be on top
         if (showMSTMenu) {
             DrawRectangleRec(mstMenuRect, LIGHTGRAY); // Background
             DrawRectangleLinesEx(mstMenuRect, 2, BLACK); // Border
             DrawText("Minimum Spanning Tree", mstMenuRect.x + 10, mstMenuRect.y + 10, 20, BLACK);

             // Draw the graph area within the MST menu
             Rectangle graphArea = {mstMenuRect.x + 10, mstMenuRect.y + 90, mstMenuRect.width - 20, mstMenuRect.height - 140};
             // DrawRectangleLinesEx(graphArea, 1, BLUE); // Optional: visualize graph area

             // Calculate scaling and offset for MST graph drawing
             int numNodesToDraw = nodePositions.size(); // Use original node count
             std::vector<Vector2> mstDrawNodePositions(numNodesToDraw);
             if (numNodesToDraw > 0) {
                  // Find bounds of original node positions
                  Vector2 minBoundsOrig = nodePositions[0], maxBoundsOrig = nodePositions[0];
                  for(const auto& p : nodePositions) {
                      minBoundsOrig.x = std::min(minBoundsOrig.x, p.x); minBoundsOrig.y = std::min(minBoundsOrig.y, p.y);
                      maxBoundsOrig.x = std::max(maxBoundsOrig.x, p.x); maxBoundsOrig.y = std::max(maxBoundsOrig.y, p.y);
                  }
                  float graphWidthOrig = std::max(1.0f, maxBoundsOrig.x - minBoundsOrig.x); // Avoid zero width
                  float graphHeightOrig = std::max(1.0f, maxBoundsOrig.y - minBoundsOrig.y);
                  Vector2 graphCenterOrig = { minBoundsOrig.x + graphWidthOrig / 2.0f, minBoundsOrig.y + graphHeightOrig / 2.0f };


                  // Calculate scale to fit original graph into graphArea
                  float scaleX = (graphArea.width - 40) / graphWidthOrig; // Add padding inside area
                  float scaleY = (graphArea.height - 40) / graphHeightOrig;
                  float scale = std::min(scaleX, scaleY) * 0.9f; // Apply scaling factor

                  // Calculate offset to center the scaled graph in graphArea
                  Vector2 graphAreaCenter = { graphArea.x + graphArea.width / 2.0f, graphArea.y + graphArea.height / 2.0f };
                  Vector2 offset = { graphAreaCenter.x - graphCenterOrig.x * scale, graphAreaCenter.y - graphCenterOrig.y * scale };


                  // Apply transformation to original positions
                  for(int i=0; i < numNodesToDraw; ++i) {
                      mstDrawNodePositions[i] = { offset.x + nodePositions[i].x * scale, offset.y + nodePositions[i].y * scale };
                  }

                 // --- Draw Edges within MST Menu ---
                  // Draw non-MST edges faded first (if MST is calculated)
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

                 // Draw confirmed MST edges (red)
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

                 // Draw the MST edge currently being animated (blue)
                  if (!mstDoneDrawing && !isMSTDrawingPaused && mstEdgeIndex > 0 && mstEdgeIndex <= mstEdges.size()) {
                      float t = mstDrawTimer / speedSlider.getValue(); // Animation progress (0 to 1)
                       const auto& currentMSTEdge = mstEdges[mstEdgeIndex - 1];
                      int fromIdx = currentMSTEdge.from - 1;
                      int toIdx = currentMSTEdge.to - 1;
                      if (fromIdx >= 0 && fromIdx < numNodesToDraw && toIdx >= 0 && toIdx < numNodesToDraw) {
                           Vector2 startP = mstDrawNodePositions[fromIdx];
                           Vector2 endP = mstDrawNodePositions[toIdx];
                           Vector2 currentP = Vector2Lerp(startP, endP, t); // Animate line drawing
                          DrawLineEx(startP, currentP, 2.5f, SKYBLUE); // Draw partial line
                           // Draw weight only when animation completes? Or show immediately? Show near end.
                           if (t > 0.9f) {
                              drawEdgeWeight(startP, endP, currentMSTEdge.weight);
                           }
                      }
                  }


                 // --- Draw Nodes within MST Menu ---
                  for(int i=0; i < numNodesToDraw; ++i) {
                      // Determine node color based on whether it's part of the drawn MST
                      Color nodeColor = (mstNodesDrawn.empty() || !mstNodesDrawn[i]) ? LIGHTGRAY : ORANGE; // Gray if not yet included
                       // If animation is running, highlight nodes of the edge being drawn?
                       if (!mstDoneDrawing && mstEdgeIndex > 0 && mstEdgeIndex <= mstEdges.size()) {
                            const auto& currentMSTEdge = mstEdges[mstEdgeIndex - 1];
                            if (i == currentMSTEdge.from - 1 || i == currentMSTEdge.to - 1) {
                                nodeColor = SKYBLUE; // Highlight nodes of edge being drawn
                            }
                       }
                      drawNode(mstDrawNodePositions[i], i + 1, nodeColor, 15.0f); // Smaller nodes in MST view
                  }

             } // end if numNodesToDraw > 0


             // Draw MST Menu Buttons and Slider
             DrawRectangleRec(primButton, WHITE); DrawText("Prim", primButton.x + 10, primButton.y + 7, 20, BLACK);
             DrawRectangleRec(kruskalButton, WHITE); DrawText("Kruskal", kruskalButton.x + 10, kruskalButton.y + 7, 20, BLACK);
             DrawRectangleRec(backButton, WHITE); DrawText("Back", backButton.x + 10, backButton.y + 7, 20, BLACK);

             // Draw MST Info (Weight, Pause/Resume) if an algorithm ran
             if (showWeightInfo) {
                  // Need to calculate the actual position for pause/resume button based on drawMSTInfo's logic
                  std::string tempWeightStr = "Total Weight: XXX";
                  Vector2 tempWeightTextSize = MeasureTextEx(GetFontDefault(), tempWeightStr.c_str(), 20, 1);
                  Rectangle tempWeightBox = {pauseResumeButtonDef.x, mstMenuRect.y + 40, tempWeightTextSize.x + 10, tempWeightTextSize.y + 10};
                  Rectangle actualPauseButton = pauseResumeButtonDef;
                  actualPauseButton.y = tempWeightBox.y + tempWeightBox.height + 5;
                  // Now pass the correctly positioned button definition
                 drawMSTInfo(totalMSTWeight, isMSTDrawingPaused, actualPauseButton, mstMenuRect);
             }

             speedSlider.draw(); // Draw slider

             // Draw MST Error Message (if any)
             if (showMSTError) {
                  DrawText(mstErrorMessage.c_str(), mstMenuRect.x + 10, mstButtonsY - 25, 18, RED); // Position above buttons
             }

         } // end if showMSTMenu


        // Draw General/File Error Messages (if any) - Draw last over UI elements
         if (showError) {
             DrawText(errorMessage.c_str(), 130, screenHeight - 60, 20, RED); // Position near bottom-left
         }
         if (showFileError) {
              DrawText(fileErrorMessage.c_str(), 130, screenHeight - 60, 20, RED); // Position near bottom-left
         }


        EndDrawing();
    } // End main game loop

    CloseWindow();
    return 0;
}

void ResetStates(bool &isCreating, bool &isRandomizing, bool &isShowingExamples,
    bool &showMatrixInput, bool &graphDrawn,
    std::string &numNodesStr, std::string &numEdgesStr, std::string &matrixInput,
    bool &nodesFocused, bool &edgesFocused,
    bool &showMSTMenu, bool &isEditingGraph, EditTool& currentTool,
    bool& showFileError, std::string& fileErrorMessage,
    bool& showError, std::string& errorMessage,
    bool& showMSTError, std::string& mstErrorMessage)
    {
// Reset graph creation/generation states
       isCreating = false;
       isRandomizing = false;
       isShowingExamples = false;
       showMatrixInput = false;
       graphDrawn = false; // Usually reset when starting a new graph action

// Reset input fields
       numNodesStr = "";
       numEdgesStr = "";
       matrixInput = ""; // Reset matrix input field
       nodesFocused = false;
       edgesFocused = false;

// Reset UI states
       showMSTMenu = false;
       isEditingGraph = false;
       currentTool = EditTool::TOOL_NONE;

// Reset error messages and flags
       showFileError = false;
       fileErrorMessage = "";
       showError = false; // General/Create error
       errorMessage = "";
       showMSTError = false;
       mstErrorMessage = "";
// errorMessageInput is reset within Input mode usually

// Note: Consider if edges and nodePositions should be cleared here or
//       in the specific functions that initiate new graph creation/loading.
//       Clearing them here makes ResetStates more destructive.
// edges.clear();       // Example: If needed globally
// nodePositions.clear(); // Example: If needed globally
}
// using namespace std;

// bool isCreatingActive = false;
// bool isRandomizingActive = false;
// bool isShowingExamplesActive = false;
// bool isInputActive = false;
// bool isFileActive = false;

// struct Edge {
//     int from;
//     int to;
//     int weight;

//     bool operator<(const Edge& other) const {
//         return weight < other.weight;
//     }
// };

// class Slider {
//     public:
//         Rectangle rect;         // Vùng thanh trượt
//         Rectangle knob;         // Núm trượt
//         float value;            // Giá trị hiện tại (0.0 - 1.0)
//         float minValue;         // Giá trị nhỏ nhất
//         float maxValue;         // Giá trị lớn nhất
//         bool isDragging;       // Đang kéo
//         std::string label;       // Nhãn
//         bool editValue;       // Đang chỉnh sửa giá trị trực tiếp
//         std::string editBuffer; // Buffer cho chỉnh sửa
    
//         Slider(Rectangle r, float minVal, float maxVal, std::string l)
//             : rect(r), minValue(minVal), maxValue(maxVal), value(0.5f),
//               isDragging(false), label(l), editValue(false), editBuffer("") {
//             knob = {rect.x, rect.y - 5, 15, rect.height + 10};
//         }
    
//         void update() {
//             // Xử lý sự kiện kéo
//             if (CheckCollisionPointRec(GetMousePosition(),knob) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 isDragging = true;
//             }
    
//             if (isDragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
//                 knob.x = GetMouseX() - knob.width / 2;
//                 knob.x = std::clamp(knob.x, rect.x, rect.x + rect.width - knob.width);
//                 value = (float)(knob.x - rect.x) / (rect.width - knob.width);
//             }
    
//             if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
//                 isDragging = false;
//             }
//         }
    
//         float getValue() const {
//             return minValue + (maxValue - minValue) * value;
//         }
    
//         void draw() {
//             // Vẽ nhãn
//             DrawText(label.c_str(), rect.x, rect.y - 20, 16, RAYWHITE);
    
//             // Vẽ thanh trượt
//             DrawRectangleRec(rect, LIGHTGRAY);
//             DrawRectangleRec(knob, RED);
    
//             // Vẽ giá trị hiện tại
//             DrawText(TextFormat("%.2f", getValue()), rect.x + rect.width + 10, rect.y, 16, RAYWHITE);
    
//             // Vẽ vùng chỉnh sửa giá trị (nếu được chọn)
//             if (editValue) {
//                 DrawRectangle(rect.x, rect.y + rect.height + 5, 100, 20, LIGHTGRAY);
//                 DrawText(editBuffer.c_str(), rect.x + 5, rect.y + rect.height + 8, 16, BLACK);
//             }
//         }
    
//         bool isHovered() const {
//             return CheckCollisionPointRec(GetMousePosition(),rect);
//         }
    
//         bool isEditing() const {
//             return editValue;
//         }
    
//         void startEditing() {
//             editValue = true;
//             editBuffer = TextFormat("%.2f", getValue());
//         }
    
//         void stopEditing() {
//             editValue = false;
//             try {
//                 value = std::stof(editBuffer);
//                 value = (value - minValue) / (maxValue - minValue);
//                 value = std::clamp(value, 0.0f, 1.0f);
//                 knob.x = rect.x + (rect.width - knob.width) * value;
//             } catch (...) {
//                 // Xử lý lỗi nếu đầu vào không hợp lệ
//                 editBuffer = TextFormat("%.2f", getValue()); // Khôi phục giá trị cũ
//             }
//         }
    
//         std::string getLabel() const {
//             return label;
//         }
// };

// bool compareEdge(const std::pair<int, Edge>& a, const std::pair<int, Edge>& b) {
//     return a.first > b.first; // So sánh theo trọng số (a.first, b.first)
// }

// bool edgeExists(const std::vector<Edge>& edges, int from, int to) {
//     for (const auto& edge : edges) {
//         if ((edge.from == from && edge.to == to) || (edge.from == to && edge.to == from)) {
//             return true;
//         }
//     }
//     return false;
// }

// // Hàm kiểm tra tính liên thông của đồ thị bằng DFS
// bool isGraphConnected(const std::vector<Edge>& edges, int numNodes) {
//     if (numNodes <= 1) return true; // Đồ thị có 0 hoặc 1 node luôn liên thông

//     std::vector<bool> visited(numNodes, false);
//     std::vector<int> stack;

//     // Chọn node đầu tiên làm điểm bắt đầu DFS
//     stack.push_back(0); // Chọn node 0 làm gốc (đã hiệu chỉnh để node bắt đầu từ 1)
//     visited[0] = true;

//     while (!stack.empty()) {
//         int currentNode = stack.back();
//         stack.pop_back();

//         for (const auto& edge : edges) {
//             int from = edge.from - 1; // Hiệu chỉnh chỉ số node
//             int to = edge.to - 1;     // Hiệu chỉnh chỉ số node

//             if (from == currentNode && !visited[to]) {
//                 visited[to] = true;
//                 stack.push_back(to);
//             } else if (to == currentNode && !visited[from]) {
//                 visited[from] = true;
//                 stack.push_back(from);
//             }
//         }
//     }

//     // Kiểm tra xem tất cả các node đã được thăm hay chưa
//     for (bool v : visited) {
//         if (!v) return false;
//     }

//     return true;
// }

// void generateCompleteGraph(std::vector<Edge>& edges, int numNodes) {
//     edges.clear();
//     for (int i = 1; i <= numNodes; ++i) {
//         for (int j = i + 1; j <= numNodes; ++j) {
//             edges.push_back({ i, j, 1 }); // Weight = 1 for simplicity
//         }
//     }
// }

// void generateCycleGraph(std::vector<Edge>& edges, int numNodes) {
//     edges.clear();
//     for (int i = 1; i < numNodes; ++i) {
//         edges.push_back({ i, i + 1, 1 });
//     }
//     edges.push_back({ numNodes, 1, 1 });
// }

// void generatePathGraph(std::vector<Edge>& edges, int numNodes, std::vector<Vector2>& nodePositions, int screenWidth, int screenHeight) {
//     edges.clear();
//     nodePositions.resize(numNodes);

//     float spacing = 150.0f; // Khoảng cách giữa các node
//     float startX = (screenWidth - (numNodes - 1) * spacing) / 2.0f;
//     float centerY = screenHeight / 2.0f;

//     for (int i = 0; i < numNodes; ++i) {
//         nodePositions[i] = { startX + i * spacing, centerY };
//         if (i < numNodes - 1) {
//             edges.push_back({ i + 1, i + 2, 1 });
//         }
//     }
// }

// void generateStarGraph(std::vector<Edge>& edges, int numNodes) {
//     edges.clear();
//     for (int i = 2; i <= numNodes; ++i) {
//         edges.push_back({ 1, i, 1 }); // Node 1 is the center
//     }
// }

// void generateRandomGraph(std::vector<Edge>& edges, int numNodes, int numEdges) {
//     edges.clear();
//     srand(time(0));

//     // Tạo một đồ thị đường (path graph) để đảm bảo tính liên thông cơ bản
//     for (int i = 1; i < numNodes; ++i) {
//         edges.push_back({ i, i + 1, GetRandomValue(1, 10) });
//     }

//     // Thêm các cạnh ngẫu nhiên cho đến khi đạt đủ số lượng cạnh
//     while (edges.size() < numEdges && edges.size() < (long unsigned int) (numNodes * (numNodes - 1) / 2)) {
//         int from = GetRandomValue(1, numNodes);
//         int to = GetRandomValue(1, numNodes);
//         int weight = GetRandomValue(1, 10);
//         if (from != to && !edgeExists(edges, from, to)) {
//             edges.push_back({ from, to, weight });
//         }
//     }
//     // kiểm tra số cạnh tạo ra để tránh vòng lặp vô hạn.
//     // kiểm tra tính liên thông sau khi thêm đủ số edge.
//     if (!isGraphConnected(edges,numNodes)){
//         edges.clear();
//         for (int i = 1; i < numNodes; ++i) {
//             edges.push_back({ i, i + 1, GetRandomValue(1, 10) });
//         }
//         for (int i = 1; i < numNodes /2; ++i) {
//              edges.push_back({ 1, i+2 , GetRandomValue(1, 10) });
//         }
//     }
// }

// // Hàm để lấy vị trí index trong chuỗi dựa trên dòng và cột
// int GetCursorIndex(const std::string& text, int row, int col) {
//     int currentRow = 0;
//     int currentCol = 0;
//     for (int i = 0; i < text.length(); ++i) {
//         if (currentRow == row && currentCol == col) {
//             return i;
//         }
//         if (text[i] == '\n') {
//             currentRow++;
//             currentCol = 0;
//         } else {
//             currentCol++;
//         }
//     }
//     return text.length(); // Nếu vị trí vượt quá độ dài chuỗi
// }

// // Hàm lấy cột từ vị trí index trong chuỗi
// int GetColFromIndex(const std::string& text, int index) {
//     int col = 0;
//     for (int i = index-1; i >=0; --i) {
//         if (text[i] == '\n') {
//             break;
//         }
//         col++;
//     }
//     return col;
// }

// // Hàm tính toán vị trí con trỏ trên màn hình dựa trên dòng và cột
// Vector2 GetCursorScreenPos(const std::string& text, int row, int col, Rectangle rect) {
//     int currentRow = 0;
//     int currentCol = 0;
//     int x = rect.x + 5;
//     int y = rect.y + 5;
//     for (int i = 0; i < text.length(); ++i) {
//         if (currentRow == row && currentCol == col) {
//             return {static_cast<float>(x), static_cast<float>(y)};
//         }
//         if (text[i] == '\n') {
//             y += 20;
//             x = rect.x + 5;
//             currentRow++;
//             currentCol = 0;
//         } else {
//             x += MeasureText(text.substr(i, 1).c_str(), 20);
//             currentCol++;
//         }
//     }
//     return {static_cast<float>(x), static_cast<float>(y)};
// }

// std::string ValidateMatrixInput(const std::string& input, std::vector<std::vector<int>>& adjacencyMatrix, int& numNodes) {
//     adjacencyMatrix.clear();
//     std::stringstream ss(input);
//     std::string line;

//     // Đọc số node từ dòng đầu tiên
//     if (!std::getline(ss, line)) {
//         return "Lỗi: Không có dữ liệu đầu vào.";
//     }
//     try {
//         numNodes = std::stoi(line);
//     } catch (const std::invalid_argument& e) {
//         return "Lỗi: Dòng đầu tiên không phải là số hợp lệ.";
//     }

//     if (numNodes <= 0) {
//         return "Lỗi: Số node phải lớn hơn 0.";
//     }

//     int rowCount = 0;
//     int colCount = -1;

//     // Đọc ma trận từ các dòng còn lại
//     while (std::getline(ss, line)) {
//         std::vector<int> row;
//         std::stringstream lineStream(line);
//         int value;
//         char comma;
//         int currentRowColCount = 0;
//         while (lineStream >> value) {
//             row.push_back(value);
//             if (lineStream.peek() == ',' || lineStream.peek() == ' ') {
//                 lineStream >> comma;
//             }
//             currentRowColCount++;
//         }
//         if (colCount != -1 && colCount != currentRowColCount) {
//             return "Lỗi: Số cột không đồng nhất ở dòng " + std::to_string(rowCount + 2) + "."; // Cộng 2 vì dòng đầu tiên là số node
//         }
//         colCount = currentRowColCount;
//         adjacencyMatrix.push_back(row);
//         rowCount++;
//     }

//     // Kiểm tra ma trận vuông
//     if (rowCount != numNodes || colCount != numNodes) {
//         return "Lỗi: Ma trận không vuông hoặc kích thước không khớp với số node.";
//     }

//     return ""; // Trả về chuỗi rỗng nếu ma trận hợp lệ
// }

// void ResetStates(bool &isCreating, bool &isRandomizing, bool &isShowingExamples, bool &showMatrixInput, bool &graphDrawn, std::string &numNodesStr, std::string &numEdgesStr, std::string &matrixInput, bool &nodesFocused, bool &edgesFocused) {
//     graphDrawn = false;
//     isCreating = false;
//     isRandomizing = false;
//     isShowingExamples = false;
//     showMatrixInput = false;
//     numNodesStr = "";
//     numEdgesStr = "";
//     matrixInput = " ";
//     nodesFocused = false;
//     edgesFocused = false;
// }

// std::vector<Edge> calculatePrimMST(const std::vector<Edge>& edges, int numNodes) {
//     std::vector<Edge> mstEdges; // Cạnh của cây MST
//     std::vector<bool> visited(numNodes, false); // Đánh dấu đỉnh đã thăm
//     std::priority_queue<std::pair<int, Edge>, std::vector<std::pair<int, Edge>>, bool(*)(const std::pair<int, Edge>&, const std::pair<int, Edge>&)> pq(compareEdge); // Hàng đợi ưu tiên, sử dụng compareEdge

//     // Chọn đỉnh bắt đầu (ví dụ: đỉnh 0)
//     visited[0] = true;

//     // Thêm các cạnh kề đỉnh bắt đầu vào hàng đợi ưu tiên
//     for (const auto& edge : edges) {
//         if (edge.from == 1 && !visited[edge.to - 1]) { // Đã sửa chỉ số
//             pq.push({edge.weight, edge});
//         } else if (edge.to == 1 && !visited[edge.from - 1]) {
//             pq.push({edge.weight, edge});
//         }
//     }

//     while (!pq.empty() && mstEdges.size() < numNodes - 1) {
//         int weight = pq.top().first;
//         Edge currentEdge = pq.top().second;
//         pq.pop();

//         int from = currentEdge.from - 1; // Sửa chỉ số
//         int to = currentEdge.to - 1;     // Sửa chỉ số

//         if (visited[from] && visited[to]) {
//             continue; // Cả hai đỉnh đã được thăm
//         }

//         mstEdges.push_back(currentEdge);

//         int nextNode = visited[from] ? to : from;
//         visited[nextNode] = true;

//         for (const auto& edge : edges) {
//             if (edge.from == nextNode + 1 && !visited[edge.to - 1]) {
//                 pq.push({edge.weight, edge});
//             } else if (edge.to == nextNode + 1 && !visited[edge.from - 1]) {
//                 pq.push({edge.weight, edge});
//             }
//         }
//     }

//     return mstEdges;
// }

// // Hàm tìm tập hợp (find) với Union-Find
// int findSet(std::vector<int>& parent, int i) {
//     if (parent[i] == i)
//         return i;
//     return parent[i] = findSet(parent, parent[i]);
// }

// // Hàm hợp nhất hai tập hợp (union) với Union-Find
// void unionSets(std::vector<int>& parent, int a, int b) {
//     int rootA = findSet(parent, a);
//     int rootB = findSet(parent, b);
//     if (rootA != rootB)
//         parent[rootA] = rootB;
// }

// std::vector<Edge> calculateKruskalMST(const std::vector<Edge>& edges, int numNodes) {
//     std::vector<Edge> mstEdges;
//     std::vector<int> parent(numNodes);
//     for (int i = 0; i < numNodes; ++i)
//         parent[i] = i; // Khởi tạo mỗi nút là một tập hợp riêng

//     std::vector<Edge> sortedEdges = edges;
//     std::sort(sortedEdges.begin(), sortedEdges.end()); // Sắp xếp các cạnh theo trọng số tăng dần

//     for (const auto& edge : sortedEdges) {
//         int from = edge.from - 1;
//         int to = edge.to - 1;
//         if (findSet(parent, from) != findSet(parent, to)) {
//             mstEdges.push_back(edge);
//             unionSets(parent, from, to);
//         }
//     }

//     return mstEdges;
// }

// // Hàm tính toán vị trí node theo hình tròn với độ lệch ngẫu nhiên
// void positionNodesInCircle(std::vector<Vector2>& nodePositions, int numNodes, float centerX, float centerY,float radius) {
//     nodePositions.resize(numNodes);
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<> distRadius(0.9f, 1.1f); // Thay đổi bán kính
//     std::uniform_real_distribution<> distAngle(-0.1f, 0.1f);  // Thay đổi góc

//    for (int i = 0; i < numNodes; ++i) {
//        float angle = 2.0f * PI * i / numNodes + distAngle(gen);
//        float randomRadius = radius * distRadius(gen);
//        nodePositions[i] = {centerX + randomRadius * cosf(angle),centerY + randomRadius * sinf(angle)};
//     }
// }

// // Hàm vẽ node đẹp hơn
// void drawNode(Vector2 position, int id, Color color, float radius) {
//     DrawCircleV(position, radius, color);
//     DrawCircleV(position, radius - 2, WHITE);
//     DrawText(TextFormat("%d", id), position.x - MeasureText(TextFormat("%d", id), 20) / 2, position.y - 10, 20,
//              DARKGRAY);
// }

// // Hàm vẽ cạnh cong (Bezier)
// void drawBezierEdge(Vector2 start, Vector2 end, float thickness, Color color) {
//     DrawLineEx(start, end, thickness, color);
// }

// //Hàm vẽ trọng số cạnh ở giữa và đẹp hơn
// void drawEdgeWeight(Vector2 start, Vector2 end, int weight) {
//     Vector2 midPoint = { (start.x + end.x) / 2.0f, (start.y + end.y) / 2.0f };
//     std::string weightText = std::to_string(weight);
//     int fontSize = 14;
//     Vector2 textSize = { (float)MeasureText(weightText.c_str(), fontSize), (float)fontSize };
//     Vector2 textPosition = { midPoint.x - textSize.x / 2.0f, midPoint.y - textSize.y / 2.0f };

//     DrawRectangle(textPosition.x - 2, textPosition.y - 2, textSize.x + 4, textSize.y + 4, WHITE); // Background
//     DrawText(weightText.c_str(), (int)textPosition.x, (int)textPosition.y, fontSize, BLACK);
// }

// void drawMSTInfo(int totalWeight, bool isMSTDrawingPaused,Rectangle pauseResumeButton,Rectangle menuRect) {
//     float textX = menuRect.x + 10;
//     float textY = menuRect.y + 10;
//     int fontSize = 20;
//     int textHeight = fontSize;
//     Rectangle weightBox = {menuRect.x + 10, textY + textHeight + 5, 150, 30};
//     DrawRectangleRec(weightBox, WHITE);
//     DrawText(TextFormat("Total Weight: %d", totalWeight), weightBox.x + 5, weightBox.y + 5, 20, BLACK);

//     // Vị trí nút Pause/Resume (đặt dưới Total Weight)
//     pauseResumeButton.y = weightBox.y + weightBox.height + 5;
//     DrawRectangleRec(pauseResumeButton, WHITE);
//     DrawText(isMSTDrawingPaused ? "Resume" : "Pause", pauseResumeButton.x + 10, pauseResumeButton.y + 10, 20, BLACK);
// }

// std::string ReadTextFile(const char *filePath) {
//     std::ifstream fileStream(filePath);
//     if (!fileStream.is_open()) {
//         std::cerr << "Error: Could not open file: " << filePath << std::endl;
//         return ""; // Trả về chuỗi rỗng nếu lỗi
//     }
//     std::stringstream buffer;
//     buffer << fileStream.rdbuf();
//     fileStream.close();
//     return buffer.str();
// }

// bool LoadGraphFromFile(const char* filePath, std::vector<Edge>& outEdges, std::vector<Vector2>& outNodePositions, int screenWidth, int screenHeight, std::string& outErrorMessage) {
//     outEdges.clear();
//     outNodePositions.clear();
//     outErrorMessage = "";

//     std::string fileContent = ReadTextFile(filePath);
//     if (fileContent.empty()) {
//         outErrorMessage = "Error: File is empty or could not be read.";
//         return false;
//     }

//     std::vector<std::vector<int>> adjacencyMatrix;
//     int numNodes;
//     outErrorMessage = ValidateMatrixInput(fileContent, adjacencyMatrix, numNodes); // Tái sử dụng hàm validate

//     if (!outErrorMessage.empty()) {
//         // Thêm thông tin file vào thông báo lỗi
//         outErrorMessage = "Error in file '" + std::string(filePath) + "': " + outErrorMessage;
//         return false; // Ma trận không hợp lệ
//     }

//     // Ma trận hợp lệ, tạo danh sách cạnh
//     for (int i = 0; i < numNodes; ++i) {
//         for (int j = i + 1; j < numNodes; ++j) {
//             if (adjacencyMatrix[i][j] != 0) {
//                 // Đảm bảo trọng số dương nếu cần, hoặc xử lý trọng số 0/âm tùy yêu cầu
//                 int weight = adjacencyMatrix[i][j];
//                 if (weight <= 0) weight = 1; // Ví dụ: đặt trọng số mặc định là 1 nếu <= 0
//                 outEdges.push_back({ i + 1, j + 1, weight });
//             }
//         }
//     }

//     // Kiểm tra tính liên thông sau khi tạo cạnh từ file
//     if (numNodes > 0 && !isGraphConnected(outEdges, numNodes)) {
//          outErrorMessage = "Warning: Graph loaded from file is not connected.";
//          return false; // Nếu muốn coi đây là lỗi
//     }


//     // Tính toán vị trí nút (ví dụ: xếp vòng tròn)
//     positionNodesInCircle(outNodePositions, numNodes, screenWidth / 2.0f, screenHeight / 2.0f, std::min(screenWidth, screenHeight) / 3.0f); // Tăng bán kính một chút

//     return true; // Tải thành công
// }

// void UpdateGraphCamera(Camera2D &camera, const std::vector<Vector2>& nodes, float nodeRadius, int screenWidth, int screenHeight) {
//     if (nodes.empty()) {
//         // Reset camera về mặc định nếu không có nút nào
//         camera.target = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
//         camera.offset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
//         camera.zoom = 1.0f;
//         return;
//     }

//     // 1. Xác định Viewport (vùng vẽ đồ thị)
//     // CẬP NHẬT: Đảm bảo chiều rộng UI đúng với code của bạn
//     float uiLeftWidth = 120.0f; // Chiều rộng UI bên trái (Create, Random...)
//     float uiRightWidth = 130.0f; // Chiều rộng UI Edit bên phải (Add Vertex...)
//     float padding = 50.0f;  // <<-- CÓ THỂ TĂNG padding LÊN một chút (ví dụ 50 hoặc 60)

//     Rectangle viewport = {
//         uiLeftWidth + padding,
//         padding,
//         // Chiều rộng = Tổng rộng màn hình - UI trái - UI phải - 2 * padding
//         screenWidth - uiLeftWidth - uiRightWidth - 2 * padding,
//         screenHeight - 2 * padding
//     };

//     // Đảm bảo viewport có kích thước hợp lệ
//     if (viewport.width <= 1.0f) viewport.width = 1.0f; // Phải lớn hơn 0
//     if (viewport.height <= 1.0f) viewport.height = 1.0f;


//     // 2. Tính Bounding Box của đồ thị (bao gồm cả bán kính nút)
//     Vector2 minBounds = nodes[0];
//     Vector2 maxBounds = nodes[0];

//     for (const auto& nodePos : nodes) {
//         minBounds.x = std::min(minBounds.x, nodePos.x);
//         minBounds.y = std::min(minBounds.y, nodePos.y);
//         maxBounds.x = std::max(maxBounds.x, nodePos.x);
//         maxBounds.y = std::max(maxBounds.y, nodePos.y);
//     }

//     // Thêm bán kính nút vào bounding box
//     minBounds.x -= nodeRadius;
//     minBounds.y -= nodeRadius;
//     maxBounds.x += nodeRadius;
//     maxBounds.y += nodeRadius;

//     float graphWidth = maxBounds.x - minBounds.x;
//     float graphHeight = maxBounds.y - minBounds.y;

//     // Xử lý trường hợp đồ thị chỉ có 1 điểm hoặc các điểm trùng nhau
//     // Đặt kích thước tối thiểu để tránh scale quá lớn
//     if (graphWidth <= nodeRadius) graphWidth = nodeRadius * 2;
//     if (graphHeight <= nodeRadius) graphHeight = nodeRadius * 2;

//     // 3. Tính Scale và Offset

//     // >> THAY ĐỔI TÍNH TOÁN SCALE <<
//     // Đặt mục tiêu đồ thị chiếm khoảng 85% không gian viewport
//     float targetViewportRatio = 0.85f;
//     float targetViewportWidth = viewport.width * targetViewportRatio;
//     float targetViewportHeight = viewport.height * targetViewportRatio;

//     float scaleX = (graphWidth > 0) ? (targetViewportWidth / graphWidth) : 1.0f; // Tránh chia cho 0
//     float scaleY = (graphHeight > 0) ? (targetViewportHeight / graphHeight) : 1.0f;
//     float scale = std::min(scaleX, scaleY); // Lấy tỷ lệ nhỏ hơn để đảm bảo vừa cả 2 chiều

//     // >> THÊM GIỚI HẠN ZOOM TỐI ĐA <<
//     // Ngăn không cho zoom quá gần vào đồ thị nhỏ ban đầu
//     const float maxZoom = 1.0f; // Đặt giới hạn zoom tối đa là 1.0 (hoặc 0.9 nếu muốn nhỏ hơn nữa)
//     if (scale > maxZoom) {
//         scale = maxZoom;
//     }

//     // Giới hạn scale tối thiểu để tránh quá nhỏ
//     const float minZoom = 0.1f;
//     if (scale < minZoom) {
//          scale = minZoom;
//     }

//     // Tính tâm của đồ thị gốc
//     Vector2 graphCenter = {
//         minBounds.x + graphWidth / 2.0f,
//         minBounds.y + graphHeight / 2.0f
//     };

//     // Tính tâm của viewport (vẫn dùng tâm của viewport đầy đủ để căn giữa)
//     Vector2 viewportCenter = {
//         viewport.x + viewport.width / 2.0f,
//         viewport.y + viewport.height / 2.0f
//     };

//     // 4. Cập nhật Camera
//     camera.target = graphCenter;    // Camera nhìn vào tâm đồ thị gốc
//     camera.offset = viewportCenter; // Tâm đồ thị gốc sẽ được chiếu vào tâm viewport
//     camera.zoom = scale;            // Áp dụng tỷ lệ thu phóng đã tính toán và giới hạn
// }

// enum EditTool {
//     TOOL_NONE,
//     TOOL_ADD_VERTEX,
//     TOOL_ADD_EDGE_START, // Bước 1: Chọn đỉnh bắt đầu
//     TOOL_ADD_EDGE_END,   // Bước 2: Chọn đỉnh kết thúc
//     TOOL_EDIT_WEIGHT,
//     TOOL_MOVE_VERTEX, // Tùy chọn: Thêm công cụ di chuyển
//     TOOL_DELETE_VERTEX,
//     TOOL_DELETE_EDGE
// };

// int main() {
//     const int screenWidth = 1400;
//     const int screenHeight = 1000;
    
//     InitWindow(screenWidth, screenHeight, "Graph Creator");
//     SetTargetFPS(60);
 
//     Rectangle createButton = {10, 10, 100, 30};
//     Rectangle randomButton = {10, createButton.y + createButton.height + 10, 100, 30};
//     Rectangle exampleButton = {10, randomButton.y + randomButton.height + 10, 100, 30};
//     Rectangle inputButton = {10, exampleButton.y + exampleButton.height + 10, 100, 30};
//     Rectangle fileButton = {10, inputButton.y + inputButton.height + 10, 100, 30}; // Nút File

//     int exampleNumNodes = 0; // Biến lưu trữ số lượng node cho đồ thị mẫu
//     int exampleNumEdges = 0; // Biến lưu trữ số lượng edge cho đồ thị mẫu

//     bool createButtonClicked = false;
//     bool exampleButtonClicked = false;
//     bool showExampleButtons = false; // Flag to show example graph buttons
//     bool randomButtonClicked = false; // Biến flag cho nút random

//     // Các biến cờ trạng thái cho từng chức năng
//     bool isCreating = false;
//     bool isRandomizing = false;
//     bool isShowingExamples = false;

//     bool inputMode = false;
//     std::string numNodesStr = "";
//     std::string numEdgesStr = "";
//     std::string errorMessage = "";
//     std::vector<Edge> edges;
//     bool graphDrawn = false;
//     std::vector<Vector2> nodePositions;

//     Rectangle nodesInput = { createButton.x + createButton.width + 30, 10, 100, 30 };
//     Rectangle edgesInput = { nodesInput.x + nodesInput.width + 30, 10, 100, 30 };
//     bool nodesFocused = false;
//     bool edgesFocused = false;
//     bool canCreateGraph = true;
//     bool showError = false;

//    // Thay đổi vị trí và kích thước nút
//    Rectangle k5Button = { randomButton.x + randomButton.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Complete Graph (K5)
//    Rectangle c6Button = { k5Button.x + k5Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Cycle Graph (C6)
//    Rectangle p4Button = { c6Button.x + c6Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Path Graph (P4)
//    Rectangle s7Button = { p4Button.x + p4Button.width + 10, randomButton.y + randomButton.height + 10, 60, 30 }; // Star Graph (S7)
  
//    // Input
//    bool matrixInputFocused = false;
//    bool showMatrixInput = false;
//    std::string matrixInput = " ";
//    int cursorColumn = 0;
//    int cursorRow = 0; 
//    float cursorTimer = 0.0f; // Thời gian nhấp nháy con trỏ
//    std::string errorMessageInput = ""; // Biến để lưu thông báo lỗi

//    Rectangle inputWindow = {screenWidth / 4, screenHeight / 4, screenWidth / 2, screenHeight / 2};
//    Rectangle matrixInputRect = {inputWindow.x + 10, inputWindow.y + 40, inputWindow.width - 20, inputWindow.height - 90};
//    Rectangle submitButton = {inputWindow.x + 10, inputWindow.y + inputWindow.height - 40, 80, 30}; // Nút Submit
//    Rectangle closeButton = {submitButton.x + submitButton.width + 10, submitButton.y, 80, 30}; // Nút Back
  
//    // MST
//    Rectangle editButton = {10, fileButton.y + fileButton.height + 10, 100, 30};
//    Rectangle mstButton = {10, editButton.y + editButton.height + 10, 100, 30}; 
//    bool mstButtonClicked = false;
//    bool showMSTMenu = false;
//    Rectangle mstMenuRect = {screenWidth / 8, screenHeight / 8, screenWidth * 3 / 4, screenHeight * 3 / 4};
//    const int buttonWidth = 80;
//    const int buttonHeight = 30;
//    const int buttonSpacing = 10;
//    const float buttonsY = mstMenuRect.y + mstMenuRect.height - buttonHeight - 10; // Đặt các nút ở đáy menu   
//    Rectangle primButton = {mstMenuRect.x+10, buttonsY, buttonWidth, buttonHeight};
//    Rectangle kruskalButton = {primButton.x+buttonWidth+buttonSpacing, buttonsY, buttonWidth, buttonHeight};
//    Rectangle backButton = {kruskalButton.x+buttonWidth+buttonSpacing, buttonsY, buttonWidth, buttonHeight};
//    Rectangle pauseResumeButton = {mstMenuRect.x + 10, mstMenuRect.y + 75, 80, 30}; 
//    bool usePrim = false;
//    bool useKruskal = false;
//    bool showMSTError = false; // Theo dõi xem có hiển thị lỗi hay không
//    std::string mstErrorMessage = ""; // Chuỗi để lưu thông báo lỗi
//    bool showGraph = true; 
//    std::vector<Edge> mstEdges;
//    std::vector<bool> mstEdgesDrawn;
//    std::vector<Edge> mstNodesDrawn;
//    int mstEdgeIndex = 0;
//    int totalMSTWeight = 0;
//    float mstDrawTimer = 0.0f;
//    bool mstDoneDrawing = false;
//    bool drawMSTOnMSTMenu = false;
//    int numNodesMST = 0;
//    bool showWeightInfo = false;
//    bool isMSTDrawingPaused = false; // Trạng thái tạm dừng
//    Rectangle graphBoundingBox = {mstMenuRect.x + 200, mstMenuRect.y + 10, mstMenuRect.width - 210, mstMenuRect.height - 80};
//    float graphPadding = 20.0f;
//    Slider speedSlider = Slider({mstMenuRect.x + mstMenuRect.width - 160, mstMenuRect.y + (mstMenuRect.height - 70) / 2, 150, 20}, 0.1f, 2.0f, "Speed");
//    Vector2 graphOriginalSize = {0, 0};

//    // File
//    std::string fileErrorMessage = ""; // Biến lưu lỗi khi đọc file
//    bool showFileError = false;      // Cờ hiển thị lỗi file
  
//    // Edit
//    bool isEditingGraph = false;
//    bool isEditingWeight = false;
//    std::string weightInputBuffer = "";
//    Rectangle weightInputRect = {0, 0, 60, 25}; // Kích thước ô nhập
//    Vector2 weightInputPosition = {0, 0};     // Vị trí ô nhập
//    EditTool currentTool = TOOL_NONE;
//    int selectedNodeIndex = -1;     // Lưu chỉ số (index) của node được chọn (cho Add Edge, Move)
//    int selectedEdgeIndex = -1;     // Lưu chỉ số (index) của cạnh được chọn (cho Edit Weight)
//    bool isDraggingNode = false;    // Cờ cho việc di chuyển node

//     Camera2D graphCamera = { 0 };
//     graphCamera.rotation = 0.0f;
//     graphCamera.zoom = 1.0f; // Zoom mặc định
//     while (!WindowShouldClose()) {
//          Vector2 screenMousePosOnClick = GetMousePosition();
//          Vector2 worldMousePos = GetScreenToWorld2D(GetMousePosition(), graphCamera); // <<--- Lấy tọa độ chuột trong thế giới đồ thị
//         if (isEditingGraph) {
//             float uiLeftWidth = 120.0f;
//             float uiRightWidth = 130.0f;
//             Rectangle leftPanelRect = { 0, 0, uiLeftWidth, (float)screenHeight }; // Vùng UI bên trái
//             Rectangle editPanelRect = { screenWidth, 0, uiRightWidth, (float)screenHeight }; // Vùng UI bên phải (bao gồm các nút edit)
//             Rectangle addVertexButtonRect = { screenWidth - 120, 10, 110, 30 };
//             Rectangle addEdgeButtonRect = { screenWidth - 120, 50, 110, 30 };
//             Rectangle editWeightButtonRect = { screenWidth - 120, 90, 110, 30 };
//             Rectangle moveNodeButtonRect = { screenWidth - 120, 130, 110, 30 };
//             Rectangle deleteVertexButtonRect = { screenWidth - 120, 170, 110, 30 };
//             Rectangle deleteEdgeButtonRect = { screenWidth - 120, 210, 110, 30 };   
//             Rectangle doneButtonRect = { screenWidth - 120, screenHeight - 50, 110, 30 };
          
//             bool clickedOnUI = false;
//            if (CheckCollisionPointRec(screenMousePosOnClick, leftPanelRect) || // Kiểm tra vùng UI trái
//                // Kiểm tra từng nút trên vùng UI phải
//                CheckCollisionPointRec(screenMousePosOnClick, addVertexButtonRect) ||
//                CheckCollisionPointRec(screenMousePosOnClick, addEdgeButtonRect) ||
//                CheckCollisionPointRec(screenMousePosOnClick, editWeightButtonRect) ||
//                CheckCollisionPointRec(screenMousePosOnClick, moveNodeButtonRect) ||
//                CheckCollisionPointRec(screenMousePosOnClick, deleteVertexButtonRect) ||
//                CheckCollisionPointRec(screenMousePosOnClick, deleteEdgeButtonRect) ||
//                CheckCollisionPointRec(screenMousePosOnClick, doneButtonRect)
//                // Thêm các nút UI bên trái nếu cần kiểm tra chính xác hơn
//                || CheckCollisionPointRec(screenMousePosOnClick, createButton)
//                || CheckCollisionPointRec(screenMousePosOnClick, randomButton)
//                || CheckCollisionPointRec(screenMousePosOnClick, exampleButton)
//                || CheckCollisionPointRec(screenMousePosOnClick, inputButton)
//                || CheckCollisionPointRec(screenMousePosOnClick, fileButton)
//                || CheckCollisionPointRec(screenMousePosOnClick, editButton)
//                || CheckCollisionPointRec(screenMousePosOnClick, mstButton)
//               )
//            { 
//             clickedOnUI = true;
//             if (CheckCollisionPointRec(GetMousePosition(), addVertexButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 currentTool = TOOL_ADD_VERTEX;
//                 isEditingWeight = false; // Tắt nhập trọng số nếu đang bật
//                 selectedNodeIndex = -1;  // Bỏ chọn node/edge
//                 selectedEdgeIndex = -1;
//             }
//             if (CheckCollisionPointRec(GetMousePosition(), addEdgeButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 currentTool = TOOL_ADD_EDGE_START; // Bắt đầu quy trình thêm cạnh
//                 isEditingWeight = false;
//                 selectedNodeIndex = -1;
//                 selectedEdgeIndex = -1;
//                 TraceLog(LOG_INFO, "EditTool: ADD_EDGE_START"); // Log để debug
//             }
//             if (CheckCollisionPointRec(GetMousePosition(), editWeightButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 currentTool = TOOL_EDIT_WEIGHT;
//                 isEditingWeight = false;
//                 selectedNodeIndex = -1;
//                 selectedEdgeIndex = -1;
//                  TraceLog(LOG_INFO, "EditTool: EDIT_WEIGHT");
//             }
//             if (CheckCollisionPointRec(GetMousePosition(), moveNodeButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 currentTool = TOOL_MOVE_VERTEX;
//                 isEditingWeight = false;
//                 selectedNodeIndex = -1;
//                 selectedEdgeIndex = -1;
//                  TraceLog(LOG_INFO, "EditTool: MOVE_VERTEX");
//             }
//             if (CheckCollisionPointRec(GetMousePosition(), deleteVertexButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 currentTool = TOOL_DELETE_VERTEX;
//                 isEditingWeight = false; // Tắt nhập trọng số nếu đang bật
//                 selectedNodeIndex = -1;  // Bỏ chọn node/edge
//                 selectedEdgeIndex = -1;
//                 isDraggingNode = false; // Đảm bảo không kéo thả nữa
//                 TraceLog(LOG_INFO, "EditTool: DELETE_VERTEX");
//             }
//             if (CheckCollisionPointRec(GetMousePosition(), deleteEdgeButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 currentTool = TOOL_DELETE_EDGE;
//                 isEditingWeight = false;
//                 selectedNodeIndex = -1;
//                 selectedEdgeIndex = -1;
//                 isDraggingNode = false;
//                 TraceLog(LOG_INFO, "EditTool: DELETE_EDGE");
//             }
//             if (CheckCollisionPointRec(GetMousePosition(), doneButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 isEditingGraph = false; // Thoát chế độ edit
//                 currentTool = TOOL_NONE;
//                 isEditingWeight = false;
//                 isDraggingNode = false;
//                 // Cập nhật lại camera nếu đồ thị có thể đã thay đổi kích thước/vị trí
//                 if (graphDrawn && !nodePositions.empty()) {
//                     UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                 }
//             }
//         }
    
//             // Xử lý nhập trọng số nếu đang bật
//             if (isEditingWeight) {
//                 int key = GetCharPressed();
//                 if ((key >= '0' && key <= '9')) { // Chỉ cho nhập số
//                     if(weightInputBuffer.length() < 5) // Giới hạn độ dài
//                         weightInputBuffer += (char)key;
//                 }
//                 if (IsKeyPressed(KEY_BACKSPACE) && weightInputBuffer.length() > 0) {
//                     weightInputBuffer.pop_back();
//                 }
//                 if (IsKeyPressed(KEY_ENTER)) {
//                     int newWeight = 1; // Giá trị mặc định nếu nhập lỗi
//                     try {
//                         newWeight = std::stoi(weightInputBuffer);
//                         if (newWeight <= 0) newWeight = 1; // Đảm bảo trọng số dương
//                     } catch (...) {
//                         TraceLog(LOG_WARNING, "Invalid weight input.");
//                     }
    
//                     if (currentTool == TOOL_ADD_EDGE_END && selectedNodeIndex != -1 && selectedEdgeIndex != -1) { // selectedEdgeIndex giờ dùng để lưu node thứ 2 tạm thời
//                         int nodeIndex1 = selectedNodeIndex;
//                         int nodeIndex2 = selectedEdgeIndex; // Lấy node thứ 2
//                          if (!edgeExists(edges, nodeIndex1 + 1, nodeIndex2 + 1)) {
//                             edges.push_back({nodeIndex1 + 1, nodeIndex2 + 1, newWeight});
//                             TraceLog(LOG_INFO, "Added edge: %d-%d, weight: %d", nodeIndex1 + 1, nodeIndex2 + 1, newWeight);
//                          }
    
//                     } else if (currentTool == TOOL_EDIT_WEIGHT && selectedEdgeIndex != -1) {
//                          if(selectedEdgeIndex >= 0 && selectedEdgeIndex < edges.size()){
//                              edges[selectedEdgeIndex].weight = newWeight;
//                              TraceLog(LOG_INFO, "Edited edge %d weight to: %d", selectedEdgeIndex, newWeight);
//                          }
//                     }
//                     isEditingWeight = false;
//                     weightInputBuffer = "";
//                     selectedNodeIndex = -1;
//                     selectedEdgeIndex = -1;
//                     // Quay lại tool trước đó hoặc TOOL_NONE
//                      if(currentTool == TOOL_ADD_EDGE_END) currentTool = TOOL_ADD_EDGE_START;
//                      // if(currentTool == TOOL_EDIT_WEIGHT) currentTool = TOOL_EDIT_WEIGHT; // Giữ nguyên tool edit weight
//                      // Hoặc đặt về NONE: currentTool = TOOL_NONE;
//                 }
//                  // Hủy nhập trọng số nếu click ra ngoài ô nhập
//                 if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(GetMousePosition(), weightInputRect)) {
//                      isEditingWeight = false;
//                      weightInputBuffer = "";
//                      selectedNodeIndex = -1;
//                      selectedEdgeIndex = -1;
//                      if(currentTool == TOOL_ADD_EDGE_END) currentTool = TOOL_ADD_EDGE_START; // Reset lại tool add edge
//                 }
    
//             }
//             // --- Xử lý click chuột chính (khi không nhập trọng số) ---
//             else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)  && !clickedOnUI) {
//                  int nodeClickedIndex = -1;
//                  // Kiểm tra va chạm với các node hiện có
//                 for(int i=0; i< nodePositions.size(); ++i) {
//                      if(CheckCollisionPointCircle(worldMousePos, nodePositions[i], 20.0f /*bán kính node*/)) {
//                           nodeClickedIndex = i;
//                           break;
//                      }
//                 }
               
//                 int edgeClickedIndex = -1;
//             // Chỉ kiểm tra cạnh nếu không click trúng node VÀ tool hiện tại cần tương tác với cạnh
//             if (nodeClickedIndex == -1 && (currentTool == TOOL_EDIT_WEIGHT || currentTool == TOOL_DELETE_EDGE)) {
//                 float min_dist_sq = 25.0f * 25.0f; // Khoảng cách bình phương tối thiểu để coi là click gần cạnh (có thể cần tinh chỉnh)
//                 for (int i = 0; i < edges.size(); ++i) {
//                     if (edges[i].from > 0 && edges[i].from <= nodePositions.size() &&
//                         edges[i].to > 0 && edges[i].to <= nodePositions.size())
//                     {
//                         Vector2 p1 = nodePositions[edges[i].from - 1];
//                         Vector2 p2 = nodePositions[edges[i].to - 1];
//                         Vector2 midPoint = Vector2Scale(Vector2Add(p1, p2), 0.5f);
//                         float dist_sq = Vector2DistanceSqr(worldMousePos, midPoint);

//                         // Thêm kiểm tra bounding box đơn giản để tránh chọn cạnh quá xa
//                         Rectangle edgeBounds = { fmin(p1.x, p2.x) - 10, fmin(p1.y, p2.y) - 10, fabsf(p1.x - p2.x) + 20, fabsf(p1.y - p2.y) + 20 }; // Hộp bao quanh + lề

//                         if (dist_sq < min_dist_sq && CheckCollisionPointRec(worldMousePos, edgeBounds)) {
//                             edgeClickedIndex = i;
//                             min_dist_sq = dist_sq; // Cập nhật khoảng cách nhỏ nhất
//                         }
//                     }
//                 }
//                  if (edgeClickedIndex != -1) {
//                      TraceLog(LOG_DEBUG, "Edge %d detected as closest for interaction.", edgeClickedIndex);
//                  }
//             }
    
//                 // Xử lý theo công cụ hiện tại
//                 switch (currentTool) {
//                     case TOOL_ADD_VERTEX:
//                         if (nodeClickedIndex == -1)
//                         { // && edgeClickedIndex == -1) { // Chỉ thêm nếu click vào khoảng trống
//                             nodePositions.push_back(worldMousePos);
//                             TraceLog(LOG_INFO, "Added vertex at: %.1f, %.1f", worldMousePos.x, worldMousePos.y);
//                             UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                         }
//                         else
//                         {
//                             TraceLog(LOG_INFO, "Clicked on existing node %d, vertex not added.", nodeClickedIndex + 1);
//                         }
//                         break;
    
//                     case TOOL_ADD_EDGE_START:
//                         if (nodeClickedIndex != -1) {
//                             selectedNodeIndex = nodeClickedIndex; // Lưu index node bắt đầu
//                             currentTool = TOOL_ADD_EDGE_END;   // Chuyển sang chọn node kết thúc
//                              TraceLog(LOG_INFO, "Selected start node: %d", selectedNodeIndex + 1);
//                              UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                         }
//                         break;
    
//                     case TOOL_ADD_EDGE_END:
//                         if (nodeClickedIndex != -1) {
//                             if (selectedNodeIndex != -1 && nodeClickedIndex != selectedNodeIndex) {
//                                 // Đã chọn node thứ 2 hợp lệ
//                                  // Kiểm tra cạnh đã tồn tại chưa
//                                 if (!edgeExists(edges, selectedNodeIndex + 1, nodeClickedIndex + 1)) {
//                                     isEditingWeight = true; // Bật ô nhập trọng số
//                                     weightInputBuffer = "1"; // Giá trị mặc định
//                                     // Tính vị trí ô nhập gần giữa cạnh
//                                     Vector2 p1 = nodePositions[selectedNodeIndex];
//                                     Vector2 p2 = nodePositions[nodeClickedIndex];
//                                     weightInputPosition = Vector2Scale(Vector2Add(p1, p2), 0.5f);
//                                     weightInputPosition = GetWorldToScreen2D(weightInputPosition, graphCamera); // Chuyển về tọa độ màn hình
//                                     weightInputRect.x = weightInputPosition.x - weightInputRect.width / 2;
//                                     weightInputRect.y = weightInputPosition.y - weightInputRect.height / 2;
    
//                                     selectedEdgeIndex = nodeClickedIndex; // << Tận dụng biến này để lưu node thứ 2 TẠM THỜI
//                                     TraceLog(LOG_INFO, "Selected end node: %d. Enter weight.", nodeClickedIndex + 1);
//                                     UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                                 } else {
//                                      TraceLog(LOG_INFO, "Edge already exists between %d and %d", selectedNodeIndex + 1, nodeClickedIndex + 1);
//                                      // Reset lại
//                                      selectedNodeIndex = -1;
//                                      currentTool = TOOL_ADD_EDGE_START;
//                                 }
    
//                             } else if (nodeClickedIndex == selectedNodeIndex) {
//                                  // Click lại vào node cũ, hủy chọn
//                                  selectedNodeIndex = -1;
//                                  currentTool = TOOL_ADD_EDGE_START;
//                                  TraceLog(LOG_INFO, "Cancelled edge start node.");
//                                  UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                             }
//                         } else {
//                              // Click ra ngoài, hủy chọn node bắt đầu
//                              selectedNodeIndex = -1;
//                              currentTool = TOOL_ADD_EDGE_START;
//                               TraceLog(LOG_INFO, "Cancelled edge creation.");
//                         }
//                         break;
    
//                     case TOOL_EDIT_WEIGHT:
//                          if (edgeClickedIndex != -1) {
//                              selectedEdgeIndex = edgeClickedIndex;
//                              isEditingWeight = true;
//                              weightInputBuffer = std::to_string(edges[selectedEdgeIndex].weight);
//                              // Tính vị trí ô nhập
//                              Vector2 p1 = nodePositions[edges[selectedEdgeIndex].from - 1];
//                              Vector2 p2 = nodePositions[edges[selectedEdgeIndex].to - 1];
//                              weightInputPosition = Vector2Scale(Vector2Add(p1, p2), 0.5f);
//                              weightInputPosition = GetWorldToScreen2D(weightInputPosition, graphCamera);
//                              weightInputRect.x = weightInputPosition.x - weightInputRect.width / 2;
//                              weightInputRect.y = weightInputPosition.y - weightInputRect.height / 2;
//                              TraceLog(LOG_INFO, "Selected edge %d to edit weight.", selectedEdgeIndex);
//                              UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                          }
//                          else
//                          {
//                              isEditingWeight = false; // Tắt ô nhập
//                              selectedEdgeIndex = -1; // Bỏ chọn cạnh
//                          }
//                         break;
//                     case TOOL_MOVE_VERTEX:
//                          if (nodeClickedIndex != -1) {
//                              selectedNodeIndex = nodeClickedIndex;
//                              isDraggingNode = true; // Bắt đầu kéo node
//                              TraceLog(LOG_INFO, "Started dragging node %d", selectedNodeIndex + 1);
//                              UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                          }
//                         break;
//                     case TOOL_DELETE_VERTEX:
//                     if (nodeClickedIndex != -1) { // Kiểm tra xem có click trúng nút không
//                         int deletedNodeIndex = nodeClickedIndex;
//                         int deletedNodeId = deletedNodeIndex + 1; // ID của nút = index + 1

//                         TraceLog(LOG_INFO, "Attempting to delete vertex ID: %d (Index: %d)", deletedNodeId, deletedNodeIndex);

//                         // 1. Xóa các cạnh nối với đỉnh bị xóa
//                         // Sử dụng std::remove_if để hiệu quả
//                         edges.erase(std::remove_if(edges.begin(), edges.end(),
//                             [deletedNodeId](const Edge& edge) {
//                                 // Trả về true nếu cạnh nối với đỉnh bị xóa
//                                 return edge.from == deletedNodeId || edge.to == deletedNodeId;
//                             }), edges.end());

//                         // 2. Xóa vị trí của đỉnh khỏi vector nodePositions
//                         if (deletedNodeIndex >= 0 && deletedNodeIndex < nodePositions.size()) {
//                             nodePositions.erase(nodePositions.begin() + deletedNodeIndex);
//                         }

//                         // 3. Cập nhật lại ID (from, to) trong các cạnh còn lại
//                         // Vì index của các nút sau nút bị xóa đã thay đổi
//                         for (Edge& edge : edges) {
//                             if (edge.from > deletedNodeId) {
//                                 edge.from--; // Giảm ID đi 1
//                             }
//                             if (edge.to > deletedNodeId) {
//                                 edge.to--; // Giảm ID đi 1
//                             }
//                         }

//                         // 4. Đặt lại lựa chọn (không còn nút nào được chọn sau khi xóa)
//                         selectedNodeIndex = -1;
//                         // Giữ nguyên currentTool = TOOL_DELETE_VERTEX để có thể xóa nhiều nút liên tiếp

//                          // 5. Cập nhật camera vì đồ thị có thể thay đổi kích thước/vị trí
//                          if (!nodePositions.empty()) {
//                             UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                          } else {
//                             // Reset camera về mặc định nếu không còn nút nào
//                             graphCamera.target = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
//                             graphCamera.offset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
//                             graphCamera.zoom = 1.0f;
//                             // Nếu không còn nút và cạnh, có thể đặt graphDrawn = false
//                             // if (edges.empty()) graphDrawn = false; // Xem xét cẩn thận nếu muốn làm vậy
//                          }

//                         TraceLog(LOG_INFO, "Deleted vertex ID: %d. Adjusted remaining edges.", deletedNodeId);

//                     } else {
//                          TraceLog(LOG_INFO, "Delete Vertex tool active, but clicked empty space.");
//                     }
//                     break; // Kết thúc case TOOL_DELETE_VERTEX
//                     case TOOL_DELETE_EDGE:
//                      // Sử dụng edgeClickedIndex đã được tính ở trên
//                      if (edgeClickedIndex != -1) { // Nếu click gần một cạnh
//                         TraceLog(LOG_INFO, "Attempting to delete edge index: %d (From: %d, To: %d, Weight: %d)",
//                                  edgeClickedIndex, edges[edgeClickedIndex].from, edges[edgeClickedIndex].to, edges[edgeClickedIndex].weight);

//                         // Xóa cạnh khỏi vector edges
//                         if (edgeClickedIndex >= 0 && edgeClickedIndex < edges.size()) {
//                             edges.erase(edges.begin() + edgeClickedIndex);
//                             TraceLog(LOG_INFO, "Edge deleted successfully.");
//                         } else {
//                             TraceLog(LOG_WARNING, "Invalid edge index %d to delete.", edgeClickedIndex);
//                         }

//                         // Đặt lại lựa chọn (không còn cạnh nào được chọn)
//                         selectedEdgeIndex = -1;
//                         // Giữ nguyên currentTool = TOOL_DELETE_EDGE để xóa nhiều cạnh
//                         UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//                     } else {
//                         // Click vào node hoặc khoảng trống khi đang dùng tool xóa cạnh
//                         TraceLog(LOG_INFO, "Delete Edge tool active, but clicked empty space or node.");
//                         selectedEdgeIndex = -1; // Bỏ chọn nếu có cạnh đang được chọn
//                     }
//                     break;
//                     case TOOL_NONE:
//                      default:
//                          break; // Không làm gì nếu không chọn tool hoặc click không trúng gì
//                 }
//             }
//             // Xử lý kéo thả node
//             if (currentTool == TOOL_MOVE_VERTEX && isDraggingNode && selectedNodeIndex != -1) {
//                  if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
//                      // Cập nhật vị trí node theo chuột (trong world space)
//                      nodePositions[selectedNodeIndex] = worldMousePos;
//                  }
//                  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
//                      isDraggingNode = false;
//                      selectedNodeIndex = -1; // Kết thúc kéo, bỏ chọn
//                       TraceLog(LOG_INFO, "Finished dragging node.");
//                  }
//             }
    
    
//         }
//     BeginDrawing();
//     ClearBackground(GRAY);
//     if (graphDrawn && showGraph && !showMSTMenu && !showMatrixInput) {
//         BeginMode2D(graphCamera);
//         if (nodePositions.empty() && !edges.empty()) {
//             positionNodesInCircle(nodePositions, std::max(static_cast<std::size_t>(edges.size()), edges.empty() ? static_cast<std::size_t>(0) : static_cast<std::size_t>(std::max_element(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) {
//                 return std::max(static_cast<int>(a.from), static_cast<int>(a.to)) < std::max(static_cast<int>(a.from), static_cast<int>(a.to));
//             })->to)), screenWidth / 2.0f, screenHeight / 2.0f, 200.0f);
//         }
//         // Cập nhật kích thước ban đầu
//         for (const auto& pos : nodePositions) {
//             graphOriginalSize.x = std::max(graphOriginalSize.x, pos.x);
//             graphOriginalSize.y = std::max(graphOriginalSize.y, pos.y);
//         }
//     // Vẽ cạnh
//     for (const auto& edge : edges) {
//         // Kiểm tra chỉ số hợp lệ trước khi truy cập nodePositions
//          if (edge.from > 0 && edge.from <= nodePositions.size() &&
//             edge.to > 0 && edge.to <= nodePositions.size())
//         {
//             // Sử dụng nodePositions gốc, camera sẽ tự động biến đổi tọa độ
//             Vector2 startPos = nodePositions[edge.from - 1];
//             Vector2 endPos = nodePositions[edge.to - 1];
//             drawBezierEdge(startPos, endPos, 2.0f / graphCamera.zoom, DARKBLUE); // Làm dày đường kẻ khi zoom out
//             // Vị trí trọng số cũng cần tính từ tọa độ gốc
//             drawEdgeWeight(startPos, endPos, edge.weight); // Hàm drawEdgeWeight nên độc lập với zoom
//         } else {
//              // Có thể log lỗi nếu chỉ số không hợp lệ
//              std::cerr << "Warning: Invalid edge indices (" << edge.from << ", " << edge.to << ") for node count " << nodePositions.size() << std::endl;
//         }
//     }
//     //Vẽ Node
//     for (int i = 0; i < nodePositions.size(); i++) {
//         drawNode(nodePositions[i], i + 1, ORANGE, 20.0f);

//     }
//     if (isEditingGraph && currentTool == TOOL_ADD_VERTEX) {
//         Vector2 currentScreenMousePos = GetMousePosition(); // Lấy vị trí chuột hiện tại để vẽ
//         float uiLeftWidth = 120.0f;
//         float uiRightWidth = 130.0f;

//         // Chỉ vẽ nếu chuột nằm trong khu vực vẽ đồ thị (không chồng lên UI)
//         if (currentScreenMousePos.x > uiLeftWidth && currentScreenMousePos.x < (screenWidth - uiRightWidth)) {
//             Vector2 currentWorldMousePos = GetScreenToWorld2D(currentScreenMousePos, graphCamera);

//             // --- KIỂM TRA CHỒNG LẤN VỚI NODE HIỆN CÓ ---
//             bool overExistingNode = false;
//             for(int i=0; i < nodePositions.size(); ++i) {
//                  // Dùng bán kính nhỏ hơn một chút để tránh bị che khuất khi ở gần
//                 if(CheckCollisionPointCircle(currentWorldMousePos, nodePositions[i], 20.0f )) {
//                     overExistingNode = true;
//                     break;
//                 }
//             }
//             // --- Chỉ vẽ nếu không chồng lên node nào ---
//             if (!overExistingNode) {
//                 DrawCircleV(currentWorldMousePos, 20.0f, Fade(SKYBLUE, 0.5f));
//                 DrawCircleLines(currentWorldMousePos.x, currentWorldMousePos.y, 20.0f, Fade(BLUE, 0.6f));
//             }
//         }
//     }
//     if (isEditingGraph) {
//         // Highlight node đang chọn (cho Add Edge hoặc Move)
//         if (selectedNodeIndex != -1 && selectedNodeIndex < nodePositions.size()) {
//             DrawCircleLines(nodePositions[selectedNodeIndex].x, nodePositions[selectedNodeIndex].y, 22.0f, YELLOW); // Viền vàng quanh node
//         }
//         // Vẽ đường line nối từ node đầu tiên đến chuột khi đang thêm cạnh
//         if (currentTool == TOOL_ADD_EDGE_END && selectedNodeIndex != -1 && selectedNodeIndex < nodePositions.size()) {
//             DrawLineEx(nodePositions[selectedNodeIndex], worldMousePos, 2.0f / graphCamera.zoom, YELLOW);
//         }
//         // Highlight cạnh đang chọn để sửa trọng số
//         if (currentTool == TOOL_EDIT_WEIGHT && selectedEdgeIndex != -1 && selectedEdgeIndex < edges.size()) {
//               if (edges[selectedEdgeIndex].from > 0 && edges[selectedEdgeIndex].from <= nodePositions.size() &&
//                   edges[selectedEdgeIndex].to > 0 && edges[selectedEdgeIndex].to <= nodePositions.size())
//              {
//                   DrawLineEx(nodePositions[edges[selectedEdgeIndex].from - 1], nodePositions[edges[selectedEdgeIndex].to - 1], 4.0f / graphCamera.zoom, YELLOW); // Line dày hơn
//              }
//         }
//     }
//     EndMode2D();
// }
//     if (graphDrawn && numNodesStr != "" && numEdgesStr != "") {
//         int numNodes = std::stoi(numNodesStr);
//         int nodeRadius = 20;
//         for (const auto& edge : edges) {
//             Vector2 fromPos = nodePositions[edge.from - 1];
//             Vector2 toPos = nodePositions[edge.to - 1];
//             DrawLineV(fromPos, toPos, DARKBLUE);
//             Vector2 weightPos = {
//                 fromPos.x + (toPos.x - fromPos.x) * 0.5f,
//                 fromPos.y + (toPos.y - fromPos.y) * 0.5f
//             };
//             DrawText(TextFormat("%d", edge.weight), weightPos.x, weightPos.y, 20, SKYBLUE);
//         }

//         for (int i = 0; i < numNodes; ++i) {
//             DrawCircleV(nodePositions[i], nodeRadius, ORANGE);
//             DrawCircleV(nodePositions[i], nodeRadius - 2, WHITE);
//             DrawText(TextFormat("%d", i + 1), nodePositions[i].x - MeasureText(TextFormat("%d", i + 1), 20) / 2, nodePositions[i].y - 10, 20, DARKGRAY);
//         }
//     }
//     if (!isEditingGraph)
//     {
//     if (isCreating && canCreateGraph) {
//         // Logic cho chức năng Create
//         if (canCreateGraph) {
//             DrawText("N:", nodesInput.x - 25, nodesInput.y + 5, 20, WHITE);
//             DrawText("E:", edgesInput.x - 25, edgesInput.y + 5, 20, WHITE);

//             DrawRectangleRec(nodesInput, LIGHTGRAY);
//             DrawText(numNodesStr.c_str(), nodesInput.x + 5, nodesInput.y + 5, 20, BLACK);
//             if (CheckCollisionPointRec(GetMousePosition(), nodesInput) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 nodesFocused = true;
//                 edgesFocused = false;
//             }
//             if (nodesFocused) {
//                 int key = GetCharPressed();
//                 if (key >= 32 && key <= 125) {
//                     numNodesStr += (char)key;
//                 }
//                 if (IsKeyPressed(KEY_BACKSPACE) && numNodesStr.length() > 0) {
//                     numNodesStr.pop_back();
//                 }
//             }

//             DrawRectangleRec(edgesInput, LIGHTGRAY);
//             DrawText(numEdgesStr.c_str(), edgesInput.x + 5, edgesInput.y + 5, 20, BLACK);
//             if (CheckCollisionPointRec(GetMousePosition(), edgesInput) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 edgesFocused = true;
//                 nodesFocused = false;
//             }
//             if (edgesFocused) {
//                 int key = GetCharPressed();
//                 if (key >= 32 && key <= 125) {
//                     numEdgesStr += (char)key;
//                 }
//                 if (IsKeyPressed(KEY_BACKSPACE) && numEdgesStr.length() > 0) {
//                     numEdgesStr.pop_back();
//                 }
//             }
//         }

//         if (IsKeyPressed(KEY_ENTER)) {
//             //logic ve do thi khi nhan enter
//              if (numNodesStr == "" || numEdgesStr == "") {
//                 canCreateGraph = false;
//                 showError = true;
//                 errorMessage = "Please enter nodes and edges.";
//             } else {
//                 int numNodes = std::stoi(numNodesStr);
//                 int numEdges = std::stoi(numEdgesStr);

//                 if (numNodes <= 0 || numEdges < 0 || numEdges > numNodes * (numNodes - 1) / 2 || numEdges < numNodes - 1) {
//                     canCreateGraph = false;
//                     showError = true;
//                     errorMessage = "Can't create a graph";
//                 } else {
//                     edges.clear();
//                     srand(time(0));
//                     while (edges.size() < numEdges) {
//                         int from = GetRandomValue(1, numNodes);
//                         int to = GetRandomValue(1, numNodes);
//                         int weight = GetRandomValue(1, 10);
//                         if (from != to && !edgeExists(edges, from, to)) {
//                             edges.push_back({ from, to, weight });
//                         }
//                     }

//                     if (!isGraphConnected(edges, numNodes)) {
//                         canCreateGraph = false;
//                         showError = true;
//                         errorMessage = "Không thể tạo graph liên thông với số liệu này.";
//                     } else {
//                         nodePositions.resize(numNodes);
//                         int nodeRadius = 20;
//                         float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//                         for (int i = 0; i < numNodes; ++i) {
//                             float angle = 2.0f * PI * i / numNodes;
//                             nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//                         }
//                         UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight); // <<--- GỌI Ở ĐÂY
//                         graphDrawn = true;
//                         isCreating = false;
//                         numNodesStr = "";
//                         numEdgesStr = "";
//                         showError = false;
//                     }
//                 }
//             }
//         }
//     }
//     if (showError && isCreating) {
//         DrawText(errorMessage.c_str(), createButton.x + createButton.width + 30, createButton.y + 10, 20, RED);
//     }
//     if (!isCreatingActive) 
//     {
//         DrawRectangleRec(nodesInput, GRAY); // Vẽ chồng lên input field màu xám.
//         DrawRectangleRec(edgesInput, GRAY);
//         DrawText("N:", nodesInput.x - 25, nodesInput.y + 5, 20, GRAY);
//         DrawText("E:", edgesInput.x - 25, edgesInput.y + 5, 20, GRAY);
//     }
//     if (isRandomizing) {
//         // Logic cho chức năng Random
//         int numNodes = GetRandomValue(2, 7);
//         int numEdges = GetRandomValue(numNodes - 1, numNodes * (numNodes - 1) / 2);
//         generateRandomGraph(edges, numNodes, numEdges);

//         nodePositions.resize(numNodes);
//         for (int i = 0; i < numNodes; ++i) {
//             float angle = 2.0f * PI * i / numNodes;
//             float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//             nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//         }
//         UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight); // <<--- GỌI Ở ĐÂY
//         graphDrawn = true;
//         isRandomizing = false;
//         numNodesStr = "";
//         numEdgesStr = "";
//         inputMode = false;
//         isCreatingActive = false;
//     }
//     if (showMSTMenu && !isEditingGraph) {
//         DrawRectangleRec(mstMenuRect, LIGHTGRAY);
//         DrawText("Minimum Spanning Tree", mstMenuRect.x + 10, mstMenuRect.y + 10, 20, BLACK);
    
//         // Vẽ các nút Prim, Kruskal, và Back
//         DrawRectangleRec(primButton, WHITE);
//         DrawText("Prim", primButton.x + 10, primButton.y + 10, 20, BLACK);
    
//         DrawRectangleRec(kruskalButton, WHITE);
//         DrawText("Kruskal", kruskalButton.x + 10, kruskalButton.y + 10, 20, BLACK);
    
//         DrawRectangleRec(backButton, WHITE);
//         DrawText("Back", backButton.x + 10, backButton.y + 10, 20, BLACK);

//         speedSlider.update();
//         speedSlider.draw();
//         if (speedSlider.isEditing() && (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER))) {
//              speedSlider.stopEditing();
//         }
//         if (showWeightInfo)
//         {
//             drawMSTInfo(totalMSTWeight, isMSTDrawingPaused,pauseResumeButton,mstMenuRect); 
            
//         }
//         // Xử lý sự kiện click chuột cho các nút
//         if (CheckCollisionPointRec(GetMousePosition(), primButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 mstEdgesDrawn.clear();
//                 mstEdgeIndex = 0;
//                 mstDrawTimer = 0.0f;
//                 mstDoneDrawing = false;
//                 mstNodesDrawn.clear();
//                 totalMSTWeight = 0;
//                 usePrim = true;
//                 showWeightInfo = true; // Hiển thị thông tin trọng số MST
//                 useKruskal = false;
//                 mstEdges = calculatePrimMST(edges, nodePositions.size()); // Tính toán MST bằng Prim
//                 mstEdgesDrawn.resize(mstEdges.size(), false);
//                 mstEdgeIndex = 0;
//                 mstDrawTimer = 0.0f; 
//                 drawMSTOnMSTMenu = true;
//                 mstDoneDrawing = false; // Reset flag MST drawing finish status
//             }
    
//         if (CheckCollisionPointRec(GetMousePosition(), kruskalButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 mstEdgesDrawn.clear();
//                 mstEdgeIndex = 0;
//                 mstDrawTimer = 0.0f;
//                 mstDoneDrawing = false;
//                 mstNodesDrawn.clear();
//                 totalMSTWeight = 0;
//                 showWeightInfo = true; // Hiển thị thông tin trọng số MST
//                 usePrim = false;
//                 useKruskal = true;
//                 mstEdges = calculateKruskalMST(edges, nodePositions.size()); // Tính toán MST bằng Kruskal
//                 mstEdgesDrawn.resize(mstEdges.size(), false);
//                 mstEdgeIndex = 0;
//                 mstDrawTimer = 0.0f;
//                 drawMSTOnMSTMenu = true;
//                 mstDoneDrawing = false; // Reset flag MST drawing finish status
//             }
    
//         if (CheckCollisionPointRec(GetMousePosition(), backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             showMSTMenu = false;
//             showGraph = true;   // Hiển thị lại đồ thị gốc
//             drawMSTOnMSTMenu = false;
//             mstEdges.clear();
//             mstEdgesDrawn.clear(); // Reset mstEdgesDrawn vector
//             mstEdgeIndex = 0; // Reset index to zero
//             mstDrawTimer = 0.0f; // Reset time to zero
//             usePrim = false;
//             useKruskal = false;
//             showWeightInfo = false; // Ẩn thông tin trọng số MST
//             showMSTError = false; // Ẩn thông báo lỗi
//             mstDoneDrawing = false;
//             isMSTDrawingPaused = false; // Reset trạng thái tạm dừng
//         }
//         if (CheckCollisionPointRec(GetMousePosition(), pauseResumeButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             isMSTDrawingPaused = !isMSTDrawingPaused; // Đảo trạng thái tạm dừng
//         }
//         int numNodesMST = 0;
//         for (const auto& edge : mstEdges) {
//             numNodesMST = std::max({numNodesMST, edge.from, edge.to});
//         }
        
//         std::vector<Vector2> mstNodePositions(numNodesMST);
//         std::vector<bool> mstNodesDrawn(numNodesMST, false);
//         float radius = std::min(mstMenuRect.width, mstMenuRect.height) / 3.0f; // Adjust for spacing
        
//         //Tính toán tỉ lệ scaling
//         float scaleX = graphBoundingBox.width / graphOriginalSize.x;
//         float scaleY = graphBoundingBox.height / graphOriginalSize.y;
//         float graphScale = std::min(scaleX, scaleY); // Chọn tỉ lệ nhỏ hơn để đồ thị vừa với chiều rộng và cao của khung

//         Vector2 graphOffset = {
//         graphBoundingBox.x + (graphBoundingBox.width - graphOriginalSize.x * graphScale) / 2, // Căn giữa theo chiều x
//         graphBoundingBox.y + (graphBoundingBox.height - graphOriginalSize.y * graphScale) / 2  // Căn giữa theo chiều y
//         };

//      for (int i = 0; i < numNodesMST; ++i) {
//          float originalX = mstNodePositions[i].x; // Lấy vị trí x ban đầu
//          float originalY = mstNodePositions[i].y; // Lấy vị trí y ban đầu

//          // Áp dụng phép scaling và translation
//          mstNodePositions[i] = {
//              graphOffset.x + (originalX - graphOriginalSize.x / 2) * graphScale, // Thay đổi vị trí theo kích thước đồ thị
//              graphOffset.y + (originalY - graphOriginalSize.y / 2) * graphScale  // Thay đổi vị trí theo kích thước đồ thị
//          };
//      }
//      mstNodePositions.resize(numNodesMST);
//      for (int i = 0; i < numNodesMST; ++i) {
//         // Sử dụng cách tính vị trí nút phù hợp với bạn
//         float angle = 2.0f * PI * i / numNodesMST;
//         float radius = std::min(mstMenuRect.width, mstMenuRect.height) / 3.0f;
//         mstNodePositions[i] = {
//             mstMenuRect.x + mstMenuRect.width / 2.0f + radius * cosf(angle),
//             mstMenuRect.y + mstMenuRect.height / 2.0f + radius * sinf(angle)
//         };
//     }

//         // Vẽ các cạnh MST (từ từ)
//         if (usePrim || useKruskal) {
//             // Vẽ các cạnh MST (từ từ)
//             for (int i = 0; i < mstEdgesDrawn.size(); i++) {
//                 if (mstEdgesDrawn[i]) {
//                     int from = mstEdges[i].from - 1;
//                     int to = mstEdges[i].to - 1;
//                     Vector2 fromPos = mstNodePositions[from];
//                     Vector2 toPos = mstNodePositions[to];
//                     DrawLineEx(fromPos, toPos, 3.0f, RED);
//                     drawEdgeWeight(fromPos, toPos, mstEdges[i].weight);
    
//                     // Đánh dấu các nút đã được vẽ
//                     mstNodesDrawn[from] = true;
//                     mstNodesDrawn[to] = true;
//                 }
//             }
    
//             // Vẽ cạnh MST tiếp theo
//             if (!isMSTDrawingPaused &&mstEdgeIndex < mstEdges.size()) {
//                 mstDrawTimer += GetFrameTime();
//                 if (mstDrawTimer >= speedSlider.getValue()) {
//                     int from = mstEdges[mstEdgeIndex].from - 1;
//                     int to = mstEdges[mstEdgeIndex].to - 1;
//                     Vector2 fromPos = mstNodePositions[from];
//                     Vector2 toPos = mstNodePositions[to];
//                     DrawLineEx(fromPos, toPos, 3.0f, SKYBLUE);
//                     drawEdgeWeight(fromPos, toPos, mstEdges[mstEdgeIndex].weight);
    
//                     mstEdgesDrawn[mstEdgeIndex] = true;
    
//                     // Đánh dấu các nút đã được vẽ
//                     mstNodesDrawn[from] = true;
//                     mstNodesDrawn[to] = true;
            
//                     mstEdgeIndex++;
//                     mstDrawTimer = 0.0f;
//                     totalMSTWeight += mstEdges[mstEdgeIndex-1].weight;
//                 }
//             } else {
//                 // Sau khi vẽ xong MST, vẽ các cạnh không thuộc MST
//                 for (const auto& edge : edges) {
//                     bool isMSTEdge = false;
//                     for (const auto& mstEdge : mstEdges) {
//                         if ((edge.from == mstEdge.from && edge.to == mstEdge.to) ||
//                             (edge.from == mstEdge.to && edge.to == mstEdge.from)) {
//                             isMSTEdge = true;
//                             break;
//                         }
//                     }
//                     if (!isMSTEdge) {
//                         Color edgeColor = Fade(DARKBLUE, 0.3f);
//                         int fromIndex = edge.from - 1;
//                         int toIndex = edge.to - 1;
//                         if (fromIndex >= 0 && fromIndex < mstNodePositions.size() &&
//                             toIndex >= 0 && toIndex < mstNodePositions.size()) {
//                             DrawLineEx(mstNodePositions[fromIndex], mstNodePositions[toIndex], 2.0f, edgeColor);
//                             drawEdgeWeight(mstNodePositions[fromIndex], mstNodePositions[toIndex], edge.weight);
//                         }
//                     }
//                 }
//             }
//         } else if (!mstEdges.empty()) {
//             // draw edges
//             for (const auto& edge : edges) {
//                 Color edgeColor = DARKBLUE;
//                 bool isMSTEdge = false;
//                 for (const auto& mstEdge : mstEdges) {
//                     if ((edge.from == mstEdge.from && edge.to == mstEdge.to) ||
//                         (edge.from == mstEdge.to && edge.to == mstEdge.from)) {
//                         isMSTEdge = true;
//                         break;
//                     }
//                 }
//                 if (usePrim && !isMSTEdge) {
//                     edgeColor = Fade(DARKBLUE, 0.3f); // Làm mờ cạnh không thuộc MST
//                 }
        
//                 int fromIndex = edge.from - 1;
//                 int toIndex = edge.to - 1;
//                 if (fromIndex >= 0 && fromIndex < mstNodePositions.size() &&
//                 toIndex >= 0 && toIndex < mstNodePositions.size()) {
//                 DrawLineEx(mstNodePositions[fromIndex], mstNodePositions[toIndex], 2.0f, edgeColor);
//                 drawEdgeWeight(mstNodePositions[fromIndex], mstNodePositions[toIndex], edge.weight);
//                 }
//             }
//         }
//         // draw node
//         for (int i = 0; i < mstNodePositions.size(); i++) {
//             if (mstNodesDrawn[i])
//             {
//                 drawNode(mstNodePositions[i], i + 1, ORANGE, 20); // Use styled node drawing
//             }
//             else
//             {
//                 drawNode(mstNodePositions[i], i + 1, WHITE, 20); // Use styled node drawing
//             }
//         }
//     }
//     // Vẽ thông báo lỗi (nếu có)
//     if (showMSTError) {
//     DrawText(mstErrorMessage.c_str(), mstButton.x + mstButton.width + 10, mstButton.y + 10, 20, RED);
//     }
//     if (showFileError) {
//         // Vẽ gần nút File hoặc ở một vị trí thông báo chung
//         DrawText(fileErrorMessage.c_str(), fileButton.x + fileButton.width + 10, fileButton.y + 5, 18, RED);
//     }
//     if (CheckCollisionPointRec(GetMousePosition(), createButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//         showError = false; 
//         errorMessage = ""; 
//         showFileError = false;
//         fileErrorMessage = "";
//         isEditingGraph = false;
//         currentTool = TOOL_NONE;
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         weightInputBuffer = "";
//         isDraggingNode = false;
//         std::vector<Vector2> emptyNodes;
//         UpdateGraphCamera(graphCamera, emptyNodes, 20.0f, screenWidth, screenHeight);
//         ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused);
//         edges.clear(); 
//         nodePositions.clear(); 
//         isCreating = true;
//         isCreatingActive = true;
//         inputMode = false;
//         canCreateGraph = true;
//         showExampleButtons = false;
//         showMSTError = false;
//         showMSTMenu = false;
//     }
//     else if(CheckCollisionPointRec(GetMousePosition(),randomButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
//         showError = false; 
//         errorMessage = ""; 
//         showFileError = false;
//         fileErrorMessage = "";
//         isEditingGraph = false;
//         currentTool = TOOL_NONE;
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         weightInputBuffer = "";
//         isDraggingNode = false;
//         std::vector<Vector2> emptyNodes;
//         UpdateGraphCamera(graphCamera, emptyNodes, 20.0f, screenWidth, screenHeight);
//         ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused);
//         isRandomizing = true;
//         isRandomizingActive = true;
//         edges.clear(); 
//         nodePositions.clear(); 
//         inputMode = false;
//         isCreatingActive = false;
//         isCreating = false;
//         showExampleButtons = false;
//         showMSTError = false;
//         showMSTMenu = false;
//     }
//     else if(CheckCollisionPointRec(GetMousePosition(), exampleButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
//         showError = false; 
//         errorMessage = ""; 
//         showFileError = false;
//         fileErrorMessage = "";
//         isEditingGraph = false;
//         currentTool = TOOL_NONE;
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         weightInputBuffer = "";
//         isDraggingNode = false;
//         std::vector<Vector2> emptyNodes;
//         UpdateGraphCamera(graphCamera, emptyNodes, 20.0f, screenWidth, screenHeight);
//         ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused);
//         edges.clear(); 
//         nodePositions.clear(); 
//         showExampleButtons = !showExampleButtons;
//         isShowingExamplesActive = true;
//         inputMode=false;
//         isCreatingActive = false;
//         showMSTError = false;
//         showMSTMenu = false;
//     }
//     else if(CheckCollisionPointRec(GetMousePosition(),inputButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
//         showError = false; 
//         errorMessage = ""; 
//         showFileError = false;
//         fileErrorMessage = "";
//         isEditingGraph = false;
//         currentTool = TOOL_NONE;
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         weightInputBuffer = "";
//         isDraggingNode = false;
//         std::vector<Vector2> emptyNodes;
//         UpdateGraphCamera(graphCamera, emptyNodes, 20.0f, screenWidth, screenHeight);
//         ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused);
//         edges.clear(); 
//         nodePositions.clear(); 
//         inputMode = true;
//         showMatrixInput = true;
//         isInputActive = true;
//         isCreatingActive = false;
//         isShowingExamplesActive = false;
//         showExampleButtons = false;
//         showMSTError = false;
//         showMSTMenu = false;
//     }
//     else if(CheckCollisionPointRec(GetMousePosition(), fileButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
//         showError = false; 
//         showFileError = false;
//         fileErrorMessage = "";
//         errorMessage = ""; 
//         isEditingGraph = false;
//         currentTool = TOOL_NONE;
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         weightInputBuffer = "";
//         isDraggingNode = false;
//         std::vector<Vector2> emptyNodes;
//         UpdateGraphCamera(graphCamera, emptyNodes, 20.0f, screenWidth, screenHeight);
//         ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused);
//         edges.clear(); 
//         nodePositions.clear(); 
//         isFileActive = true;
//         isCreatingActive = false;
//         isRandomizingActive = false;
//         isShowingExamplesActive = false;
//         isInputActive = false;
//         showExampleButtons = false;
//         showMSTError = false;
//         showMSTMenu = false;
//         // Mở hộp thoại chọn file
//         const char * filterPatterns[1] = { "*.txt" }; // Chỉ cho phép chọn file .txt (có thể thêm *.csv, ...)
//         const char * selectedFilePath = tinyfd_openFileDialog(
//              "Select Graph File (Adjacency Matrix)", // Tiêu đề hộp thoại
//              "", // Đường dẫn mặc định (để trống)
//              1, // Số lượng filter
//              filterPatterns, // Mảng các filter
//              "Text Files (*.txt)", // Mô tả filter
//              0 // Không cho phép chọn nhiều file
//          );
//         if (selectedFilePath != NULL) {
//             // Người dùng đã chọn một file
//             std::string loadErrorMessage;
//             bool success = LoadGraphFromFile(selectedFilePath, edges, nodePositions, screenWidth, screenHeight, loadErrorMessage);

//             if (success) {
//                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight); // <<--- GỌI Ở ĐÂY
//                 graphDrawn = true;
//                 inputMode = false;
//                 matrixInputFocused = false;
//                 isFileActive = false; // Có thể tắt cờ này nếu không dùng nữa
//             } else {
//                 // Xảy ra lỗi khi tải file
//                 graphDrawn = false; // Không có đồ thị để vẽ
//                 edges.clear();      // Xóa cạnh cũ (nếu có)
//                 nodePositions.clear(); // Xóa vị trí nút cũ
//                 fileErrorMessage = loadErrorMessage;
//                 showFileError = true; // Hiển thị thông báo lỗi file
//                 isFileActive = false; 
//             }
//         } else {
//             isFileActive = false; // Tắt cờ
//             // Không làm gì cả hoặc hiển thị thông báo "Cancelled"
//         }
//     }
//     else if (CheckCollisionPointRec(GetMousePosition(), mstButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//         if (!graphDrawn)
//         {
//             showMSTMenu = false;
//             showMSTError = true;
//             mstErrorMessage = "Please Input or Create a graph first";
//         }
//         else
//         {
//             showMSTMenu = true;
//             mstButtonClicked = true;
//             showGraph = false;
//             mstNodesDrawn.clear();
//             drawMSTOnMSTMenu = false;
//             mstEdges.clear();
//             mstEdgesDrawn.clear();
//         }
//         isEditingGraph = false;
//         currentTool = TOOL_NONE;
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         weightInputBuffer = "";
//         isDraggingNode = false;
//         showFileError = false;
//         fileErrorMessage = "";
//         std::vector<Vector2> emptyNodes;
//         UpdateGraphCamera(graphCamera, emptyNodes, 20.0f, screenWidth, screenHeight);
//         showMatrixInput = false;
//         showExampleButtons = false;
//         isRandomizingActive = false;
//         isCreatingActive = false;
//         isFileActive = false;
//         isInputActive = false;
//         isShowingExamplesActive = false;
//     }
//         DrawRectangleRec(createButton, WHITE);
//         DrawText("Create", createButton.x + (createButton.width - MeasureText("Create", 20)) / 2, createButton.y + 10, 20, BLACK);
       
//         DrawRectangleRec(randomButton, WHITE);
//         DrawText("Random", randomButton.x + (randomButton.width - MeasureText("Random", 20)) / 2, randomButton.y + 10, 20, BLACK);
    
//         DrawRectangleRec(exampleButton, WHITE);
//         DrawText("Examples", exampleButton.x + (exampleButton.width - MeasureText("Examples", 20)) / 2, exampleButton.y + 10, 20, BLACK);

//         DrawRectangleRec(inputButton, WHITE);
//         DrawText("Input", inputButton.x + (inputButton.width - MeasureText("Input", 20)) / 2, inputButton.y + 10, 20, BLACK);
       
//         DrawRectangleRec(fileButton, WHITE); 
//         DrawText("File", fileButton.x + (fileButton.width - MeasureText("File", 20)) / 2, fileButton.y + 10, 20, BLACK);

//         DrawRectangleRec(editButton, WHITE); 
//         DrawText("Edit", editButton.x + (editButton.width - MeasureText("Edit", 20)) / 2, editButton.y + 10, 20, BLACK);
       
//         DrawRectangleRec(mstButton, WHITE);
//         DrawText("MST", mstButton.x + (mstButton.width - MeasureText("MST", 20)) / 2, mstButton.y + 10, 20, BLACK);

//         if (showExampleButtons) {
//             DrawRectangleRec(k5Button, WHITE);
//             DrawText("K5", k5Button.x + 5, k5Button.y + 5, 20, BLACK);
    
//             DrawRectangleRec(c6Button, WHITE);
//             DrawText("C6", c6Button.x + 5, c6Button.y + 5, 20, BLACK);
    
//             DrawRectangleRec(p4Button, WHITE);
//             DrawText("P4", p4Button.x + 5, p4Button.y + 5, 20, BLACK);
    
//             DrawRectangleRec(s7Button, WHITE);
//             DrawText("S7", s7Button.x + 5, s7Button.y + 5, 20, BLACK);
    
//             if (CheckCollisionPointRec(GetMousePosition(), k5Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 generateCompleteGraph(edges, 5);
//                 nodePositions.resize(5);
//                 for (int i = 0; i < 5; ++i) {
//                     float angle = 2.0f * PI * i / 5;
//                     float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//                     nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//                 }
//                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight); // <<--- GỌI Ở ĐÂY
//                 graphDrawn = true;
//                 showExampleButtons = false;
//                 inputMode = false;
//                 exampleNumNodes = 5;
//                 exampleNumEdges = 10;
//                 numNodesStr = "";
//                 numEdgesStr = "";
//             }
    
//             if (CheckCollisionPointRec(GetMousePosition(), c6Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 generateCycleGraph(edges, 6);
//                 nodePositions.resize(6);
//                 for (int i = 0; i < 6; ++i) {
//                     float angle = 2.0f * PI * i / 6;
//                     float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//                     nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//                 }
//                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight); // <<--- GỌI Ở ĐÂY
//                 graphDrawn = true;
//                 showExampleButtons = false;
//                 inputMode = false;
//                 exampleNumNodes = 6;
//                 exampleNumEdges = 6;
//                 numNodesStr = "";
//                 numEdgesStr = "";
//             }
    
//             if (CheckCollisionPointRec(GetMousePosition(), p4Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 generatePathGraph(edges, 4, nodePositions, screenWidth, screenHeight);
//                 nodePositions.resize(4);
//                 for (int i = 0; i < 4; ++i) {
//                     float angle = 2.0f * PI * i / 4;
//                     float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//                     nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//                 }
//                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight); // <<--- GỌI Ở ĐÂY
//                 graphDrawn = true;
//                 showExampleButtons = false;
//                 inputMode = false;
//                 exampleNumNodes = 4;
//                 exampleNumEdges = 3;
//                 numNodesStr = "";
//                 numEdgesStr = "";
//             }
    
//             if (CheckCollisionPointRec(GetMousePosition(), s7Button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                 generateStarGraph(edges, 7);
//                 nodePositions.resize(7);
//                 for (int i = 0; i < 7; ++i) {
//                     float angle = 2.0f * PI * i / 7;
//                     float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//                     nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//                 }
//                 graphDrawn = true;
//                 showExampleButtons = false;
//                 inputMode = false;
//                 exampleNumNodes = 7;
//                 exampleNumEdges = 6;
//                 numNodesStr = "";
//                 numEdgesStr = "";
//             }
//         }
        
//         if (inputMode && randomButtonClicked) {
//             edges.clear();
//             nodePositions.clear();
//             int numNodes = GetRandomValue(2, 7);
//             int numEdges = GetRandomValue(numNodes - 1, numNodes * (numNodes - 1) / 2);
//             generateRandomGraph(edges, numNodes, numEdges);
    
//             nodePositions.resize(numNodes);
//             for (int i = 0; i < numNodes; ++i) {
//                 float angle = 2.0f * PI * i / numNodes;
//                 float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//                 nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//             }
    
//             graphDrawn = true;
//             inputMode = false;
//             randomButtonClicked = false;
//             numNodesStr = "";
//             numEdgesStr = "";
//         }
    
//         if (isCreatingActive) {
//             if (canCreateGraph) {
//                 DrawText("N:", nodesInput.x - 25, nodesInput.y + 5, 20, WHITE);
//                 DrawText("E:", edgesInput.x - 25, edgesInput.y + 5, 20, WHITE);
    
//                 DrawRectangleRec(nodesInput, LIGHTGRAY);
//                 DrawText(numNodesStr.c_str(), nodesInput.x + 5, nodesInput.y + 5, 20, BLACK);
//                 if (CheckCollisionPointRec(GetMousePosition(), nodesInput) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                     nodesFocused = true;
//                     edgesFocused = false;
//                 }
//                 if (nodesFocused) {
//                     int key = GetCharPressed();
//                     if (key >= 32 && key <= 125) {
//                         numNodesStr += (char)key;
//                     }
//                     if (IsKeyPressed(KEY_BACKSPACE) && numNodesStr.length() > 0) {
//                         numNodesStr.pop_back();
//                     }
//                 }
    
//                 DrawRectangleRec(edgesInput, LIGHTGRAY);
//                 DrawText(numEdgesStr.c_str(), edgesInput.x + 5, edgesInput.y + 5, 20, BLACK);
//                 if (CheckCollisionPointRec(GetMousePosition(), edgesInput) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//                     edgesFocused = true;
//                     nodesFocused = false;
//                 }
//                 if (edgesFocused) {
//                     int key = GetCharPressed();
//                     if (key >= 32 && key <= 125) {
//                         numEdgesStr += (char)key;
//                     }
//                     if (IsKeyPressed(KEY_BACKSPACE) && numEdgesStr.length() > 0) {
//                         numEdgesStr.pop_back();
//                     }
//                 }
//             }
    
//             if (IsKeyPressed(KEY_ENTER)) {
//                 if (numNodesStr == "" || numEdgesStr == "") {
//                     canCreateGraph = false;
//                     showError = true;
//                     errorMessage = "Please enter Nodes and Edges.";
//                 } else {
//                     int numNodes = std::stoi(numNodesStr);
//                     int numEdges = std::stoi(numEdgesStr);
    
//                     if (numNodes <= 0 || numEdges < 0 || numEdges > numNodes * (numNodes - 1) / 2 || numEdges < numNodes - 1) {
//                         canCreateGraph = false;
//                         showError = true;
//                         errorMessage = "Can't create a graph.";
//                     } else {
//                         edges.clear();
//                         srand(time(0));
//                         while (edges.size() < numEdges) {
//                             int from = GetRandomValue(1, numNodes);
//                             int to = GetRandomValue(1, numNodes);
//                             int weight = GetRandomValue(1, 10);
//                             if (from != to && !edgeExists(edges, from, to)) {
//                                 edges.push_back({ from, to, weight });
//                             }
//                         }
    
//                         if (!isGraphConnected(edges, numNodes)) {
//                             canCreateGraph = false;
//                             showError = true;
//                             errorMessage = "Can't create connected graph with this data.";
//                         } else {
//                             nodePositions.resize(numNodes);
//                             int nodeRadius = 20;
//                             float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//                             for (int i = 0; i < numNodes; ++i) {
//                                 float angle = 2.0f * PI * i / numNodes;
//                                 nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//                             }
//                             graphDrawn = true;
//                             inputMode = false;
//                         }
//                     }
//                 }
//             }
//         }
//     }
//     if (showMatrixInput && !isEditingGraph) {
//         DrawRectangleRec(inputWindow, LIGHTGRAY);
//         DrawText("Input Adjacency Matrix", inputWindow.x + 10, inputWindow.y + 10, 20, BLACK);
//         DrawRectangleRec(matrixInputRect, WHITE);
    
//         // Vẽ thông báo lỗi (nếu có)
//         DrawText(errorMessageInput.c_str(), matrixInputRect.x + 5, matrixInputRect.y + 5, 20, RED);
    
//         // Vẽ con trỏ nhấp nháy
//         cursorTimer += GetFrameTime();
//         if (cursorTimer >= 0.5f) { 
//            cursorTimer = 0.0f;
//         }
//         if (matrixInputFocused && cursorTimer < 0.25f) { // Vẽ con trỏ khi focused và trong nửa đầu chu kỳ
//            // Tính toán vị trí x và y của con trỏ chính xác hơn
//            Vector2 cursorPosition = GetCursorScreenPos(matrixInput, cursorRow, cursorColumn, matrixInputRect);
//            DrawLine(cursorPosition.x, cursorPosition.y, cursorPosition.x, cursorPosition.y + 20, BLACK);
//        }
        
    
//         DrawText(matrixInput.c_str(), matrixInputRect.x + 5, matrixInputRect.y + 5, 20, BLACK);
    
//         if (CheckCollisionPointRec(GetMousePosition(), matrixInputRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             matrixInputFocused = true;
//         }
    
//         if (matrixInputFocused) {
//             int key = GetCharPressed();
//             if (key >= 32 && key <= 125) {
//                 matrixInput.insert(GetCursorIndex(matrixInput, cursorRow, cursorColumn), 1, (char)key);
//                 cursorColumn++;
//             }
//             if (IsKeyPressed(KEY_BACKSPACE) && (cursorColumn > 0 || cursorRow > 0)) {
//                 int deleteIndex = GetCursorIndex(matrixInput, cursorRow, cursorColumn) - 1;
//                 if(matrixInput[deleteIndex] == '\n'){
//                     cursorRow--;
//                     int index = GetCursorIndex(matrixInput, cursorRow, INT_MAX);
//                     cursorColumn = GetColFromIndex(matrixInput, index);
//                 } else{
//                     cursorColumn--;
//                 }
//                 matrixInput.erase(deleteIndex, 1);
//             }
//             if (IsKeyPressed(KEY_LEFT) && (cursorColumn > 0 || cursorRow > 0)) {
//                 if (cursorColumn > 0){
//                     cursorColumn--;
//                 }else{
//                     cursorRow--;
//                     int index = GetCursorIndex(matrixInput, cursorRow, INT_MAX);
//                     cursorColumn = GetColFromIndex(matrixInput, index);
//                 }
//             }
//             if (IsKeyPressed(KEY_RIGHT) && GetCursorIndex(matrixInput, cursorRow, cursorColumn) < matrixInput.length()) {
    
//                 if (matrixInput[GetCursorIndex(matrixInput, cursorRow, cursorColumn)] == '\n'){
//                     cursorRow++;
//                     cursorColumn = 0;
//                 }else{
//                     cursorColumn++;
//                 }
//             }
//             if (IsKeyPressed(KEY_UP) && cursorRow > 0) {
//                 cursorRow--;
//                 cursorColumn = std::min(cursorColumn, GetColFromIndex(matrixInput, GetCursorIndex(matrixInput, cursorRow, INT_MAX)));
//             }
//             if (IsKeyPressed(KEY_DOWN) && GetCursorIndex(matrixInput, cursorRow + 1, 0) <= matrixInput.length()) {
//                 cursorRow++;
//                 cursorColumn = std::min(cursorColumn, GetColFromIndex(matrixInput, GetCursorIndex(matrixInput, cursorRow, INT_MAX)));
//             }
//             if (IsKeyPressed(KEY_ENTER)) {
//                 matrixInput.insert(GetCursorIndex(matrixInput, cursorRow, cursorColumn), 1, '\n');
//                 cursorRow++;
//                 cursorColumn = 0;
//             }
//         }
       
//         // Nút Submit
//         DrawRectangleRec(submitButton, WHITE);
//         DrawText("Submit", submitButton.x + 10, submitButton.y + 10, 20, BLACK);
//         if (CheckCollisionPointRec(GetMousePosition(), submitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             std::vector<std::vector<int>> adjacencyMatrix;
//             int numNodes;
//             errorMessageInput = ValidateMatrixInput(matrixInput, adjacencyMatrix, numNodes);
    
//             if (errorMessageInput.empty()) {
//                 // Ma trận hợp lệ, vẽ đồ thị
//                 edges.clear();
//                 nodePositions.clear();
//                 nodePositions.resize(numNodes);
    
//             // Tạo danh sách cạnh từ ma trận
//             for (int i = 0; i < numNodes; ++i) {
//                 for (int j = i + 1; j < numNodes; ++j) {
//                     if (adjacencyMatrix[i][j] != 0) {
//                         edges.push_back({ i + 1, j + 1, adjacencyMatrix[i][j] });
//                     }
//                 }
//             }
//     // Tính toán vị trí nút
//     float layoutRadius = std::min(screenWidth, screenHeight) / 8.0f;
//     for (int i = 0; i < numNodes; ++i) {
//        float angle = 2.0f * PI * i / numNodes;
//        nodePositions[i] = { screenWidth / 2.0f + layoutRadius * cosf(angle), screenHeight / 2.0f + layoutRadius * sinf(angle) };
//     }
//     UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight); // <<--- GỌI Ở ĐÂY
//     graphDrawn = true;
//     showMatrixInput = false;
//     matrixInputFocused = false;
//     }
//     }
//         // Nút Close
//         DrawRectangleRec(closeButton, WHITE);
//         DrawText("Close", closeButton.x + 10, closeButton.y + 10, 20, BLACK);
//         if (CheckCollisionPointRec(GetMousePosition(), closeButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             showMatrixInput = false;
//             matrixInputFocused = false;
//             matrixInput = ""; 
//             cursorColumn = 0; 
//             cursorRow = 0;
//             errorMessageInput = ""; 
//         }
    
    
//     if (showError && inputMode) {
//         DrawText(errorMessage.c_str(), createButton.x + createButton.width + 30, createButton.y + 10, 20, RED);
//     }
//     }
//     if (CheckCollisionPointRec(GetMousePosition(),editButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
//     {
//         if (graphDrawn)
//         {
//         isEditingGraph = false;
//         currentTool = TOOL_NONE;
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         weightInputBuffer = "";
//         isDraggingNode = false;
//         isCreating = false;
//         isRandomizing = false;
//         isShowingExamples = false;
//         showMatrixInput = false;
//         showMSTMenu = false;
//         isFileActive = false;
//         isCreatingActive = false; 
//         isRandomizingActive = false;
//         isShowingExamplesActive = false;
//         isInputActive = false;
//         showExampleButtons = false; 
//         showError = false;
//         showMSTError = false;
//         showFileError = false;
//         errorMessage = "";
//         mstErrorMessage = "";
//         fileErrorMessage = "";
//         errorMessageInput = "";
//         UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight);
//         // Bật chế độ Edit
//         isEditingGraph = true;
//         currentTool = TOOL_NONE; // Bắt đầu mà không chọn công cụ nào
//         selectedNodeIndex = -1;
//         selectedEdgeIndex = -1;
//         isEditingWeight = false;
//         isDraggingNode = false;
//         }
//         else
//         {
//             showError = true;
//             errorMessage = "Please create or load a graph first to edit.";
//             isEditingGraph = false;
//         }
//     }
//     if (isEditingGraph) {
//         // Vẽ thanh công cụ bên phải chẳng hạn
//         float uiPanelX = screenWidth - 130;
//         DrawRectangle(screenWidth, 0, 130, screenHeight, Fade(BLACK, 0.7f)); // Panel nền mờ

//         Rectangle addVertexBtnRect = { uiPanelX + 10, 10, 110, 30 };
//         Rectangle addEdgeBtnRect = { uiPanelX + 10, 50, 110, 30 };
//         Rectangle editWeightBtnRect = { uiPanelX + 10, 90, 110, 30 };
//         Rectangle moveNodeBtnRect = { uiPanelX + 10, 130, 110, 30 };
//         Rectangle deleteVertexBtnRect = { uiPanelX + 10, 170, 110, 30 };
//         Rectangle deleteEdgeButtonRect = { uiPanelX + 10, 210, 110, 30 }; 
//         Rectangle doneBtnRect = { uiPanelX + 10, screenHeight - 50, 110, 30 };
    
//         DrawRectangleRec(addVertexBtnRect, (currentTool == TOOL_ADD_VERTEX) ? ORANGE : LIGHTGRAY); DrawText("Add Vertex", addVertexBtnRect.x + 10, addVertexBtnRect.y + 7, 20, BLACK);
//         DrawRectangleRec(addEdgeBtnRect, (currentTool == TOOL_ADD_EDGE_START || currentTool == TOOL_ADD_EDGE_END) ? ORANGE : LIGHTGRAY); DrawText("Add Edge", addEdgeBtnRect.x + 10, addEdgeBtnRect.y + 7, 20, BLACK);
//         DrawRectangleRec(editWeightBtnRect, (currentTool == TOOL_EDIT_WEIGHT) ? ORANGE : LIGHTGRAY); DrawText("Edit Weight", editWeightBtnRect.x + 10, editWeightBtnRect.y + 7, 20, BLACK);
//         DrawRectangleRec(moveNodeBtnRect, (currentTool == TOOL_MOVE_VERTEX) ? ORANGE : LIGHTGRAY); DrawText("Move Node", moveNodeBtnRect.x+10, moveNodeBtnRect.y+7, 20, BLACK);
//         DrawRectangleRec(deleteVertexBtnRect, (currentTool == TOOL_DELETE_VERTEX) ? ORANGE : LIGHTGRAY); DrawText("Delete Vertex", deleteVertexBtnRect.x+5, deleteVertexBtnRect.y+7, 20, BLACK);
//         DrawRectangleRec(deleteEdgeButtonRect, (currentTool == TOOL_DELETE_EDGE) ? ORANGE : LIGHTGRAY); DrawText("Delete Edge", deleteEdgeButtonRect.x+10, deleteEdgeButtonRect.y+7, 20, BLACK);
//         DrawRectangleRec(doneBtnRect, RAYWHITE); DrawText("Done", doneBtnRect.x + 30, doneBtnRect.y + 7, 20, BLACK);
//         // Vẽ ô nhập trọng số nếu cần (Vẽ trên cùng)
//         if (isEditingWeight) {
//             DrawRectangleRec(weightInputRect, WHITE);
//             DrawRectangleLinesEx(weightInputRect, 1, BLACK);
//             DrawText(weightInputBuffer.c_str(), weightInputRect.x + 5, weightInputRect.y + 4, 20, BLACK);
//             // Vẽ con trỏ nhấp nháy (tùy chọn)
//             if (((int)(GetTime() * 2.0f)) % 2 == 0) { // Nhấp nháy mỗi nửa giây
//                DrawText("|", weightInputRect.x + 5 + MeasureText(weightInputBuffer.c_str(), 20), weightInputRect.y + 4, 20, BLACK);
//             }
//         }
//          // Vẽ thông báo hướng dẫn nhỏ
//          const char* hintText = "";
//            switch(currentTool){
//                 case TOOL_ADD_VERTEX: hintText = "Click empty space to add vertex."; break;
//                 case TOOL_ADD_EDGE_START: hintText = "Click first vertex for edge."; break;
//                 case TOOL_ADD_EDGE_END: hintText = "Click second vertex for edge."; break;
//                 case TOOL_EDIT_WEIGHT: hintText = "Click near edge weight to edit."; break;
//                 case TOOL_MOVE_VERTEX: hintText = "Click and drag a vertex."; break;
//                 case TOOL_DELETE_VERTEX: hintText = "Click a vertex to delete."; break;
//                 case TOOL_DELETE_EDGE: hintText = "Click near an edge to delete."; break;
//                 default: hintText = "Select a tool from the right panel."; break;
//            }
//           DrawText(hintText, 10, screenHeight - 30, 20, RAYWHITE);
//     }
//     EndDrawing();
// }

// CloseWindow();
// return 0;
// }