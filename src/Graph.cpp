#include "Graph.h"
#include "GUI.h"
#include "Button.h"
#include "raylib.h"
#include <vector>
#include <string>

Graph::Graph(int size) : size(size) {
    vertices.resize(size);
}

Graph::~Graph() {
    ClearStates();
    for (auto& vertex : vertices) {
        while (vertex.edges) {
            Edge* temp = vertex.edges;
            vertex.edges = vertex.edges->next;
            delete temp;
        }
    }
}

Graph::Edge* Graph::CopyEdges(Edge* source) {
    if (!source) return nullptr;
    Edge* newEdge = new Edge(source->to, source->weight);
    newEdge->next = CopyEdges(source->next);
    return newEdge;
}

void* Graph::CopyState() {
    std::vector<Vertex>* newVertices = new std::vector<Vertex>(size);
    for (int i = 0; i < size; i++) {
        (*newVertices)[i].data = vertices[i].data;
        (*newVertices)[i].edges = CopyEdges(vertices[i].edges);
    }
    return newVertices;
}

void Graph::Draw(Font font, void* state, int x, int y) {
    std::vector<Vertex>* graphState = static_cast<std::vector<Vertex>*>(state);
    for (int i = 0; i < size; i++) {
        if ((*graphState)[i].data != -1) {
            DrawCircle(x + i * 100, y, 20, LIGHTGRAY);
            DrawTextEx(font, TextFormat("%d", (*graphState)[i].data), {static_cast<float>(x + i * 100 - 10), static_cast<float>(y - 10)}, 20, 1, DARKBLUE);
            Edge* edge = (*graphState)[i].edges;
            while (edge) {
                DrawLine(x + i * 100, y + 20, x + edge->to * 100, y + 20, BLACK);
                DrawTextEx(font, TextFormat("%d", edge->weight), {static_cast<float>(x + (i + edge->to) * 50), static_cast<float>(y + 25)}, 15, 1, RED);
                edge = edge->next;
            }
        }
    }
}

void Graph::ClearStates() {
    for (auto state : stepStates) {
        std::vector<Vertex>* graphState = static_cast<std::vector<Vertex>*>(state);
        for (auto& vertex : *graphState) {
            while (vertex.edges) {
                Edge* temp = vertex.edges;
                vertex.edges = vertex.edges->next;
                delete temp;
            }
        }
        delete graphState;
    }
    stepStates.clear();
}

void Graph::Initialize(int param) {
    ClearSteps();
    ResetAnimation();
    for (auto& vertex : vertices) {
        while (vertex.edges) {
            Edge* temp = vertex.edges;
            vertex.edges = vertex.edges->next;
            delete temp;
        }
        vertex.data = -1;
    }
    SaveStep("Initial state (empty)", CopyState());
}

void Graph::Add(int value) {
    for (int i = 0; i < size; i++) {
        if (vertices[i].data == -1) {
            vertices[i].data = value;
            SaveStep("Added vertex: " + std::to_string(value), CopyState());
            break;
        }
    }
}

void Graph::AddEdge(int from, int to, int weight) {
    if (from >= size || to >= size || vertices[from].data == -1 || vertices[to].data == -1) return;
    Edge* newEdge = new Edge(to, weight);
    if (!vertices[from].edges) {
        vertices[from].edges = newEdge;
    } else {
        Edge* current = vertices[from].edges;
        while (current->next) current = current->next;
        current->next = newEdge;
    }
    SaveStep("Added edge from " + std::to_string(from) + " to " + std::to_string(to), CopyState());
}

void Graph::Delete(int value) {
    for (int i = 0; i < size; i++) {
        if (vertices[i].data == value) {
            while (vertices[i].edges) { // Sửa 'vertex' thành 'vertices[i]'
                Edge* temp = vertices[i].edges;
                vertices[i].edges = vertices[i].edges->next; // Sửa 'vertex' thành 'vertices[i]'
                delete temp;
            }
            vertices[i].data = -1;
            SaveStep("Deleted vertex: " + std::to_string(value), CopyState());
            return;
        }
    }
}

void Graph::Update(int oldValue, int newValue) {
    for (int i = 0; i < size; i++) {
        if (vertices[i].data == oldValue) {
            vertices[i].data = newValue;
            SaveStep("Updated " + std::to_string(oldValue) + " to " + std::to_string(newValue), CopyState());
            return;
        }
    }
}

bool Graph::Search(int value) {
    ClearSteps();
    for (int i = 0; i < size; i++) {
        if (vertices[i].data != -1) {
            SaveStep("Searching at vertex " + std::to_string(i) + ": " + std::to_string(vertices[i].data), CopyState());
            if (vertices[i].data == value) {
                SaveStep("Found " + std::to_string(value), CopyState());
                return true;
            }
        }
    }
    SaveStep("Not found: " + std::to_string(value), CopyState());
    return false;
}

void Graph::DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen) {
    static char inputBuffer1[32] = "";
    static char inputBuffer2[32] = "";
    static int inputLength1 = 0;
    static int inputLength2 = 0;
    static bool inputActive1 = false;
    static bool inputActive2 = false;

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

    ClearBackground(PINK);
    DrawTextEx(font, "Graph", { 600.0f, 50.0f }, 40, 1, DARKGRAY);

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
        Initialize(0);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Add Edge", buttonX, buttonY + 70.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        AddEdge(0, value, 4); // Giả sử from = 0, weight = 4
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Delete", buttonX, buttonY + 140.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        Delete(value);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    if (DrawButton("Update", buttonX, buttonY + 210.0f, font, buttonClicked, buttonMessage)) {
        int oldValue = GetInputValue1();
        int newValue = GetInputValue2();
        Update(oldValue, newValue);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
        inputLength2 = 0; inputBuffer2[0] = '\0';
    }
    if (DrawButton("Search", buttonX, buttonY + 280.0f, font, buttonClicked, buttonMessage)) {
        int value = GetInputValue1();
        Search(value);
        ResetAnimation();
        inputLength1 = 0; inputBuffer1[0] = '\0';
    }
    DrawAnimation(font, 800, 200);
    DrawAnimationControls(font, buttonClicked, buttonMessage, this);
    DrawBackButton(font, buttonClicked, buttonMessage, currentScreen);
}