#ifndef AVLMAIN_H
#define AVLMAIN_H

#include "AVLTree.h"
#include "button.h"
#include "raylib.h"
#include <string>
#include <stack>
#include <set>
#include <vector>

enum State {
    IDLE,
    TRAVERSING,
    INSERTING,
    HIGHLIGHTING_BEFORE_DELETE,
    DELETING,
    ROTATING,
    SHOWING_RESULT,
    SEARCHING,
    SEARCH_NOT_FOUND
};

class AVLTreeVisualizer {
private:
    struct OperationStep {
        std::string operation; // Operation type
        State currentState; // Current animation state
        std::vector<Node*> currentPath; // Path for operation
        int pathIndex; // Current path index
        std::set<Node*> highlightNodes; // Nodes to highlight
        std::string notificationMessage; // Displayed notification
        int operationValue; // Value being operated
        int pendingInsertValue; // Value to insert
        bool searchFound; // Search result flag
        float stateTimer; // Animation state timer
        float resultTimer; // Result display timer
        std::string currentOperation; // Current operation name
        int rotationIndex; // Rotation step index
        AVLTree treeSnapshot; // Tree state snapshot
    };

    float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue); // Updates slider value
    AVLTree tree; // Main AVL tree
    AVLTree initialTreeState; // Initial tree state
    std::string inputText; // User input text
    bool inputActive; // Input box active
    Rectangle handleSpace; // Slider handle area
    Rectangle inputBox; // Input box rectangle
    Rectangle initButtonRect; // Initialize button rectangle
    Rectangle insertButtonRect; // Insert button rectangle
    Rectangle deleteButtonRect; // Delete button rectangle
    Rectangle searchButtonRect; // Search button rectangle
    Rectangle loadFileButtonRect; // Load file button
    Rectangle rewindButtonRect; // Rewind animation button
    Rectangle previousButtonRect; // Previous step button
    Rectangle playPauseButtonRect; // Play/pause animation button
    Rectangle nextButtonRect; // Next step button
    Rectangle fastForwardButtonRect; // Fast-forward animation button
    Rectangle speedBar; // Animation speed bar
    Rectangle sliderHandle; // Slider handle rectangle
    State currentState; // Current visualizer state
    std::vector<Node*> currentPath; // Current operation path
    int pathIndex; // Path traversal index
    int rotationIndex; // Rotation animation index
    bool searchFound; // Search result status
    float stateTimer; // State transition timer
    float resultTimer; // Result display duration
    int operationValue; // Current operation value
    int pendingInsertValue; // Pending insert value
    float animationSpeed; // Animation speed factor
    bool draggingSlider; // Slider being dragged
    bool paused; // Animation paused flag
    std::string notificationMessage; // Current notification text
    std::set<Node*> highlightNodes; // Highlighted nodes set
    std::string currentOperation; // Active operation type
    bool initButtonClicked; // Init button clicked
    bool insertButtonClicked; // Insert button clicked
    bool deleteButtonClicked; // Delete button clicked
    bool searchButtonClicked; // Search button clicked
    bool loadFileButtonClicked; // Load file clicked
    bool rewindButtonClicked; // Rewind button clicked
    bool previousButtonClicked; // Previous button clicked
    bool playPauseButtonClicked; // Play/pause button clicked
    bool nextButtonClicked; // Next button clicked
    bool fastForwardButtonClicked; // Fast-forward button clicked
    bool randomButtonClicked; // Random operation clicked
    std::string initButtonMessage; // Init button text
    std::string insertButtonMessage; // Insert button text
    std::string deleteButtonMessage; // Delete button text
    std::string searchButtonMessage; // Search button text
    std::string loadFileButtonMessage; // Load file text
    std::string rewindButtonMessage; // Rewind button text
    std::string previousButtonMessage; // Previous button text
    std::string playPauseButtonMessage; // Play/pause button text
    std::string nextButtonMessage; // Next button text
    std::string fastForwardButtonMessage; // Fast-forward button text
    Rectangle randomButtonRect; // Random button rectangle
    std::string randomButtonMessage; // Random button text
    bool manualStepping; // Manual animation stepping
    std::vector<State> stateHistory; // State history log
    int stateHistoryIndex; // Current history index
    Texture2D backButtonTexture; // Back button texture
    bool backButtonClicked; // Back button clicked
    bool showInputWindow; // Input window visible
    Rectangle okButtonRect; // OK button rectangle
    bool okButtonClicked; // OK button clicked
    std::string okButtonMessage; // OK button text
    std::string operationType; // Current operation type
    bool isInitialized; // Visualizer initialized flag
    bool shouldClose; // Window close flag
    std::vector<OperationStep> operationSteps; // Operation steps log
    int currentStepIndex; // Current step index

public:
    AVLTreeVisualizer(); // Initializes visualizer
    ~AVLTreeVisualizer(); // Cleans up resources

    void handleInput(); // Processes user input
    void updateAnimation(float deltaTime); // Updates animation state
    void draw(); // Renders tree and UI

    void animateInsert(int value); // Animates node insertion
    void animateDelete(int value); // Animates node deletion
    void animateSearch(int value); // Animates value search
    void animateRandom(); // Animates random operation
    void animateClear(); // Animates tree clear
    void animateLoadFile(); // Animates file loading
    void animatePrevious(); // Animates previous step
    void animateNext(); // Animates next step
    void stepBackward(); // Steps animation backward
    void stepForward(); // Steps animation forward

    void setNotificationMessage(const std::string& message); // Sets notification text
    std::string getNotificationMessage() const; // Gets notification text
    std::string getPseudocode(); // Returns pseudocode string

    bool IsBackButtonClicked() const { return backButtonClicked; } // Checks back button
    bool ShouldClose() const { return shouldClose; } // Checks close flag

private:
    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight); // Draws tree recursively
};

extern std::stack<AVLTree> treeUndoState; // Undo state stack
extern std::stack<AVLTree> treeRedoState; // Redo state stack

#endif // AVLMAIN_H