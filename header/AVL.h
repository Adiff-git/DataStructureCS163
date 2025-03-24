#ifndef SCREEN_H
#define SCREEN_H

#include "raylib.h"
#include <functional>
#include <stack>

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

    // Individual screen rendering methods
    void RenderWelcomeScreen();
    void RenderInitializationScreen();
    void RenderVisualizationScreen();

    // Individual screen input handling methods
    void HandleWelcomeInput();
    void HandleInitializationInput();
    void HandleVisualizationInput();

    // Button utilities
    void RenderButton(Rectangle button, const char* text);
    bool IsButtonClicked(Rectangle button);

    // Back button utilities
    void RenderBackButton();
    bool HandleBackButtonInput();
    
public:
    Screen();                                       
    ~Screen();                                      

    void SetupScreen(ScreenType type);             
    void Render();                                  
    void HandleInput();                             
    void SetTransitionCallback(const std::function<void(ScreenType)>& callback);
};

#endif // SCREEN_H
