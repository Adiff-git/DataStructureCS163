#include "LinkedList.h"
#include "Button.h"
#include "Utils.h"
#include <sstream>
#include <fstream> // Thêm header này
#include <string>

LinkedList::LinkedList(int width, int height) 
    : head(nullptr), isPaused(true), skipStep(false), speed(1.0f), 
      activeFunction(0), activeLine(0), scrollOffset(0.0f), notification(""), 
      screenWidth(width), screenHeight(height), foundNode(nullptr), currentStepIndex(-1) {
    operationStates.clear();
    operationHistory = std::deque<ListState>();
}
void LinkedList::Clear() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    operationStates.clear();
    currentStepIndex = -1;
}

std::string LinkedList::GetNotification() const {
    return notification;
}

void LinkedList::SetNotification(const std::string& msg) {
    notification = msg;
}

void LinkedList::ClearList() {
    Clear();
}

void LinkedList::EnqueueOperation(int type, int value1, int value2) {
    operationQueue.push(Operation(type, value1, value2));
}

void LinkedList::EnqueueOperationWithValues(int type, const std::vector<int>& values) {
    operationQueue.push(Operation(type, 0, 0));
    opState.initValues = values;
    opState.initCount = values.size();
}

void LinkedList::ProcessOperations() {
    if (activeFunction != 0 || operationQueue.empty()) return;
    
    foundNode = nullptr;
    Operation op = operationQueue.front();
    operationQueue.pop();
    activeFunction = op.type;
    activeLine = 1;
    opState.step = 1;
    opState.traversalIndex = 0;

    // Save the state before the operation starts
    ListState preState;
    Node* current = head;
    while (current) {
        preState.values.push_back(current->value);
        preState.positions.push_back(current->position);
        preState.isActive.push_back(current->isActive);
        preState.alphas.push_back(current->alpha);
        preState.targetPositions.push_back(current->targetPosition);
        preState.animationProgresses.push_back(current->animationProgress);
        preState.animationTypes.push_back(current->animationType);
        current = current->next;
    }
    preState.activeFunction = activeFunction;
    preState.activeLine = activeLine;
    preState.opState = opState;
    operationHistory.push_back(preState);

    // Clear operationStates for the new operation
    operationStates.clear();
    currentStepIndex = -1;

    // Save the initial state of the operation
    SaveState();

    switch (op.type) {
        case 1: // Initialize
            opState.initCount = op.value1; 
            opState.initIndex = 0; 
            Clear(); 
            opState.current = nullptr;
            if (!opState.initValues.empty()) {
                opState.initCount = opState.initValues.size();
            }
            break;
        case 2: // Add Head
            opState.newNode = new Node(op.value1); 
            break;
        case 3: // Add Index
            opState.newNode = new Node(op.value2);
            opState.addIndex = op.value1;
            opState.current = head;
            break;
        case 4: // Add Tail
            opState.newNode = new Node(op.value1); 
            break;
        case 5: // Delete
            opState.deleteValue = op.value1;
            opState.current = head; 
            break;
        case 6: // Search
            opState.searchValue = op.value1;
            opState.current = head; 
            opState.searchResult = false; 
            break;
        case 7: // Update
            opState.updateOld = op.value1; 
            opState.updateNew = op.value2; 
            opState.current = head; 
            break;
    }
}

void LinkedList::UpdateOperations() {
    if (activeFunction == 0 || isPaused) return;

    bool isAnimating = false;
    Node* current = head;
    while (current) {
        if (current->animationType != 0) {
            isAnimating = true;
            break;
        }
        current = current->next;
    }

    if (isAnimating || opState.delayTimer > 0) return;

    SaveState();

    switch (activeFunction) {
        case 1: UpdateInitialize(); break;
        case 2:
        case 3:
        case 4:
            UpdateAdd(); break;
        case 5: UpdateDelete(); break;
        case 6: UpdateSearch(); break;
        case 7: UpdateUpdate(); break;
    }
}

void LinkedList::UpdateAnimation() {
    if (isPaused) return;

    if (opState.delayTimer > 0) {
        opState.delayTimer -= GetFrameTime();
        if (opState.delayTimer < 0) opState.delayTimer = 0;
    }

    Node* current = head;
    bool hasAnimation = false;
    while (current) {
        if (current->animationType != 0) {
            hasAnimation = true;
            current->animationProgress += speed / 60.0f;
            if (current->animationProgress >= 1.0f || skipStep) {
                current->animationProgress = 1.0f;
                switch (current->animationType) {
                    case 1: current->isActive = false; break;
                    case 2: 
                        current->position = current->targetPosition; 
                        current->alpha = 1.0f; 
                        break;
                    case 3: current->alpha = 0.0f; break;
                    case 4: current->position = current->targetPosition; break;
                    case 5: break;
                }
                current->animationType = 0;
            } else {
                float t = EaseInOutQuad(current->animationProgress);
                switch (current->animationType) {
                    case 1:
                        if (current->animationProgress < 0.5f) {
                            current->isActive = true;
                        } else {
                            current->isActive = false;
                        }
                        break;
                    case 2:
                        current->position.x = Lerp(current->position.x, current->targetPosition.x, t); // Thêm chuyển động x
                        current->position.y = Lerp(current->position.y, current->targetPosition.y, t);
                        current->alpha = t; // Tăng dần độ rõ
                        break;
                    case 3:
                        current->alpha = 1.0f - t;
                        break;
                    case 4:
                        current->position.x = Lerp(current->position.x, current->targetPosition.x, t);
                        break;
                    case 5: break;
                }
            }
        }
        current = current->next;
    }

    if (skipStep) {
        SkipToEnd();
        skipStep = false;
    }
}

