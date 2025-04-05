#include "AVL.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <locale>
#include <codecvt>

class AVLTreeVisualizer {
public:
    AVLTree tree;  // Your AVL Tree structure
    std::string inputText;  // Text in the input box
    bool inputActive;  // Whether the input box is active
    Rectangle handleSpace;  // Rectangle for the handle space
    Rectangle inputBox;  // Rectangle for the input box
    Rectangle insertButton;  // Button for insert operation
    Rectangle deleteButton;  // Button for delete operation
    Rectangle searchButton;  // Button for search operation
    Rectangle randomButton;  // Button for random operation
    Rectangle clearButton;  // Button for clearing the tree
    Rectangle loadFileButton;  // Button for loading a file
    Rectangle previousButton;  // Button for previous operation
    Rectangle nextButton;  // Button for next operation

    enum AnimationState { IDLE, TRAVERSING, INSERTING, ROTATING, SHOWING_RESULT, DELETING, HIGHLIGHTING_BEFORE_DELETE, SEARCHING };
    AnimationState currentState;  // Current animation state

    std::string currentOperation;  // Current operation (insert, delete, search)
    std::vector<Node*> currentPath;  // Path during tree traversal
    int pathIndex;  // Index for traversing the currentPath
    std::vector<Node*> rotationNodes;  // Nodes involved in rotations
    int rotationIndex;  // Index for rotation nodes
    bool searchFound;  // Whether search value was found
    float stateTimer;  // Timer for state duration
    float resultTimer;  // Timer for result display duration
    int operationValue;  // The value of the operation (insert, delete, search)
    int pendingInsertValue;  // The value to insert
    std::string notificationMessage;  // Current notification message
    std::set<Node*> highlightNodes;

    // Speed bar members
    float animationSpeed;  // Animation speed (0.0 slow to 1.0 fast)
    Rectangle speedBar;  // Rectangle for the speed bar
    Rectangle sliderHandle;  // Rectangle for the slider handle
    bool draggingSlider;  // Track if the slider is being dragged

    // Animation control buttons
    Rectangle playButton;     // Button to play/resume animation
    Rectangle pauseButton;    // Button to pause animation
    Rectangle forwardButton;  // Button to step forward in animation
    Rectangle backwardButton; // Button to step backward in animation
    bool paused;              // Tracks if animation is paused

    // Add the new method to get pseudocode
    std::string getPseudocode();  // New method declaration for pseudocode

    // Helper method to set notification message
    void setNotificationMessage(const std::string& message);  // Method to set the notification

    // Draw the tree at a specific position
    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight);

    // Constructor to initialize the visualizer
    AVLTreeVisualizer();

    // Handle user input
    void handleInput();

    // Update the animation states
    void updateAnimation(float deltaTime);

    // Draw the UI and tree
    void draw();

    // Animation methods for specific actions
    void animateInsert(int value);  // Insert animation
    void animateDelete(int value);  // Delete animation
    void animateSearch(int value);  // Search animation
    void animateRandom();  // Random tree modification animation
    void animateClear();  // Clear tree animation
    void animateLoadFile();  // Load file animation
    void animatePrevious();  // Previous operation animation
    void animateNext();  // Next operation animation
};