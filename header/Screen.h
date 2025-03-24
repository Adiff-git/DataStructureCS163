#ifndef SCREEN_H
#define SCREEN_H

#include "raylib.h"
#include <functional>
#include <stack>
#include <string>

using namespace std; 

// Enum for screen types
enum class ScreenType {
    WELCOME,
    INITIALIZATION,
    VISUALIZATION
    
};

// Screen class
class Screen {
private:
    ScreenType currentScreen;                       
    std::stack<ScreenType> screenHistory;           
    std::function<void(ScreenType)> transitionCallback; 

    char inputText[10]; // For user input
    std::string printedNumbers;
    bool inputActive;

    // Buttons and UI elements
    Rectangle insertButton, deleteButton, searchButton;
    Rectangle prevStepButton, nextStepButton, pauseButton, skipButton;
    Rectangle inputBox, printBox;

    // Buttons for Initialization Screen
    Rectangle randomButton, manualButton, importButton;

    // Input box for Manual Input
    Rectangle manualInputBox; 
    bool isManualInputActive = false; // Track if Manual Input is selected

    // Stores selected option
    std::string selectedOption;

    // Rendering functions
    void RenderWelcomeScreen();
    void RenderInitializationScreen();
    void RenderVisualizationScreen();

    // Input handling functions
    void HandleWelcomeInput();
    void HandleInitializationInput();
    void HandleVisualizationInput();

    // Button utilities
    void RenderButton(const Rectangle& button, const char* text);
    bool IsButtonClicked(const Rectangle& button);

    // Back button
    void RenderBackButton();
    bool HandleBackButtonInput();

public:
    Screen();                                       
    ~Screen();                                      

    void SetupScreen(ScreenType type);             
    void Render();                                 
    void HandleInput();                            
    void HandleTextInput();  
    void SetTransitionCallback(const std::function<void(ScreenType)>& callback);
    void HandleInsert();       // Handles insert button logic
    void HandleDelete();       // Handles delete button logic
    void HandleSearch();       // Handles search button logic
    void ClearInputText();     // Clears the input text buffer
};

#endif // SCREEN_H