void LinkedList::Draw() {
    Node* current = head;
    while (current) {
        Vector2 adjustedPos = {current->position.x + scrollOffset, current->position.y};
        DrawNode(current, adjustedPos);
        if (current->next) {
            Vector2 nextPos = {current->next->position.x + scrollOffset, current->next->position.y};
            if (current->animationType == 5) {
                float t = current->animationProgress;
                Vector2 midPoint = {adjustedPos.x + (nextPos.x - adjustedPos.x) * t, adjustedPos.y};
                DrawLineEx(adjustedPos, midPoint, 2, BLACK);
                if (t >= 1.0f) {
                    Vector2 dir = Vector2NormalizeCustom(Vector2SubtractCustom(nextPos, adjustedPos));
                    Vector2 arrow1 = {nextPos.x - NODE_SIZE / 2 - 10 * dir.x + 5 * dir.y, nextPos.y - 10 * dir.y - 5 * dir.x};
                    Vector2 arrow2 = {nextPos.x - NODE_SIZE / 2 - 10 * dir.x - 5 * dir.y, nextPos.y - 10 * dir.y + 5 * dir.x};
                    DrawLineEx({nextPos.x - NODE_SIZE / 2, nextPos.y}, arrow1, 2, BLACK);
                    DrawLineEx({nextPos.x - NODE_SIZE / 2, nextPos.y}, arrow2, 2, BLACK);
                }
            } else {
                DrawArrow(adjustedPos, nextPos);
            }
        }
        current = current->next;
    }
}

void LinkedList::DrawCodeBlock() {
    if (activeFunction == 0) return;

    const char* codeLines[7][7] = {
        // Initialize
        {"Initialize:", "1. Clear List", "2. Add Random Node", "", "", "", ""},
        // Add Head
        {"Add Head(value):", "1. Node* newNode = new Node(value)", "2. newNode->next = head", "3. head = newNode", "", "", ""},
        // Add Index
        {"Add Index(value):", "1. Node* newNode = new Node(value)", "2. while (!cur && cur->val != value)", "3.     cur = cur->next", "4. prev = prev->next", "5. if (cur) newNode->next = cur", "6. prev->next = newNode"},
        // Add Tail
        {"Add Tail(value):", "1. if (!head) head = new Node(value)", "2. while (cur->next)", "3.     cur = cur->next", "4. cur->next = new Node(value)", "", ""},
        // Delete
        {"Delete(value):", "1. while (cur->next &&", "2.     cur->next->val != value)", "3.     cur = cur->next", "4. if (cur->next)", "5.     Node* tmp = cur->next", "6.     cur->next = cur->next->next; delete tmp"},
        // Search
        {"Search(value):", "1. while (cur && cur->val != value)", "2.     cur = cur->next", "3. if (cur) return true", "4. return false", "", ""},
        // Update
        {"Update(old, new):", "1. while (cur && cur->val != old)", "2.     cur = cur->next", "3. if (cur) cur->val = new", "", "", ""}
    };

    int numLines = 0;
    for (int i = 0; i < 7; i++) {
        if (codeLines[activeFunction - 1][i][0] != '\0') numLines++;
    }

    float y = screenHeight - 200;
    float x = screenWidth - 450; // Điều chỉnh vị trí x để khung không bị che khuất
    float lineHeight = 30;
    float padding = 10;
    float frameWidth = 450; // Tăng chiều rộng khung để chứa dòng dài nhất
    float frameHeight = numLines * lineHeight + 2 * padding;
    DrawRectangle(x - padding, y - padding, frameWidth, frameHeight, SKYBLUE);

    for (int i = 0; i < 7; i++) {
        if (codeLines[activeFunction - 1][i][0] != '\0') {
            Color textColor = (i == activeLine) ? YELLOW : BLACK;
            DrawText(codeLines[activeFunction - 1][i], x, y, 20, textColor);
            y += lineHeight;
        }
    }
}

void LinkedList::LoadFromFile(const char* filename) {
    Clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        SetNotification("Failed to open file: " + std::string(filename));
        return;
    }

    std::string line;
    std::vector<int> values;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int value;
        while (ss >> value) values.push_back(value);
    }
    file.close();

    if (!values.empty()) {
        EnqueueOperationWithValues(1, values);
    } else {
        Clear();
        SetNotification("File is empty or contains no valid numbers.");
    }
}

