#include "Screen.h"
#include "raylib.h"
#include <iostream>
#include <cstring>

Screen::Screen() : currentScreen(ScreenType::WELCOME), inputActive(false) {
    memset(inputText, 0, sizeof(inputText));

    // Define UI elements for Visualization Screen
    inputBox = { 20.0f, 70.0f, 150.0f, 40.0f }; // Adjusted 'y' to 70.0f
    insertButton = { 180.0f, 20.0f, 80.0f, 40.0f };
    deleteButton = { 270.0f, 20.0f, 80.0f, 40.0f };
    searchButton = { 360.0f, 20.0f, 80.0f, 40.0f };
    printBox = { static_cast<float>(GetScreenWidth()) - 200.0f, 20.0f, 180.0f, 40.0f };

    prevStepButton = { 20.0f, static_cast<float>(GetScreenHeight()) - 50.0f, 100.0f, 40.0f };
    nextStepButton = { 130.0f, static_cast<float>(GetScreenHeight()) - 50.0f, 100.0f, 40.0f };
    pauseButton = { 240.0f, static_cast<float>(GetScreenHeight()) - 50.0f, 100.0f, 40.0f };
    skipButton = { 350.0f, static_cast<float>(GetScreenHeight()) - 50.0f, 100.0f, 40.0f };

    // Define buttons for Initialization Screen
    randomButton = { (GetScreenWidth() - 200) / 2.0f, 150.0f, 200.0f, 50.0f };
    manualButton = { (GetScreenWidth() - 200) / 2.0f, 220.0f, 200.0f, 50.0f };
    importButton = { (GetScreenWidth() - 200) / 2.0f, 290.0f, 200.0f, 50.0f };
}

Screen::~Screen() {}

void Screen::SetTransitionCallback(const std::function<void(ScreenType)>& callback) {
    transitionCallback = callback;
}

void Screen::SetupScreen(ScreenType type) {
    if (type != currentScreen) {
        screenHistory.push(currentScreen);
    }
    currentScreen = type;
}

void Screen::Render() {
    switch (currentScreen) {
    case ScreenType::WELCOME:
        RenderWelcomeScreen();
        break;
    case ScreenType::INITIALIZATION:
        RenderInitializationScreen();
        break;
    case ScreenType::VISUALIZATION:
        RenderVisualizationScreen();
        break;
    }
}

void Screen::HandleInput() {
    switch (currentScreen) {
    case ScreenType::WELCOME:
        HandleWelcomeInput();
        break;
    case ScreenType::INITIALIZATION:
        HandleInitializationInput();
        break;
    case ScreenType::VISUALIZATION:
        HandleVisualizationInput();
        break;
    }
}

void Screen::RenderWelcomeScreen() {
    ClearBackground(RAYWHITE);
    DrawText("Welcome to AVL tree visualization!", (GetScreenWidth() - MeasureText("Welcome to AVL tree Visualization!", 30)) / 2, 50, 30, DARKGRAY);
    RenderBackButton();
}

void Screen::RenderInitializationScreen() {
    ClearBackground(RAYWHITE);
    DrawText("Choose an option to visualize", (GetScreenWidth() - MeasureText("Choose an option to visualize", 30)) / 2, 50, 30, DARKGRAY);

    // Render Initialization buttons
    RenderButton(randomButton, "Randomized Data");
    RenderButton(manualButton, "Manual Input");
    RenderButton(importButton, "Import from File");

    // Check if "Manual Input" button is pressed to toggle input mode
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), manualButton)) {
        isManualInputActive = true; // Activate manual input mode
    }

    if (isManualInputActive) {
        // Define and draw the manual input box
        Rectangle manualInputBox = { (GetScreenWidth() - 250) / 2.0f, 350.0f, 250.0f, 50.0f };
        DrawRectangleRec(manualInputBox, LIGHTGRAY);

        // Check if the user clicks inside the box
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), manualInputBox)) {
                inputActive = true; // Activate text box
            } else {
                inputActive = false; // Deactivate text box if clicked outside
            }
        }

        // If the text box is selected, allow text input
        if (inputActive) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32 && key <= 126) && (strlen(inputText) < sizeof(inputText) - 1)) { // Add valid characters
                    int len = strlen(inputText);
                    inputText[len] = (char)key;
                    inputText[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && strlen(inputText) > 0) {
                inputText[strlen(inputText) - 1] = '\0';
            }
        }

        // Highlight the box if selected
        if (inputActive) {
            DrawRectangleLinesEx(manualInputBox, 3, BLUE); // Highlight with a blue border
        } else {
            DrawRectangleLinesEx(manualInputBox, 3, DARKGRAY); // Normal border
        }

        // Display the input text
        DrawText(inputText, manualInputBox.x + 10, manualInputBox.y + 15, 25, DARKGRAY);
    }

    RenderBackButton();
}


