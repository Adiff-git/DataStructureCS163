#ifndef ANIMATION_H
#define ANIMATION_H

#include "AVL.h"
#include "raylib.h"
#include "Button.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <locale>
#include <codecvt>

class AVLTreeVisualizer {
public:
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
    Button playPauseButton;
    Button previousButton;
    Button nextButton;
    Button fastForwardButton;

    enum AnimationState { IDLE, TRAVERSING, INSERTING, ROTATING, SHOWING_RESULT, DELETING, HIGHLIGHTING_BEFORE_DELETE, SEARCHING, SEARCH_NOT_FOUND };
    AnimationState currentState;

    std::string currentOperation;
    std::vector<Node*> currentPath;
    int pathIndex;
    std::vector<Node*> rotationNodes;
    int rotationIndex;
    bool searchFound;
    float stateTimer;
    float resultTimer;
    int operationValue;
    int pendingInsertValue;
    std::string notificationMessage;
    std::set<Node*> highlightNodes;

    float animationSpeed;
    Rectangle speedBar;
    Rectangle sliderHandle;
    bool draggingSlider;

    bool paused;

    Font ralewayFont;

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
    std::string getPseudocode();
    void setNotificationMessage(const std::string& message);
    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight);
};

#endif // ANIMATION_H