void LinkedList::NextStep() {
    // Kiểm tra nếu đã ở bước cuối cùng
    if (currentStepIndex >= static_cast<int>(operationStates.size()) - 1) {
        SetNotification("No next steps available.");
        return;
    }

    // Tăng chỉ số bước và tải trạng thái tiếp theo
    currentStepIndex++;
    LoadState(operationStates[currentStepIndex]);
    SetNotification("Advanced to step " + std::to_string(currentStepIndex + 1) + ".");
}

void LinkedList::PreviousStep() {
    // Kiểm tra nếu đã ở bước đầu tiên
    if (currentStepIndex <= 0) {
        SetNotification("No previous steps available.");
        return;
    }

    // Giảm chỉ số bước và tải trạng thái trước đó
    currentStepIndex--;
    LoadState(operationStates[currentStepIndex]);
    SetNotification("Reverted to step " + std::to_string(currentStepIndex + 1) + ".");
}

void LinkedList::SkipToEnd() {
    if (activeFunction == 0) {
        SetNotification("No operation in progress.");
        return;
    }

    // Complete all animations instantly
    Node* current = head;
    while (current) {
        if (current->animationType != 0) {
            current->animationProgress = 1.0f;
            switch (current->animationType) {
                case 1: current->isActive = false; break;
                case 2: 
                    current->position = current->targetPosition; 
                    current->alpha = 1.0f; 
                    break;
                case 3: current->alpha = 0.0f; break;
                case 4: current->position = current->targetPosition; break;
                case 5: break;
            }
            current->animationType = 0;
        }
        current = current->next;
    }

    // Execute the operation to completion
    switch (activeFunction) {
        case 1: SkipInitialize(); break;
        case 2: SkipAdd(); break;
        case 3: SkipDelete(); break;
        case 4: SkipSearch(); break;
        case 5: SkipUpdate(); break;
    }

    // Save the final state
    SaveState();
    currentStepIndex = operationStates.size() - 1;
    SetNotification("Operation completed.");
}

void LinkedList::SetPaused(bool paused) { isPaused = paused; }
bool LinkedList::IsPaused() const { return isPaused; }
void LinkedList::SkipCurrentStep() { skipStep = true; }
void LinkedList::SetSpeed(float newSpeed) { speed = newSpeed; }
float LinkedList::GetSpeed() const { return speed; }

void LinkedList::UpdateScroll(float screenWidth, float screenHeight) {
    Node* current = head;
    float maxX = 0;
    while (current) {
        maxX = (current->position.x > maxX) ? current->position.x : maxX;
        current = current->next;
    }
    float maxScroll = maxX - screenWidth + 200;
    if (maxScroll < 0) maxScroll = 0;
    scrollOffset = -UpdateSlider({50, screenHeight - 100, 200, 20}, 0, maxScroll, -scrollOffset);
}

float LinkedList::GetScrollOffset() const { return scrollOffset; }

void LinkedList::UpdateInitialize() {
    if (opState.step == 1) {
        activeLine = 1; // Highlight "Clear List"
        Clear();
        opState.delayTimer = 0.5f;
        opState.step = 2;
        SaveState();
    } else if (opState.step >= 2) { // Steps 2+: Add nodes
        activeLine = 2; // Highlight "Add Random Node"
        if (opState.initIndex < opState.initCount) {
            Node* newNode;
            if (opState.initValues.empty()) {
                newNode = new Node(GetRandomValue(1, 99));
            } else {
                newNode = new Node(opState.initValues[opState.initIndex]);
            }
            if (!head) {
                newNode->position = {200, 100};
                newNode->targetPosition = {200, 300};
                newNode->alpha = 0.0f;
                newNode->animationType = 2;
                newNode->animationProgress = 0.0f;
                head = newNode;
                opState.current = head;
            } else {
                Node* current = head;
                while (current->next) current = current->next;
                newNode->position = {current->position.x + SPACING, 100};
                newNode->targetPosition = {current->position.x + SPACING, 300};
                newNode->alpha = 0.0f;
                newNode->animationType = 2;
                newNode->animationProgress = 0.0f;
                current->next = newNode;
                current->animationType = 5;
                current->animationProgress = 0.0f;
                opState.current = newNode;
            }
            opState.initIndex++;
            opState.delayTimer = 0.5f;
            opState.step++;
            SaveState();
        } else {
            notification = "Initialized " + std::to_string(opState.initCount) + " nodes.";
            ResetOperation();
            SaveState();
        }
    }
}

