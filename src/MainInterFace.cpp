#include "MainInterface.h"
#include <iostream>

MainInterface::MainInterface() {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Data Structure Visualization");
    SetTargetFPS(60);

    currentState = ScreenState::MAIN_MENU;
    linkedListProgram = nullptr;
    hashTableProgram = nullptr; // Initialize HashTableMain pointer

    LoadTextures();

    // Define button sizes and positions
    float buttonWidth = 300;  // Adjusted to match the image
    float buttonHeight = 200; // Adjusted to match the image
    float paddingX = 100;     // Horizontal padding between buttons
    float paddingY = 50;      // Vertical padding between buttons

    // Calculate the starting position to center the 2x2 grid
    float gridWidth = 2 * buttonWidth + paddingX;  // 700 pixels
    float gridHeight = 2 * buttonHeight + paddingY; // 450 pixels
    float startX = (screenWidth - gridWidth) / 2;  // 450 pixels
    float startY = (screenHeight - gridHeight) / 2; // 225 pixels

    // Define button rectangles
    hashTableRect = { startX, startY, buttonWidth, buttonHeight };
    linkedListRect = { startX + buttonWidth + paddingX, startY, buttonWidth, buttonHeight };
    avlTreeRect = { startX, startY + buttonHeight + paddingY, buttonWidth, buttonHeight };
    graphRect = { startX + buttonWidth + paddingX, startY + buttonHeight + paddingY, buttonWidth, buttonHeight };
}

MainInterface::~MainInterface() {
    if (linkedListProgram) {
        delete linkedListProgram;
    }
    if (hashTableProgram) {
        delete hashTableProgram;
    }
    UnloadTextures();
    CloseWindow();
}

void MainInterface::LoadTextures() {
    // Load the images as textures
    hashTableButton = LoadTexture("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\images\\HashTable.png");
    if (hashTableButton.id == 0) {
        std::cerr << "Failed to load HashTable.png\n";
    }
    linkedListButton = LoadTexture("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\images\\LinkedList.png");
    if (linkedListButton.id == 0) {
        std::cerr << "Failed to load LinkedList.png\n";
    }
    avlTreeButton = LoadTexture("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\images\\AVLTree.png");
    if (avlTreeButton.id == 0) {
        std::cerr << "Failed to load AVLTree.png\n";
    }
    graphButton = LoadTexture("D:\\Downloads\\sproject\\DataStructureCS163\\resources\\images\\Graph.png");
    if (graphButton.id == 0) {
        std::cerr << "Failed to load Graph.png\n";
    }
}

void MainInterface::UnloadTextures() {
    UnloadTexture(hashTableButton);
    UnloadTexture(linkedListButton);
    UnloadTexture(avlTreeButton);
    UnloadTexture(graphButton);
}

