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
    Button initButton;
    Button insertButton;
    Button deleteButton;
    Button searchButton;
    Button loadFileButton;
    Button rewindButton;
    Button previousButton;
    Button playPauseButton;
    Button nextButton;
    Button fastForwardButton;
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

    void setNotificationMessage(const std::string& message);
    std::string getNotificationMessage() const;
    std::string getPseudocode();

private:
    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight);
};

extern std::stack<AVLTree> treeUndoState;
extern std::stack<AVLTree> treeRedoState;

#endif // ANIMATION_H