void LinkedList::SkipInitialize() {
    if (activeFunction != 1) return;

    while (opState.initIndex < opState.initCount) {
        Node* newNode;
        if (opState.initValues.empty()) {
            newNode = new Node(GetRandomValue(1, 99));
        } else {
            newNode = new Node(opState.initValues[opState.initIndex]);
        }
        if (!head) {
            newNode->position = {200, 300};
            newNode->targetPosition = {200, 300};
            newNode->alpha = 1.0f;
            head = newNode;
        } else {
            Node* current = head;
            while (current->next) current = current->next;
            newNode->position = {current->position.x + SPACING, 300};
            newNode->targetPosition = {current->position.x + SPACING, 300};
            newNode->alpha = 1.0f;
            current->next = newNode;
        }
        opState.initIndex++;
    }
    notification = "Initialized " + std::to_string(opState.initCount) + " nodes.";
    ResetOperation();
}

void LinkedList::UpdateAdd() {
    if (activeFunction == 2) { // Add Head
        if (opState.step == 1) {
            activeLine = 1; // Highlight "Node* newNode = new Node(value)"
            opState.newNode->position = {200, 100};
            opState.newNode->alpha = 0.0f;
            opState.newNode->animationType = 2;
            opState.newNode->animationProgress = 0.0f;
            opState.newNode->targetPosition = {200, 300};
            opState.delayTimer = 0.5f;
            opState.step = 2;
            SaveState();
        } else if (opState.step == 2) {
            activeLine = 2; // Highlight "newNode->next = head"
            if (head) {
                opState.newNode->next = head;
                head->animationType = 5;
                head->animationProgress = 0.0f;
            }
            opState.delayTimer = 0.5f;
            opState.step = 3;
            SaveState();
        } else if (opState.step == 3) {
            activeLine = 3; // Highlight "head = newNode"
            head = opState.newNode;
            RealignNodes();
            notification = "Added node with value " + std::to_string(opState.newNode->value) + " at head.";
            opState.newNode = nullptr;
            ResetOperation();
            SaveState();
        }
    } else if (activeFunction == 3) { // Add Index
        if (opState.step == 1) {
            activeLine = 1; // Highlight "Node* newNode = new Node(value)"
            if (!head) {
                if (opState.addIndex == 0) {
                    opState.newNode->position = {200, 100};
                    opState.newNode->alpha = 0.0f;
                    opState.newNode->targetPosition = {200, 300};
                    opState.newNode->animationType = 2;
                    opState.newNode->animationProgress = 0.0f;
                    head = opState.newNode;
                    notification = "Added node with value " + std::to_string(opState.newNode->value) + " at index 0.";
                    opState.newNode = nullptr;
                    ResetOperation();
                    SaveState();
                } else {
                    notification = "List is empty. Cannot add at index " + std::to_string(opState.addIndex) + ".";
                    delete opState.newNode;
                    opState.newNode = nullptr;
                    ResetOperation();
                    SaveState();
                }
                return;
            }

            if (opState.addIndex == 0) {
                activeFunction = 2;
                opState.step = 1;
                UpdateAdd();
                return;
            }

            opState.newNode->position = {200, 100};
            opState.newNode->alpha = 0.0f;
            opState.newNode->animationType = 2;
            opState.newNode->animationProgress = 0.0f;
            opState.delayTimer = 0.5f;
            opState.step = 2;
            SaveState();
        } else if (opState.step == 2) {
            activeLine = 2; // Highlight "while (!cur && cur->val != value)"
            if (opState.traversalIndex == 0) {
                opState.current = head;
            }
            if (opState.traversalIndex < opState.addIndex) {
                activeLine = 3; // Highlight "cur = cur->next"
                if (opState.current) {
                    opState.current->animationType = 1;
                    opState.current->animationProgress = 0.0f;
                    opState.current = opState.current->next;
                    opState.traversalIndex++;
                    opState.delayTimer = 0.5f;
                    SaveState();
                } else {
                    notification = "Index " + std::to_string(opState.addIndex) + " out of bounds.";
                    delete opState.newNode;
                    opState.newNode = nullptr;
                    ResetOperation();
                    SaveState();
                }
            } else {
                opState.step = 3;
                SaveState();
            }
        } else if (opState.step == 3) {
            activeLine = 4; // Highlight "prev = prev->next"
            Node* prev = head;
            for (int i = 0; i < opState.addIndex - 1 && prev && prev->next; i++) {
                prev = prev->next;
            }
            if (prev) {
                opState.newNode->targetPosition = {prev->position.x + SPACING, 300};
                opState.step = 4;
                opState.delayTimer = 0.5f;
                SaveState();
            } else {
                notification = "Index " + std::to_string(opState.addIndex) + " out of bounds.";
                delete opState.newNode;
                opState.newNode = nullptr;
                ResetOperation();
                SaveState();
            }
        } else if (opState.step == 4) {
            activeLine = 5; // Highlight "if (cur) newNode->next = cur"
            Node* prev = head;
            for (int i = 0; i < opState.addIndex - 1 && prev && prev->next; i++) {
                prev = prev->next;
            }
            if (prev) {
                opState.newNode->next = prev->next;
                opState.delayTimer = 0.5f;
                opState.step = 5;
                SaveState();
            }
        } else if (opState.step == 5) {
            activeLine = 6; // Highlight "prev->next = newNode"
            Node* prev = head;
            for (int i = 0; i < opState.addIndex - 1 && prev && prev->next; i++) {
                prev = prev->next;
            }
            if (prev) {
                prev->next = opState.newNode;
                prev->animationType = 5;
                prev->animationProgress = 0.0f;
                RealignNodes();
                notification = "Added node with value " + std::to_string(opState.newNode->value) + " at index " + std::to_string(opState.addIndex) + ".";
                opState.newNode = nullptr;
                ResetOperation();
                SaveState();
            }
        }
    } else if (activeFunction == 4) { // Add Tail
        if (opState.step == 1) {
            activeLine = 1; // Highlight "if (!head) head = new Node(value)"
            if (!head) {
                opState.newNode->position = {200, 100};
                opState.newNode->alpha = 0.0f;
                opState.newNode->targetPosition = {200, 300};
                opState.newNode->animationType = 2;
                opState.newNode->animationProgress = 0.0f;
                head = opState.newNode;
                notification = "Added node with value " + std::to_string(opState.newNode->value) + " at tail.";
                opState.newNode = nullptr;
                ResetOperation();
                SaveState();
            } else {
                opState.current = head;
                opState.traversalIndex = 0;
                opState.step = 2;
                opState.delayTimer = 0.5f;
                SaveState();
            }
        } else if (opState.step >= 2) { // Steps 2+: Traverse nodes until the end
            activeLine = 2; // Highlight "while (cur->next)"
            if (opState.current && opState.current->next) {
                activeLine = 3; // Highlight "cur = cur->next"
                opState.current->animationType = 1;
                opState.current->animationProgress = 0.0f;
                opState.current = opState.current->next;
                opState.traversalIndex++;
                opState.delayTimer = 0.5f;
                opState.step++;
                SaveState();
            } else {
                activeLine = 4; // Highlight "cur->next = new Node(value)"
                if (opState.current) {
                    opState.newNode->position = {opState.current->position.x + SPACING, 100};
                    opState.newNode->alpha = 0.0f;
                    opState.newNode->targetPosition = {opState.current->position.x + SPACING, 300};
                    opState.newNode->animationType = 2;
                    opState.newNode->animationProgress = 0.0f;
                    opState.current->next = opState.newNode;
                    opState.current->animationType = 5;
                    opState.current->animationProgress = 0.0f;
                    notification = "Added node with value " + std::to_string(opState.newNode->value) + " at tail.";
                    opState.newNode = nullptr;
                    ResetOperation();
                    SaveState();
                }
            }
        }
    }
}