void MainInterface::DrawInterface() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw title, centered horizontally
    const char* title = "Data Structure Visualization";
    int fontSize = 40;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), title, fontSize, 1);
    float titleX = (1600 - textSize.x) / 2; // Center horizontally
    DrawText(title, titleX, 30, fontSize, BLACK); // Adjusted y-position to 30

    // Check for mouse hover to apply the highlight effect
    Vector2 mousePos = GetMousePosition();
    float scale = 1.1f; // Scale factor for the hover effect (10% larger)

    // Hash Table Button
    bool isHashTableHovered = CheckCollisionPointRec(mousePos, hashTableRect);
    if (isHashTableHovered) {
        // Calculate scaled dimensions and position to keep the image centered
        float scaledWidth = hashTableRect.width * scale;
        float scaledHeight = hashTableRect.height * scale;
        float offsetX = (scaledWidth - hashTableRect.width) / 2;
        float offsetY = (scaledHeight - hashTableRect.height) / 2;
        DrawTexturePro(hashTableButton,
                       { 0, 0, (float)hashTableButton.width, (float)hashTableButton.height },
                       { hashTableRect.x - offsetX, hashTableRect.y - offsetY, scaledWidth, scaledHeight },
                       { 0, 0 }, 0, WHITE);
    } else {
        DrawTextureRec(hashTableButton, { 0, 0, (float)hashTableButton.width, (float)hashTableButton.height }, { hashTableRect.x, hashTableRect.y }, WHITE);
    }

    // Linked List Button
    bool isLinkedListHovered = CheckCollisionPointRec(mousePos, linkedListRect);
    if (isLinkedListHovered) {
        float scaledWidth = linkedListRect.width * scale;
        float scaledHeight = linkedListRect.height * scale;
        float offsetX = (scaledWidth - linkedListRect.width) / 2;
        float offsetY = (scaledHeight - linkedListRect.height) / 2;
        DrawTexturePro(linkedListButton,
                       { 0, 0, (float)linkedListButton.width, (float)linkedListButton.height },
                       { linkedListRect.x - offsetX, linkedListRect.y - offsetY, scaledWidth, scaledHeight },
                       { 0, 0 }, 0, WHITE);
    } else {
        DrawTextureRec(linkedListButton, { 0, 0, (float)linkedListButton.width, (float)linkedListButton.height }, { linkedListRect.x, linkedListRect.y }, WHITE);
    }

    // AVL Tree Button
    bool isAvlTreeHovered = CheckCollisionPointRec(mousePos, avlTreeRect);
    if (isAvlTreeHovered) {
        float scaledWidth = avlTreeRect.width * scale;
        float scaledHeight = avlTreeRect.height * scale;
        float offsetX = (scaledWidth - avlTreeRect.width) / 2;
        float offsetY = (scaledHeight - avlTreeRect.height) / 2;
        DrawTexturePro(avlTreeButton,
                       { 0, 0, (float)avlTreeButton.width, (float)avlTreeButton.height },
                       { avlTreeRect.x - offsetX, avlTreeRect.y - offsetY, scaledWidth, scaledHeight },
                       { 0, 0 }, 0, WHITE);
    } else {
        DrawTextureRec(avlTreeButton, { 0, 0, (float)avlTreeButton.width, (float)avlTreeButton.height }, { avlTreeRect.x, avlTreeRect.y }, WHITE);
    }

    // Graph Button
    bool isGraphHovered = CheckCollisionPointRec(mousePos, graphRect);
    if (isGraphHovered) {
        float scaledWidth = graphRect.width * scale;
        float scaledHeight = graphRect.height * scale;
        float offsetX = (scaledWidth - graphRect.width) / 2;
        float offsetY = (scaledHeight - graphRect.height) / 2;
        DrawTexturePro(graphButton,
                       { 0, 0, (float)graphButton.width, (float)graphButton.height },
                       { graphRect.x - offsetX, graphRect.y - offsetY, scaledWidth, scaledHeight },
                       { 0, 0 }, 0, WHITE);
    } else {
        DrawTextureRec(graphButton, { 0, 0, (float)graphButton.width, (float)graphButton.height }, { graphRect.x, graphRect.y }, WHITE);
    }

    EndDrawing();
}

void MainInterface::DrawPlaceholderScreen(const char* title) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw the placeholder screen
    DrawText(title, 600, 400, 40, BLACK);
    DrawText("Press ESC to return to the main menu", 550, 500, 20, DARKGRAY);

    EndDrawing();

    // Check for ESC key to return to the main menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        currentState = ScreenState::MAIN_MENU;
    }
}

void MainInterface::HandleButtonClicks() {
    if (currentState != ScreenState::MAIN_MENU) {
        return; // Only handle clicks in the main menu
    }

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, hashTableRect)) {
            currentState = ScreenState::HASH_TABLE;
            if (!hashTableProgram) {
                hashTableProgram = new HashTableMain();
            }
            std::cout << "Hash Table selected\n";
        }
        else if (CheckCollisionPointRec(mousePos, linkedListRect)) {
            currentState = ScreenState::LINKED_LIST;
            if (!linkedListProgram) {
                linkedListProgram = new SLLmain();
            }
            std::cout << "Linked List selected\n";
        }
        else if (CheckCollisionPointRec(mousePos, avlTreeRect)) {
            currentState = ScreenState::AVL_TREE;
            std::cout << "AVL Tree selected\n";
        }
        else if (CheckCollisionPointRec(mousePos, graphRect)) {
            currentState = ScreenState::GRAPH;
            std::cout << "Graph selected\n";
        }
    }
}

void MainInterface::Run() {
    while (!WindowShouldClose()) {
        switch (currentState) {
            case ScreenState::MAIN_MENU:
                HandleButtonClicks();
                DrawInterface();
                break;

            case ScreenState::HASH_TABLE:
                if (hashTableProgram) {
                    hashTableProgram->Run();
                    if (hashTableProgram->ShouldClose()) {
                        delete hashTableProgram;
                        hashTableProgram = nullptr;
                        currentState = ScreenState::MAIN_MENU;
                    }
                }
                break;

            case ScreenState::LINKED_LIST:
                if (linkedListProgram) {
                    linkedListProgram->Run();
                    if (linkedListProgram->ShouldClose()) {
                        delete linkedListProgram;
                        linkedListProgram = nullptr;
                        currentState = ScreenState::MAIN_MENU;
                    }
                }
                break;

            case ScreenState::AVL_TREE:
                DrawPlaceholderScreen("AVL Tree Visualization");
                break;

            case ScreenState::GRAPH:
                DrawPlaceholderScreen("Graph Visualization");
                break;
        }
    }
}

int main() {
    MainInterface interface;
    interface.Run();
    return 0;
}