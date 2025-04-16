#include "Graph.h" // Include header file
#include "GraphMain.h" // <<< THÊM INCLUDE NÀY

// Include headers cần thiết CHỈ cho implementation
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <limits.h> // For INT_MAX

// --- Function Definitions ---

// -- Graph Utils --
bool Edge::operator<(const Edge& other) const { return weight < other.weight; }
bool edgeExists(const std::vector<Edge>& edges, int from, int to) {
    for (const auto& edge : edges) {
        if ((edge.from == from && edge.to == to) || (edge.from == to && edge.to == from)) {
            return true;
        }
    }
    return false;
}
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

// -- MST Algorithms --
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

// -- Graph Generators --
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

// -- Drawing Utils --
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
    DrawLineEx(start, end, thickness, color); // Keep it simple for now
}
void drawEdgeWeight(Vector2 start, Vector2 end, int weight) {
    Vector2 midPoint = Vector2Lerp(start, end, 0.5f); // Use Lerp for midpoint
    std::string weightText = std::to_string(weight);
    int fontSize = 18; // Consider making font size dynamic based on zoom?
    Vector2 textSize = MeasureTextEx(GetFontDefault(), weightText.c_str(), fontSize, 1); // Use MeasureTextEx

    // Calculate position to center the text background
    Vector2 textPosition = {
        midPoint.x - textSize.x / 2.0f,
        midPoint.y - textSize.y / 2.0f
    };

    // Padding for the background rectangle
    float padding = 4.0f;
    Rectangle bgRect = {
        textPosition.x - padding,
        textPosition.y - padding,
        textSize.x + 2 * padding,
        textSize.y + 2 * padding
    };

    DrawRectangleRec(bgRect, WHITE); // Draw background first
    DrawRectangleLinesEx(bgRect, 1.0f, GRAY);
    DrawText(weightText.c_str(), (int)textPosition.x, (int)textPosition.y, fontSize, BLACK); // Draw text on top
}
// -- Camera Utils --
void UpdateGraphCamera(Camera2D &camera, const std::vector<Vector2>& nodes, float nodeRadius, int screenWidth, int screenHeight, float uiLeftWidth, float uiRightWidth) {
    // Define safe area avoiding UI elements
    float padding = 80.0f; // Increased padding to avoid buttons
    float bottomUIPadding = 150.0f; // Extra padding for bottom buttons (Create, Random, etc.)
    float topUIPadding = 80.0f; // Padding for top input fields
    Rectangle safeArea = {
        uiLeftWidth + padding,
        topUIPadding,
        screenWidth - uiLeftWidth - uiRightWidth - 2 * padding,
        screenHeight - topUIPadding - bottomUIPadding - padding
    };

    // Ensure safe area has positive dimensions
    safeArea.width = std::max(1.0f, safeArea.width);
    safeArea.height = std::max(1.0f, safeArea.height);

    // Default camera state if no nodes
    Vector2 defaultTarget = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    Vector2 defaultOffset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    float defaultZoom = 1.0f;

    if (nodes.empty()) {
        camera.target = defaultTarget;
        camera.offset = defaultOffset;
        camera.zoom = defaultZoom;
        return;
    }

    // Calculate bounding box of nodes
    Vector2 minBounds = nodes[0];
    Vector2 maxBounds = nodes[0];
    for (const auto& nodePos : nodes) {
        minBounds.x = std::min(minBounds.x, nodePos.x);
        minBounds.y = std::min(minBounds.y, nodePos.y);
        maxBounds.x = std::max(maxBounds.x, nodePos.x);
        maxBounds.y = std::max(maxBounds.y, nodePos.y);
    }

    // Expand bounds to account for node size
    minBounds = Vector2SubtractValue(minBounds, nodeRadius * 1.5f);
    maxBounds = Vector2AddValue(maxBounds, nodeRadius * 1.5f);

    float graphWidth = maxBounds.x - minBounds.x;
    float graphHeight = maxBounds.y - minBounds.y;

    // Ensure minimum dimensions
    graphWidth = std::max(graphWidth, nodeRadius * 4.0f);
    graphHeight = std::max(graphHeight, nodeRadius * 4.0f);

    // Calculate zoom to fit graph in safe area
    float targetViewportRatio = 0.9f;
    float targetWidth = safeArea.width * targetViewportRatio;
    float targetHeight = safeArea.height * targetViewportRatio;

    float zoomX = (graphWidth > 0) ? (targetWidth / graphWidth) : defaultZoom;
    float zoomY = (graphHeight > 0) ? (targetHeight / graphHeight) : defaultZoom;
    float zoom = std::min(zoomX, zoomY);

    // Clamp zoom
    const float minZoom = 0.2f;
    const float maxZoom = 1.5f;
    zoom = Clamp(zoom, minZoom, maxZoom);

    // Calculate camera target (center of graph)
    Vector2 graphCenter = {
        minBounds.x + graphWidth / 2.0f,
        minBounds.y + graphHeight / 2.0f
    };

    // Calculate camera offset (center of safe area)
    Vector2 safeAreaCenter = {
        safeArea.x + safeArea.width / 2.0f,
        safeArea.y + safeArea.height / 2.0f
    };

    // Update camera
    camera.target = graphCenter;
    camera.offset = safeAreaCenter;
    camera.zoom = zoom;
}