void LinkedList::UpdateDelete() {
    if (opState.step == 1) {
        activeLine = 1; // Highlight "while (cur->next &&"
        if (!head) {
            activeLine = 1;
            notification = "List is empty. Cannot delete.";
            ResetOperation();
            SaveState();
            return;
        }
        if (head->value == opState.deleteValue) {
            opState.step = 5;
            opState.delayTimer = 1.0f;
            SaveState();
            return;
        }

        if (!opState.current) opState.current = head;
        opState.traversalIndex = 0;
        activeLine = 2;
        if (opState.current && opState.current->next && opState.current->next->value != opState.deleteValue) {
            activeLine = 3;
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
            opState.current = opState.current->next;
            opState.traversalIndex++;
            opState.delayTimer = 1.0f;
            opState.step = 2;
            SaveState();
        } else {
            opState.step = 4;
            opState.delayTimer = 1.0f;
            SaveState();
        }
    } else if (opState.step >= 2 && opState.step <= 3) { // Steps 2-3: Traverse nodes
        activeLine = 2;
        if (opState.current && opState.current->next && opState.current->next->value != opState.deleteValue) {
            activeLine = 3;
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
            opState.current = opState.current->next;
            opState.traversalIndex++;
            opState.delayTimer = 1.0f;
            opState.step++;
            SaveState();
        } else {
            opState.step = 4;
            opState.delayTimer = 1.0f;
            SaveState();
        }
    } else if (opState.step == 4) {
        activeLine = 4;
        if (opState.current && opState.current->next) {
            opState.step = 5;
            opState.delayTimer = 1.0f;
            SaveState();
        } else {
            activeLine = 4;
            notification = "Value " + std::to_string(opState.deleteValue) + " not found.";
            ResetOperation();
            SaveState();
        }
    } else if (opState.step == 5) {
        activeLine = 5;
        if (opState.current) {
            Node* temp = opState.current->next;
            temp->animationType = 3;
            temp->animationProgress = 0.0f;
            opState.delayTimer = 1.0f;
            opState.step = 6;
            SaveState();
        } else if (head->value == opState.deleteValue) {
            Node* temp = head;
            temp->animationType = 3;
            temp->animationProgress = 0.0f;
            head = head->next;
            delete temp;
            RealignNodes();
            notification = "Deleted node with value " + std::to_string(opState.deleteValue) + ".";
            ResetOperation();
            SaveState();
        }
    } else if (opState.step == 6) {
        activeLine = 6;
        if (head->value == opState.deleteValue) {
            Node* temp = head;
            temp->animationType = 3;
            temp->animationProgress = 0.0f;
            head = head->next;
            delete temp;
        } else if (opState.current) {
            Node* temp = opState.current->next;
            opState.current->next = temp->next;
            delete temp;
        }
        RealignNodes();
        notification = "Deleted node with value " + std::to_string(opState.deleteValue) + ".";
        ResetOperation();
        SaveState();
    }
}

