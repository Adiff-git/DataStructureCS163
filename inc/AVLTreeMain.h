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

    std::vector<std::vector<Node*>> insertPaths;
    std::vector<std::set<Node*>> insertHighlightNodes;
    std::vector<std::string> insertNotifications;
    std::vector<State> insertStates;

    std::vector<std::vector<Node*>> deletePaths;
    std::vector<std::set<Node*>> deleteHighlightNodes;
    std::vector<std::string> deleteNotifications;
    std::vector<State> deleteStates;

    std::vector<std::vector<Node*>> searchPaths;
    std::vector<std::set<Node*>> searchHighlightNodes;
    std::vector<std::string> searchNotifications;
    std::vector<State> searchStates;

    // Biến để theo dõi bước hiện tại của animation
    int totalSteps;
    int currentStep;

    AVLTree tree;
    std::string inputText;
    bool inputActive;
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
    Rectangle randomButtonRect;
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

    // Animation control
    int curStep;
    void resetAnimation();

    // Menu con và trạng thái nút
    bool showInitOption;
    bool showInsertOption;
    bool showDeleteOption;
    bool showSearchOption;
    bool showLoadOption;
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
    std::string randomButtonMessage;

    // Back button
    Texture2D backButtonTexture;
    bool backButtonClicked;

    // Code area
    float codePosX = 10;
    float codePosY = 60;
    float codePosSpace = 25;

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
    void animateRandom(int n);  // Sửa để nhận tham số
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

private:
    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight);
    void drawOperationMenu();
    void drawInitOptions();
    void drawInsertOptions();
    void drawDeleteOptions();
    void drawSearchOptions();
    void drawLoadOptions();
    void drawAnimationMenu();
    float UpdateSlider(Rectangle slider, float minValue, float maxValue, float currentValue);
};

extern std::stack<AVLTree> treeUndoState;
extern std::stack<AVLTree> treeRedoState;

#endif // AVLMAIN_H