// -- Input/Text Utils --
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
             int codepoint = 0;
             int bytesProcessed = 0;
             codepoint = GetCodepoint(&text[textIndex], &bytesProcessed);
             GlyphInfo glyph = GetGlyphInfo(GetFontDefault(), codepoint);
             currentX += (glyph.advanceX == 0) ? glyph.image.width : glyph.advanceX;
             currentX += GetFontDefault().glyphPadding;
             currentCol++;
             textIndex += bytesProcessed -1; // -1 because the loop adds 1 later
        }
         textIndex++;
    }
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

// -- File Utils --
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

// -- Slider Class Method Definitions --
Slider::Slider(Rectangle r, float minVal, float maxVal, std::string l)
    : rect(r), minValue(minVal), maxValue(maxVal), value(0.5f), // Default normalized value
      isDragging(false), label(std::move(l)), editValue(false), editBuffer("") {
    // Initial knob position based on default value
    knob = { rect.x + (rect.width - 15) * value, rect.y - 5, 15, rect.height + 10 };
}
void Slider::update() {
    Vector2 mousePos = GetMousePosition();

    // Tính toán lại thông tin núm tròn để kiểm tra va chạm
    Vector2 knobCenter = { knob.x + knob.width / 2.0f, knob.y + knob.height / 2.0f };
    float knobRadius = knob.height / 1.8f; // Dùng cùng bán kính như khi vẽ

    // --- THAY ĐỔI KIỂM TRA VA CHẠM ---
    // Check for starting drag using circle collision
    // if (!isDragging && CheckCollisionPointRec(mousePos, knob) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // CŨ
    if (!isDragging && CheckCollisionPointCircle(mousePos, knobCenter, knobRadius) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // MỚI
        isDragging = true;
        // Có thể thêm hiệu ứng nhỏ khi nhấn ở đây nếu muốn
    }
    // --- KẾT THÚC THAY ĐỔI ---

    // Handle dragging (logic giữ nguyên, nhưng cập nhật knob.x)
    if (isDragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        float newKnobCenterX = mousePos.x; // Kéo dựa vào tâm núm
        // Clamp tâm núm trong khoảng track cho phép
        float minKnobCenterX = rect.x + knobRadius;
        float maxKnobCenterX = rect.x + rect.width - knobRadius;
        float clampedKnobCenterX = Clamp(newKnobCenterX, minKnobCenterX, maxKnobCenterX);

        // Cập nhật lại vị trí knob.x dựa trên tâm mới
        knob.x = clampedKnobCenterX - knob.width / 2.0f;

        // Cập nhật giá trị value dựa trên vị trí tâm núm
        if ((maxKnobCenterX - minKnobCenterX) > 0) {
             value = (clampedKnobCenterX - minKnobCenterX) / (maxKnobCenterX - minKnobCenterX);
        } else {
             value = 0.0f;
        }
        value = Clamp(value, 0.0f, 1.0f);
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
    // Reverse the mapping: when value is 1, return 0.1 (fastest); when value is 0, return 2.0 (slowest)
    float reversedValue = 1.0f - value;// Invert the normalized value
    return minValue + (maxValue - minValue) * reversedValue; // Map to the range [0.1, 2.0]
}
void Slider::draw() {
     // Draw label above the slider
     DrawText(label.c_str(), rect.x, rect.y - 20, 16, RAYWHITE);

     // Draw slider track
     DrawRectangleRounded(rect, 0.3f, 4, LIGHTGRAY); // Rounded track
     Vector2 knobCenter = { knob.x + knob.width / 2.0f, knob.y + knob.height / 2.0f };
     float knobRadius = knob.height / 1.8f; // Knob radius for aesthetics
 
     // Draw filled portion of the track (from start to knob center)
     float filledWidth = knobCenter.x - rect.x;
     filledWidth = Clamp(filledWidth, 0, rect.width); // Ensure it doesn't draw outside
     if (filledWidth > 0) {
         DrawRectangleRounded({rect.x, rect.y, filledWidth, rect.height}, 0.3f, 4, SKYBLUE); // Filled portion in SKYBLUE
     }
 
     // Determine knob color based on state
     Color knobColor = BLUE; // Default color
     bool knobHovered = CheckCollisionPointCircle(GetMousePosition(), knobCenter, knobRadius);
     if (isDragging) {
         knobColor = DARKBLUE; // Color when dragging
     } else if (knobHovered) {
         knobColor = SKYBLUE;  // Color when hovering
     }
 
     // Draw circular knob
     DrawCircleV(knobCenter, knobRadius, knobColor);
 
     // Draw value text to the right of the slider
     std::stringstream ss;
     ss << std::fixed << std::setprecision(2) << getValue();
     std::string valueStr = ss.str();
     int valueFontSize = 16;
     Vector2 valueTextSize = MeasureTextEx(GetFontDefault(), valueStr.c_str(), valueFontSize, 1);
     Rectangle valueTextRect = { rect.x + rect.width + 15, rect.y + (rect.height - valueFontSize)/2.0f, valueTextSize.x + 10, (float)valueFontSize + 4};
     // Draw speed value below the slider
     std::string speedText = "Speed: " + valueStr + "s";
     int speedFontSize = 14;
     Vector2 speedTextSize = MeasureTextEx(GetFontDefault(), speedText.c_str(), speedFontSize, 1);
     float speedTextX = rect.x + (rect.width - speedTextSize.x) / 2.0f; // Center the text below the slider
     float speedTextY = rect.y + rect.height + 5; // Position below the slider
     DrawText(speedText.c_str(), speedTextX, speedTextY, speedFontSize, RAYWHITE);
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