void LinkedList::UpdateSearch() {
    if (opState.step == 1) {
        activeLine = 1;
        if (opState.current) {
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
            if (opState.current->value == opState.searchValue) {
                opState.step = 4;
                opState.delayTimer = 0.5f;
                SaveState();
            } else {
                activeLine = 2;
                opState.current = opState.current->next;
                opState.traversalIndex++;
                opState.delayTimer = 0.5f;
                opState.step = 2;
                SaveState();
                if (!opState.current) {
                    opState.step = 5;
                    SaveState();
                }
            }
        } else {
            activeLine = 1;
            opState.searchResult = false;
            notification = "List is empty. Cannot search.";
            ResetOperation();
            SaveState();
        }
    } else if (opState.step >= 2 && opState.step <= 3) { // Steps 2-3: Traverse nodes
        activeLine = 1;
        if (opState.current) {
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
            if (opState.current->value == opState.searchValue) {
                opState.step = 4;
                opState.delayTimer = 0.5f;
                SaveState();
            } else {
                activeLine = 2;
                opState.current = opState.current->next;
                opState.traversalIndex++;
                opState.delayTimer = 0.5f;
                opState.step++;
                SaveState();
                if (!opState.current) {
                    opState.step = 5;
                    SaveState();
                }
            }
        }
    } else if (opState.step == 4) {
        activeLine = 3;
        opState.searchResult = true;
        foundNode = opState.current;
        notification = "Found value " + std::to_string(opState.searchValue) + ".";
        ResetOperation();
        SaveState();
    } else if (opState.step == 5) {
        activeLine = 4;
        opState.searchResult = false;
        notification = "Value " + std::to_string(opState.searchValue) + " not found.";
        ResetOperation();
        SaveState();
    }
}

void LinkedList::UpdateUpdate() {
    if (opState.step == 1) {
        activeLine = 1;
        if (opState.current) {
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
            if (opState.current->value == opState.updateOld) {
                opState.step = 4;
                opState.delayTimer = 0.5f;
                SaveState();
            } else {
                activeLine = 2;
                opState.current = opState.current->next;
                opState.traversalIndex++;
                opState.delayTimer = 0.5f;
                opState.step = 2;
                SaveState();
                if (!opState.current) {
                    activeLine = 2;
                    notification = "Value " + std::to_string(opState.updateOld) + " not found.";
                    ResetOperation();
                    SaveState();
                }
            }
        } else {
            activeLine = 1;
            notification = "List is empty. Cannot update.";
            ResetOperation();
            SaveState();
        }
    } else if (opState.step >= 2 && opState.step <= 3) { // Steps 2-3: Traverse nodes
        activeLine = 1;
        if (opState.current) {
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
            if (opState.current->value == opState.updateOld) {
                opState.step = 4;
                opState.delayTimer = 0.5f;
                SaveState();
            } else {
                activeLine = 2;
                opState.current = opState.current->next;
                opState.traversalIndex++;
                opState.delayTimer = 0.5f;
                opState.step++;
                SaveState();
                if (!opState.current) {
                    activeLine = 2;
                    notification = "Value " + std::to_string(opState.updateOld) + " not found.";
                    ResetOperation();
                    SaveState();
                }
            }
        }
    } else if (opState.step == 4) {
        activeLine = 3;
        if (opState.current) {
            opState.current->animationType = 3;
            opState.current->animationProgress = 0.0f;
            opState.delayTimer = 1.0f;
            opState.step = 5;
            SaveState();
        }
    } else if (opState.step == 5) {
        activeLine = 3;
        if (opState.current && opState.current->animationType == 0) {
            Node* newNode = new Node(opState.updateNew);
            newNode->position = {opState.current->position.x, 100};
            newNode->alpha = 0.0f;
            newNode->targetPosition = opState.current->position;
            newNode->animationType = 2;
            newNode->animationProgress = 0.0f;

            if (opState.current == head) {
                newNode->next = head->next;
                head = newNode;
            } else {
                Node* prev = head;
                while (prev && prev->next != opState.current) {
                    prev = prev->next;
                }
                if (prev) {
                    newNode->next = opState.current->next;
                    prev->next = newNode;
                    prev->animationType = 5;
                    prev->animationProgress = 0.0f;
                }
            }

            Node* temp = opState.current;
            delete temp;

            opState.current = newNode;
            opState.step = 6;
            opState.delayTimer = 0.5f;
            SaveState();
        }
    } else if (opState.step == 6) {
        activeLine = 3;
        if (opState.current && opState.current->animationType == 0) {
            notification = "Updated value from " + std::to_string(opState.updateOld) + " to " + std::to_string(opState.updateNew) + ".";
            ResetOperation();
            SaveState();
        }
    }
}
void LinkedList::SkipAdd() {
    if (activeFunction == 2) { // Add Head
        opState.newNode->position = {200, 300};
        opState.newNode->targetPosition = {200, 300};
        opState.newNode->alpha = 1.0f; // Hiển thị ngay
        opState.newNode->next = head;
        head = opState.newNode;
        RealignNodesImmediate();
        notification = "Added node with value " + std::to_string(opState.newNode->value) + " at head.";
        opState.newNode = nullptr;
        ResetOperation();
    } else if (activeFunction == 3) { // Add Index
        if (!head && opState.addIndex == 0) {
            opState.newNode->position = {200, 300};
            opState.newNode->alpha = 1.0f;
            head = opState.newNode;
            notification = "Added node with value " + std::to_string(opState.newNode->value) + " at index 0.";
            opState.newNode = nullptr;
            ResetOperation();
            return;
        }

        Node* prev = head;
        for (int i = 0; i < opState.addIndex - 1 && prev; i++) {
            prev = prev->next;
        }
        if (prev) {
            opState.newNode->position = {prev->position.x + SPACING, 300};
            opState.newNode->alpha = 1.0f;
            opState.newNode->next = prev->next;
            prev->next = opState.newNode;
            RealignNodesImmediate();
            notification = "Added node with value " + std::to_string(opState.newNode->value) + " at index " + std::to_string(opState.addIndex) + ".";
        } else {
            notification = "Index " + std::to_string(opState.addIndex) + " out of bounds.";
            delete opState.newNode;
        }
        opState.newNode = nullptr;
        ResetOperation();
    } else if (activeFunction == 4) { // Add Tail
        if (!head) {
            opState.newNode->position = {200, 300};
            opState.newNode->alpha = 1.0f;
            head = opState.newNode;
        } else {
            Node* current = head;
            while (current->next) current = current->next;
            opState.newNode->position = {current->position.x + SPACING, 300};
            opState.newNode->alpha = 1.0f;
            current->next = opState.newNode;
        }
        notification = "Added node with value " + std::to_string(opState.newNode->value) + " at tail.";
        opState.newNode = nullptr;
        ResetOperation();
    }
}

