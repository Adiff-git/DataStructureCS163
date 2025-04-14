#ifndef ANIMATION_H
#define ANIMATION_H

#include "AVL.h"
#include "Button.h"
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
    AVLTree tree;
    std::string inputText;
    bool inputActive;
    Rectangle handleSpace;
    Rectangle inputBox;
    Rectangle initButtonRect;
    Rectangle insertButtonRect;
    Rectangle deleteButtonRect;
    Rectangle searchButtonRect;
    Rectangle loadFileButtonRect;
    Rectangle rewindButtonRect;
    Rectangle previousButtonRect;
    Rectangle playPauseButtonRect;
    Rectangle nextButtonRect;
    Rectangle fastForwardButtonRect;
    Rectangle speedBar;
    Rectangle sliderHandle;
    State currentState;
    std::vector<Node*> currentPath;
    int pathIndex;
    int rotationIndex;
    bool searchFound;
    float stateTimer;
    float resultTimer;
    int operationValue;
    int pendingInsertValue;
    float animationSpeed;
    bool draggingSlider;
    bool paused;
    std::string notificationMessage;
    std::set<Node*> highlightNodes;
    std::string currentOperation;
    bool initButtonClicked;
    bool insertButtonClicked;
    bool deleteButtonClicked;
    bool searchButtonClicked;
    bool loadFileButtonClicked;
    bool rewindButtonClicked;
    bool previousButtonClicked;
    bool playPauseButtonClicked;
    bool nextButtonClicked;
    bool fastForwardButtonClicked;
    bool randomButtonClicked;
    std::string initButtonMessage;
    std::string insertButtonMessage;
    std::string deleteButtonMessage;
    std::string searchButtonMessage;
    std::string loadFileButtonMessage;
    std::string rewindButtonMessage;
    std::string previousButtonMessage;
    std::string playPauseButtonMessage;
    std::string nextButtonMessage;
    std::string fastForwardButtonMessage;
    Rectangle randomButtonRect;
    std::string randomButtonMessage;
    // New variables for stepping through animation states
    bool manualStepping; // Tracks if we're stepping manually (via Previous/Next)
    std::vector<State> stateHistory; // Tracks the sequence of states for the current operation
    int stateHistoryIndex; // Current position in the state history

public:
    AVLTreeVisualizer();
    ~AVLTreeVisualizer();

    void handleInput();
    void updateAnimation(float deltaTime);
    void draw();

    void animateInsert(int value);
    void animateDelete(int value);
    void animateSearch(int value);
    void animateRandom();
    void animateClear();
    void animateLoadFile();
    void animatePrevious();
    void animateNext();
    // New method for stepping backward in animation
    void stepBackward();
    // New method for stepping forward in animation
    void stepForward();

    void setNotificationMessage(const std::string& message);
    std::string getNotificationMessage() const;
    std::string getPseudocode();

private:
    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight);
};

extern std::stack<AVLTree> treeUndoState;
extern std::stack<AVLTree> treeRedoState;

#endif // ANIMATION_H