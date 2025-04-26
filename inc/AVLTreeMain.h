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
    SEARCH_NOT_FOUND,
    UPDATING
};

class AVLTreeVisualizer {
private:
    struct OperationStep {
        std::string operation;
        State currentState;
        std::vector<Node*> currentPath;
        int pathIndex;
        std::set<Node*> highlightNodes;
        std::string notificationMessage;
        int operationValue;
        int pendingInsertValue;
        bool searchFound;
        float stateTimer;
        float resultTimer;
        std::string currentOperation;
        int rotationIndex;
        AVLTree treeSnapshot;
    };

    float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue);
    AVLTree tree;
    AVLTree initialTreeState;
    std::string inputText; // For the first input box (used for all operations)
    std::string inputTextNew; // For the second input box (used only for "Update")
    bool inputActive; // For the first input box
    bool inputActiveNew; // For the second input box (used only for "Update")
    Rectangle handleSpace;
    Rectangle inputBox; // First input box (Old V. for Update, V: for others)
    Rectangle inputBoxNew; // Second input box (New V. for Update only)
    Rectangle initButtonRect;
    Rectangle insertButtonRect;
    Rectangle deleteButtonRect;
    Rectangle searchButtonRect;
    Rectangle updateButtonRect;
    Rectangle loadFileButtonRect;
    Rectangle rewindButtonRect;
    Rectangle previousButtonRect;
    Rectangle playPauseButtonRect;
    Rectangle nextButtonRect;
    Rectangle fastForwardButtonRect;
    Rectangle speedBar;
    Rectangle sliderHandle;
    Rectangle speedButtonRect; 
    bool speedButtonClicked;
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
    bool updateButtonClicked;
    bool loadFileButtonClicked;
    bool rewindButtonClicked;
    bool previousButtonClicked;
    bool playPauseButtonClicked;
    bool nextButtonClicked;
    bool fastForwardButtonClicked;
    bool randomButtonClicked;
    bool randomButtonClickedNew; // For the second "?" button (Update only)
    std::string initButtonMessage;
    std::string insertButtonMessage;
    std::string deleteButtonMessage;
    std::string searchButtonMessage;
    std::string updateButtonMessage;
    std::string loadFileButtonMessage;
    std::string rewindButtonMessage;
    std::string previousButtonMessage;
    std::string playPauseButtonMessage;
    std::string nextButtonMessage;
    std::string fastForwardButtonMessage;
    Rectangle randomButtonRect; // First "?" button (for first input box)
    Rectangle randomButtonRectNew; // Second "?" button (for second input box, Update only)
    std::string randomButtonMessage;
    bool manualStepping;
    std::vector<State> stateHistory;
    int stateHistoryIndex;
    Texture2D backButtonTexture;
    bool backButtonClicked;
    bool showInputWindow;
    Rectangle okButtonRect;
    bool okButtonClicked;
    std::string okButtonMessage;
    std::string operationType;
    bool isInitialized;
    
    std::vector<OperationStep> operationSteps;
    int currentStepIndex;

public:
    bool shouldClose;
    AVLTreeVisualizer();
    ~AVLTreeVisualizer();

    void handleInput();
    void updateAnimation(float deltaTime);
    void draw();

    void animateInsert(int value);
    void animateDelete(int value);
    void animateSearch(int value);
    void animateUpdate(int oldValue, int newValue);
    void animateRandom();
    void animateClear();
    void animateLoadFile();
    void animatePrevious();
    void animateNext();
    void stepBackward();
    void stepForward();

    void setNotificationMessage(const std::string& message);
    std::string getNotificationMessage() const;
    std::string getPseudocode();

    bool IsBackButtonClicked() const { return backButtonClicked; }
    bool ShouldClose() const { return shouldClose; }

private:
    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight);
};

extern std::stack<AVLTree> treeUndoState;
extern std::stack<AVLTree> treeRedoState;

#endif // AVLMAIN_H