void LinkedList::SkipDelete() {
    if (activeFunction != 3) return;

    if (!head) {
        notification = "List is empty. Cannot delete.";
        ResetOperation();
        return;
    }

    if (head->value == opState.deleteValue) {
        Node* temp = head;
        head = head->next;
        delete temp;
        RealignNodesImmediate();
        notification = "Deleted node with value " + std::to_string(opState.deleteValue) + ".";
        ResetOperation();
        return;
    }

    Node* current = head;
    while (current->next) {
        current->animationType = 0;
        current->animationProgress = 0.0f;
        current->isActive = false;
        if (current->next->value == opState.deleteValue) {
            Node* temp = current->next;
            current->next = temp->next;
            delete temp;
            RealignNodesImmediate();
            notification = "Deleted node with value " + std::to_string(opState.deleteValue) + ".";
            ResetOperation();
            return;
        }
        current = current->next;
    }

    notification = "Value " + std::to_string(opState.deleteValue) + " not found.";
    ResetOperation();
}

void LinkedList::SkipSearch() {
    if (activeFunction != 4) return;

    if (!opState.current) {
        opState.searchResult = false;
        notification = "List is empty. Cannot search.";
        ResetOperation();
        return;
    }

    Node* current = opState.current;
    while (current) {
        current->animationType = 0;
        current->animationProgress = 0.0f;
        current->isActive = false;
        if (current->value == opState.searchValue) {
            opState.searchResult = true;
            foundNode = current;
            notification = "Found value " + std::to_string(opState.searchValue) + ".";
            ResetOperation();
            return;
        }
        current = current->next;
        opState.traversalIndex++;
    }

    opState.searchResult = false;
    notification = "Value " + std::to_string(opState.searchValue) + " not found.";
    ResetOperation();
}

void LinkedList::SkipUpdate() {
    if (activeFunction != 5) return;

    if (!opState.current) {
        notification = "List is empty. Cannot update.";
        ResetOperation();
        return;
    }

    Node* current = opState.current;
    while (current) {
        current->animationType = 0;
        current->animationProgress = 0.0f;
        current->isActive = false;
        if (current->value == opState.updateOld) {
            current->value = opState.updateNew;
            notification = "Updated value from " + std::to_string(opState.updateOld) + " to " + std::to_string(opState.updateNew) + ".";
            ResetOperation();
            return;
        }
        current = current->next;
        opState.traversalIndex++;
    }

    notification = "Value " + std::to_string(opState.updateOld) + " not found.";
    ResetOperation();
}

