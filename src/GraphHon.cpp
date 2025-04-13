#include "GraphApp.h" // Include header file

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
    // Lerp between minValue and maxValue based on normalized value
    return minValue + (maxValue - minValue) * value;
}
void Slider::draw() {
    // Draw label
    DrawText(label.c_str(), rect.x, rect.y - 20, 16, RAYWHITE);

    // Draw slider track
     DrawRectangleRounded(rect, 0.3f, 4, LIGHTGRAY); // Ví dụ: Bo góc nhẹ
     Vector2 knobCenter = { knob.x + knob.width / 2.0f, knob.y + knob.height / 2.0f };
     float knobRadius = knob.height / 1.8f; // Bán kính nhỏ hơn chiều cao một chút cho đẹp
 
     // Vẽ phần đã tô màu của thanh trượt (từ đầu đến giữa núm)
     float filledWidth = knobCenter.x - rect.x;
     filledWidth = Clamp(filledWidth, 0, rect.width); // Đảm bảo không vẽ ra ngoài
     if (filledWidth > 0) {
         // Vẽ phần đã tô màu cũng bo tròn
         DrawRectangleRounded({rect.x, rect.y, filledWidth, rect.height}, 0.3f, 4, SKYBLUE); // Dùng màu khác (vd: SKYBLUE)
     }
 
     // Xác định màu sắc cho núm dựa trên trạng thái
     Color knobColor = BLUE; // Màu mặc định
     bool knobHovered = CheckCollisionPointCircle(GetMousePosition(), knobCenter, knobRadius);
     if (isDragging) {
         knobColor = DARKBLUE; // Màu khi đang kéo
     } else if (knobHovered) {
         knobColor = SKYBLUE;  // Màu khi di chuột qua
     }
 
     // Vẽ núm hình tròn
     DrawCircleV(knobCenter, knobRadius, knobColor);
    // Draw value text
     std::stringstream ss;
     ss << std::fixed << std::setprecision(2) << getValue();
     std::string valueStr = ss.str();

     int valueFontSize = 16;
     Vector2 valueTextSize = MeasureTextEx(GetFontDefault(), valueStr.c_str(), valueFontSize, 1);
     Rectangle valueTextRect = { rect.x + rect.width + 15, rect.y + (rect.height - valueFontSize)/2.0f, valueTextSize.x + 10, (float)valueFontSize + 4}; // Điều chỉnh vị trí Y và Width/Height

    if (editValue) {
         valueTextRect.height = 20;
         DrawRectangleRec(valueTextRect, WHITE);
         DrawRectangleLinesEx(valueTextRect, 1, BLACK);
         DrawText(editBuffer.c_str(), valueTextRect.x + 5, valueTextRect.y , 16, BLACK);
          // Draw blinking cursor
         if (((int)(GetTime() * 2.0f)) % 2 == 0) {
              DrawText("|", valueTextRect.x + 5 + MeasureText(editBuffer.c_str(), 16), valueTextRect.y, 16, BLACK);
         }

    } else {
         DrawText(valueStr.c_str(), valueTextRect.x + 5, valueTextRect.y, 16, RAYWHITE);
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
void RunGraphApp() {
    // --- Di chuyển TẤT CẢ biến toàn cục vào đây ---
    const int screenWidth = 1600;
    const int screenHeight = 900;

    // State Variables
    bool isCreating = false; bool isRandomizing = false; bool isShowingExamples = false;
    bool isCreatingActive = false; bool isRandomizingActive = false; bool isShowingExamplesActive = false;
    bool isInputActive = false; bool isFileActive = false; bool graphDrawn = false;
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
        "1. MST = {}; Visited = {start_node}; PQ = {}",
        "2. Add edges from start_node to PQ",
        "3. while PQ not empty and MST size < N-1:",
        "4.   edge(u, v), weight = PQ.extract_min()",
        "5.   if v in Visited: continue",
        "6.   Add edge(u, v) to MST",        // <-- Dòng tương ứng khi thêm cạnh thành công
        "7.   Visited.add(v)",
        "8.   for each edge(v, w) to unvisited w:",
        "9.     Add edge(v, w) to PQ"
    };
    std::vector<std::string> kruskalPseudocode = {
        "1. MST = {}",
        "2. Init DSU for N nodes",
        "3. Sort all edges E by weight",
        "4. for each edge(u, v) in sorted E:",
        "5.   if findSet(u) != findSet(v):", // <-- Dòng tương ứng khi kiểm tra và thấy hợp lệ
        "6.     Add edge(u, v) to MST",     // <-- Dòng tương ứng khi thêm cạnh thành công
        "7.     unionSets(u, v)",
        "8.     if MST size == N-1: break"
    };
    int currentHighlightLine = -1; // Index của dòng mã giả cần highlight (-1 là không có)

    // MST Scroll variables
    float mstPseudocodeScrollY = 0.0f;
    float mstPseudocodeTotalHeight = 0.0f;
    bool isDraggingScrollbar = false;
    float scrollbarMouseOffsetY = 0.0f;
    // -------------------------------------------------
    
    InitWindow(screenWidth, screenHeight, "Graph MST Visualizer");
    SetTargetFPS(60);
     // Left Panel Buttons
     const float buttonWidth = 100.0f;
     const float buttonHeight = 30.0f;
     const float bottomPadding = 10.0f; // Khoảng cách từ cạnh dưới màn hình
     const float leftPadding = 10.0f;   // Khoảng cách từ cạnh trái màn hình
     const float buttonSpacing = 10.0f; // Khoảng cách giữa các nút
     const float buttonY = screenHeight - buttonHeight - bottomPadding;
     // Định nghĩa vị trí các nút theo hàng ngang từ trái sang phải
     Rectangle createButton = {leftPadding, buttonY, buttonWidth, buttonHeight};
     Rectangle randomButton = {createButton.x + createButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle exampleButton = {randomButton.x + randomButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle inputButton = {exampleButton.x + exampleButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle fileButton = {inputButton.x + inputButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle editButton = {fileButton.x + fileButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     Rectangle mstButton = {editButton.x + editButton.width + buttonSpacing, buttonY, buttonWidth, buttonHeight};
     // Create Mode Inputs
     Rectangle nodesInputRect = { 30, 10, 100, 30 };
     Rectangle edgesInputRect = { nodesInputRect.x + nodesInputRect.width + 30, 10, 100, 30 };
     // Example Buttons (positions relative to exampleButton)
     Rectangle k5Button = { 30, 10, 60, 30 };
     Rectangle c6Button = { k5Button.x + k5Button.width + 10, 10, 60, 30 };
     Rectangle p4Button = { c6Button.x + c6Button.width + 10, 10, 60, 30 };
     Rectangle s7Button = { p4Button.x + p4Button.width + 10, 10, 60, 30 };
     // Matrix Input Window Elements
     Rectangle inputWindowRect = {screenWidth / 4.0f, screenHeight / 4.0f, screenWidth / 2.0f, screenHeight / 2.0f};
     Rectangle matrixInputAreaRect = {inputWindowRect.x + 10, inputWindowRect.y + 40, inputWindowRect.width - 20, inputWindowRect.height - 90};
     Rectangle submitMatrixButton = {inputWindowRect.x + 10, inputWindowRect.y + inputWindowRect.height - 40, 80, 30};
     Rectangle closeMatrixButton = {submitMatrixButton.x + submitMatrixButton.width + 10, submitMatrixButton.y, 80, 30};
 
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
     Rectangle backButton = {kruskalButton.x + mstButtonWidth + mstButtonSpacing, mstButtonsY, (float)mstButtonWidth, (float)mstButtonHeight};
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
 
 
     // Initialize Camera
     graphCamera.target = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
     graphCamera.offset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
     graphCamera.rotation = 0.0f;
     graphCamera.zoom = 1.0f;

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
                 //ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                 isInputActive = false;
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
                 //ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                 isInputActive = false;
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
                     float layoutRadius = std::min(screenWidth, screenHeight) / 3.0f;
                     positionNodesInCircle(nodePositions, r_numNodes, screenWidth / 2.0f, screenHeight / 2.0f, layoutRadius);
                     graphDrawn = true;
                     UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 0.0f, 0.0f); // Update camera for new graph
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
                 //ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                 isInputActive = false;
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
             else if (CheckCollisionPointRec(mousePos, inputButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 clickedOnUI = true;
                 // ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                 isInputActive = false;
                 isFileActive = false;
             
                 // Reset Editing state
                 selectedNodeIndex = -1;
                 selectedEdgeIndex = -1;
                 isDraggingNode = false;
                 isEditingWeight = false;
                 weightInputBuffer = ""; 
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
                 // ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                 isInputActive = false;
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
                // isFileActive = false; // Reset flag
                 // --- End TinyFileDialog ---
             }
              else if (CheckCollisionPointRec(mousePos, editButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                  clickedOnUI = true;
                  if (graphDrawn && !nodePositions.empty()) {
                      // Enter Edit Mode
                      // ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                      // Keep graphDrawn = true
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
                      isInputActive = false;
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
                      // Show error: Cannot edit without a graph
                      // ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                      isInputActive = false;
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
                     // Enter MST Menu Mode
                     // ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
                     // Keep graph data, graphDrawn = true
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
                     isInputActive = false;
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
                     // Show error: Cannot run MST without a graph
                     // ResetStates(isCreating, isRandomizing, isShowingExamples, showMatrixInput, graphDrawn, numNodesStr, numEdgesStr, matrixInput, nodesFocused, edgesFocused, showMSTMenu, isEditingGraph, currentTool, showFileError, fileErrorMessage, showError, errorMessage, showMSTError, mstErrorMessage);
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
                     isInputActive = false;
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
                             UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 0.0f, 0.0f);
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
                                 UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 0.0f, 0.0f);
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
                         currentHighlightLine = 0; // Highlight dòng khởi tạo đầu tiên
                         if (showMSTError) currentHighlightLine = -1; // Bỏ highlight nếu có lỗi
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
                         currentHighlightLine = 0; // Highlight dòng khởi tạo đầu tiên
                         if (showMSTError) currentHighlightLine = -1; // Bỏ highlight nếu có lỗi
                     }
                 } else if (CheckCollisionPointRec(mousePos, backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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
                          UpdateGraphCamera(graphCamera, nodePositions, 20.0f, screenWidth, screenHeight, 0.0f, editPanelWidth); // Pass UI widths
                      }
                 }

                 // Reset states when changing tools
                 if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                      isEditingWeight = false;
                      isDraggingNode = false;
                      // Keep selectedNodeIndex if switching between ADD_EDGE_START/END? No, reset generally safer.
                      selectedNodeIndex = -1; // Reset unless specific tool logic needs it
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
                         if (!mstDoneDrawing) { // Chỉ cập nhật nếu chưa phải là bước cuối cùng hoàn tất
                            if (usePrim) currentHighlightLine = 5;
                            else if (useKruskal) currentHighlightLine = 5;
                            else currentHighlightLine = -1;
                        } else {
                            // Highlight dòng cuối/return khi xong?
                             if (usePrim) currentHighlightLine = -1; // Hoặc index dòng return
                             else if (useKruskal) currentHighlightLine = -1; // Hoặc index dòng return
                        }
                     } else {
                          mstDoneDrawing = true; // Should already be set, but just in case
                          if (usePrim) currentHighlightLine = -1; // Hoặc index dòng return
                          else if (useKruskal) currentHighlightLine = -1; // Hoặc index dòng return
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
        //DrawRectangle(0, 0, 120, screenHeight, Fade(BLACK, 0.5f));
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
               DrawText(hintText, 10, screenHeight - 60, 20, RAYWHITE);

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
            DrawRectangleRec(mstMenuRect, LIGHTGRAY);
            DrawRectangleLinesEx(mstMenuRect, 2, BLACK);
            DrawText("Minimum Spanning Tree", mstMenuRect.x + 10, mstMenuRect.y + 10, 20, BLACK);
           
            float titleHeight = 30; // Khoảng trống cho tiêu đề
            float bottomButtonHeight = mstButtonHeight + 10; // Khoảng trống cho nút dưới cùng + padding
            float availableHeight = mstMenuRect.height - titleHeight - bottomButtonHeight - 20; // Chiều cao khả dụng cho graph và control panel (-20 padding trên dưới)
            float controlPanelWidth = 220; // Chiều rộng cho panel điều khiển bên phải
            float panelPadding = 10;
            const float controlButtonHeight = 30.0f; // Chiều cao chuẩn cho các nút control

            Rectangle graphArea = {
                mstMenuRect.x + panelPadding,
                mstMenuRect.y + titleHeight + panelPadding,
                mstMenuRect.width - controlPanelWidth - 3 * panelPadding, // Trừ panel phải và các padding
                availableHeight
            };
            DrawRectangleLinesEx(graphArea, 1, BLUE); // Visualize graph area
            
            Rectangle controlPanelRect = {
                graphArea.x + graphArea.width + panelPadding,
                graphArea.y,
                controlPanelWidth,
                availableHeight
            };
            DrawRectangleRec(controlPanelRect, { 200, 200, 200, 150 }); // Optional background for control panel
            DrawRectangleLinesEx(controlPanelRect, 1, RED); // Visualize control panel area
            
            float currentControlY = controlPanelRect.y + 10; // Y bắt đầu trong panel
            int controlFontSize = 20;
            float controlItemSpacing = 15; // Khoảng cách giữa các item trong panel

            // 1. Total Weight
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
             Rectangle pseudocodeViewRect = {
                controlPanelRect.x + panelPadding,
                currentControlY, // Bắt đầu dưới các control
                controlPanelRect.width - 2 * panelPadding,
                controlPanelRect.y + controlPanelRect.height - currentControlY - panelPadding // Chiều cao còn lại
            };
             DrawRectangle(pseudocodeViewRect.x - 2, pseudocodeViewRect.y - 2, pseudocodeViewRect.width + 4, pseudocodeViewRect.height + 4, Fade(BLACK, 0.1f)); // Nền mờ nhẹ (tùy chọn)
             DrawRectangleLinesEx(pseudocodeViewRect, 1, GRAY); // Viền (tùy chọn)

            std::vector<std::string>* codeToShow = nullptr;
            if (usePrim) codeToShow = &primPseudocode;
            else if (useKruskal) codeToShow = &kruskalPseudocode;

            if (codeToShow) {
                int pseudoFontSize = 16;
                float pseudoLineHeight = (float)pseudoFontSize * 1.25f; // Tăng khoảng cách dòng
                float textOffsetY = pseudocodeViewRect.y + 5;

                 // (Tùy chọn: Thêm logic scroll nếu mã giả quá dài)
                 BeginScissorMode((int)pseudocodeViewRect.x, (int)pseudocodeViewRect.y, (int)pseudocodeViewRect.width, (int)pseudocodeViewRect.height);
                 DrawRectangleRec(pseudocodeViewRect, Fade(BLACK, 0.1f));
                 for (int i = 0; i < codeToShow->size(); ++i) {
                    // Chỉ vẽ nếu còn nằm trong khu vực hiển thị (vẫn nên giữ check này)
                    if (textOffsetY < pseudocodeViewRect.y + pseudocodeViewRect.height - pseudoFontSize) {
                         // Highlight (nếu có, bỏ qua nếu chỉ hiển thị)
                         // if (i == highlightLine) { ... }
                         DrawText((*codeToShow)[i].c_str(), pseudocodeViewRect.x + 5, textOffsetY, pseudoFontSize, BLACK);
                    } else {
                        break;
                    }
                    textOffsetY += pseudoLineHeight;
                }
                 EndScissorMode();
             }
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
             DrawRectangleRec(primButton, WHITE); DrawText("Prim", primButton.x + 10, primButton.y + 7, 20, BLACK);
             DrawRectangleRec(kruskalButton, WHITE); DrawText("Kruskal", kruskalButton.x + 10, kruskalButton.y + 7, 20, BLACK);
             DrawRectangleRec(backButton, WHITE); DrawText("Back", backButton.x + 10, backButton.y + 7, 20, BLACK);
        
         } // end if showMSTMenu
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


        EndDrawing();
    } // End main game loop

    CloseWindow();
}