void Screen::RenderVisualizationScreen() {
    ClearBackground(RAYWHITE);
    DrawText("Visualization Screen", (GetScreenWidth() - MeasureText("Visualization Screen", 30)) / 2, 50, 30, DARKGRAY);

    // Render the input box
    DrawRectangleRec(inputBox, LIGHTGRAY);
    if (inputActive) {
        DrawRectangleLinesEx(inputBox, 3, BLUE); // Highlight when active
    } else {
        DrawRectangleLinesEx(inputBox, 3, DARKGRAY); // Normal border
    }

    // Display the entered text
    DrawText(inputText, inputBox.x + 5, inputBox.y + 10, 20, DARKGRAY);

    // Render buttons
    RenderButton(insertButton, "Insert");
    RenderButton(deleteButton, "Delete");
    RenderButton(searchButton, "Search");
    RenderButton(prevStepButton, "Prev");
    RenderButton(nextStepButton, "Next");
    RenderButton(pauseButton, "Pause");
    RenderButton(skipButton, "Skip");

    DrawText(printedNumbers.c_str(), printBox.x + 5, printBox.y + 10, 20, DARKGRAY);
    DrawText(("Current Mode: " + selectedOption).c_str(), 
         (GetScreenWidth() - MeasureText(("Current Mode: " + selectedOption).c_str(), 20)) / 2, 
         10, 20, DARKGRAY);

    RenderBackButton();
}



void Screen::HandleWelcomeInput() {
    if (HandleBackButtonInput()) return;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (transitionCallback) transitionCallback(ScreenType::INITIALIZATION);
    }
}

void Screen::HandleInitializationInput() {
    if (HandleBackButtonInput()) return;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        if (IsButtonClicked(randomButton)) {
            std::cout << "Randomized Data selected!" << std::endl;
            selectedOption = "Random Data";
            if (transitionCallback) transitionCallback(ScreenType::VISUALIZATION);
        } else if (IsButtonClicked(manualButton)) {
            std::cout << "Manual Input selected!" << std::endl;
            selectedOption = "Manual Input"; // Store selection
            if (transitionCallback) transitionCallback(ScreenType::VISUALIZATION);
        } else if (IsButtonClicked(importButton)) {
            std::cout << "Import from File selected!" << std::endl;
            selectedOption = "Import";
            if (transitionCallback) transitionCallback(ScreenType::VISUALIZATION);
        }
    }
}




void Screen::HandleVisualizationInput() {
    if (HandleBackButtonInput()) return;

    Vector2 mousePos = GetMousePosition();

    // Disable input interaction for Import and Randomized modes
    if (selectedOption == "Import" || selectedOption == "Randomized") {
        inputActive = false; // Ensure input box is not activated
        return;             // Skip input handling
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePos, inputBox)) {
            inputActive = true; // Activate input box if clicked
        } else {
            inputActive = false; // Deactivate if clicked outside
        }

        if (CheckCollisionPointRec(mousePos, insertButton)) {
            HandleInsert();
            ClearInputText(); // Clear input after handling insert
        } else if (CheckCollisionPointRec(mousePos, deleteButton)) {
            HandleDelete();
            ClearInputText(); // Clear input after handling delete
        } else if (CheckCollisionPointRec(mousePos, searchButton)) {
            HandleSearch();
            ClearInputText(); // Clear input after handling search
        }
    }

    if (inputActive) {
        int key = GetKeyPressed();
        if (key == KEY_BACKSPACE) {
            size_t len = strlen(inputText);
            if (len > 0) {
                inputText[len - 1] = '\0'; // Remove the last character
            }
        } else if (key == KEY_ENTER) {
            ClearInputText();  // Clear input on Enter
            inputActive = false; // Deactivate input
        } else if (key >= 32 && key <= 126) {
            size_t len = strlen(inputText);
            if (len < sizeof(inputText) - 1 && isdigit((char)key)) {
                inputText[len] = (char)key; // Add character to input text
                inputText[len + 1] = '\0';
            }
        }
    }
}
    
bool Screen::HandleBackButtonInput() {
    Rectangle backButton = { 10.0f, 10.0f, 100.0f, 40.0f };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
        if (!screenHistory.empty()) {
            currentScreen = screenHistory.top();
            screenHistory.pop();
            return true;
        }
    }
    return false;
}

void Screen::RenderButton(const Rectangle& button, const char* text) {
    DrawRectangleRec(button, DARKGRAY);
    DrawText(text, button.x + (button.width - MeasureText(text, 20)) / 2, button.y + 15, 20, RAYWHITE);
}

bool Screen::IsButtonClicked(const Rectangle& button) {
    return (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), button));
}

void Screen::RenderBackButton() {
    Rectangle backButton = { 10.0f, 10.0f, 100.0f, 40.0f };
    DrawRectangleRec(backButton, DARKGRAY);
    DrawText("Back", backButton.x + 25, backButton.y + 10, 20, RAYWHITE);
}
void Screen::HandleTextInput() {
    if (inputActive) {
        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= 48 && key <= 57) && (strlen(inputText) < 9)) { // Only allow numbers
                int len = strlen(inputText);
                inputText[len] = (char)key;
                inputText[len + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(inputText) > 0) {
            inputText[strlen(inputText) - 1] = '\0';
        }
    }
}
void Screen::ClearInputText() {
    memset(inputText, 0, sizeof(inputText)); // Clears the inputText buffer
}
void Screen::HandleInsert() {
    // TODO: Add logic to handle the insertion of a value
    printf("Insert button clicked. Value: %s\n", inputText);
}

void Screen::HandleDelete() {
    // TODO: Add logic to handle the deletion of a value
    printf("Delete button clicked. Value: %s\n", inputText);
}

void Screen::HandleSearch() {
    // TODO: Add logic to handle the searching of a value
    printf("Search button clicked. Value: %s\n", inputText);
}