void LinkedList::DrawNode(Node* node, Vector2 adjustedPos) {
    Color color = (node == foundNode) ? RED : (node->isActive ? YELLOW : WHITE);
    color.a = (unsigned char)(node->alpha * 255);
    DrawCircleV(adjustedPos, NODE_SIZE / 2, color);
    DrawCircleLines(adjustedPos.x, adjustedPos.y, NODE_SIZE / 2, BLACK);
    std::string text = std::to_string(node->value);
    Color textColor = BLACK;
    textColor.a = (unsigned char)(node->alpha * 255);
    DrawText(text.c_str(), adjustedPos.x - MeasureText(text.c_str(), 20) / 2, adjustedPos.y - 10, 20, textColor);
}

void LinkedList::DrawArrow(Vector2 start, Vector2 end) {
    DrawLineEx(start, {end.x - NODE_SIZE / 2, end.y}, 2, BLACK);
    Vector2 dir = Vector2NormalizeCustom(Vector2SubtractCustom(end, start));
    Vector2 arrow1 = {end.x - NODE_SIZE / 2 - 10 * dir.x + 5 * dir.y, end.y - 10 * dir.y - 5 * dir.x};
    Vector2 arrow2 = {end.x - NODE_SIZE / 2 - 10 * dir.x - 5 * dir.y, end.y - 10 * dir.y + 5 * dir.x};
    DrawLineEx({end.x - NODE_SIZE / 2, end.y}, arrow1, 2, BLACK);
    DrawLineEx({end.x - NODE_SIZE / 2, end.y}, arrow2, 2, BLACK);
}

void LinkedList::RealignNodes() {
    Node* current = head;
    float x = 200;
    while (current) {
        current->targetPosition = {x, 300};
        if (current->animationType != 2) { // Không ghi đè nếu đang drop
            current->animationType = 4; // Realign
            current->animationProgress = 0.0f;
        }
        x += SPACING;
        current = current->next;
    }
}

void LinkedList::RealignNodesImmediate() {
    Node* current = head;
    float x = 200;
    while (current) {
        current->targetPosition = {x, 300};
        current->position = current->targetPosition;
        current->animationType = 0;
        current->animationProgress = 0.0f;
        x += SPACING;
        current = current->next;
    }
}

void LinkedList::SaveState() {
    ListState state;
    Node* current = head;

    // Lưu trạng thái của các nút
    while (current) {
        state.values.push_back(current->value);
        state.positions.push_back(current->position);
        state.isActive.push_back(current->isActive);
        state.alphas.push_back(current->alpha);
        state.targetPositions.push_back(current->targetPosition);
        state.animationProgresses.push_back(current->animationProgress);
        state.animationTypes.push_back(current->animationType);
        current = current->next;
    }

    // Lưu trạng thái hoạt động
    state.activeFunction = activeFunction;
    state.activeLine = activeLine;
    state.opState = opState;

    // Thêm trạng thái vào danh sách và cập nhật chỉ số bước
    operationStates.push_back(state);
    currentStepIndex = operationStates.size() - 1;

}

void LinkedList::LoadState(const ListState& state) {
    size_t size = state.values.size();
    if (state.positions.size() != size || state.isActive.size() != size ||
        state.alphas.size() != size || state.targetPositions.size() != size ||
        state.animationProgresses.size() != size || state.animationTypes.size() != size) {
        SetNotification("Error: Corrupted state detected. Cannot load state.");
        Clear();
        return;
    }

    Clear();
    Node* current = nullptr;
    for (size_t i = 0; i < size; i++) {
        Node* newNode = new Node(state.values[i]);
        newNode->position = state.positions[i];
        newNode->isActive = state.isActive[i];
        newNode->alpha = state.alphas[i];
        newNode->targetPosition = state.targetPositions[i];
        newNode->animationProgress = state.animationProgresses[i];
        newNode->animationType = state.animationTypes[i];
        if (!head) {
            head = newNode;
            current = head;
        } else {
            current->next = newNode;
            current = newNode;
        }
    }

    activeFunction = state.activeFunction;
    activeLine = state.activeLine;
    opState = state.opState;

    opState.current = nullptr;
    opState.newNode = nullptr;
    foundNode = nullptr;

    if (opState.traversalIndex > 0) {
        current = head;
        int index = 0;
        while (current && index < opState.traversalIndex) {
            current = current->next;
            index++;
        }
        opState.current = current;
    }

    if (state.activeFunction == 6 && opState.searchResult) {
        current = head;
        while (current) {
            if (current->value == opState.searchValue) {
                foundNode = current;
                break;
            }
            current = current->next;
        }
    }
}

void LinkedList::ResetOperation() {
    activeFunction = 0;
    activeLine = 0;
    opState = OperationState();
    // Do not clear operationStates or reset currentStepIndex
}

float LinkedList::Lerp(float start, float end, float t) {
    return start + t * (end - start);
}