#include "LinkedList.h"
#include "Button.h"
#include "Utils.h"
#include <sstream>

LinkedList::LinkedList(int width, int height) 
    : head(nullptr), isPaused(false), skipStep(false), speed(1.0f), 
      activeFunction(0), activeLine(0), scrollOffset(0.0f), notification(""), 
      screenWidth(width), screenHeight(height), foundNode(nullptr) {
    operationHistory = std::deque<ListState>();
}
void LinkedList::Clear() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    past.clear();
    future_continuous.clear();
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

    // Lưu trạng thái trước khi thực hiện thao tác vào operationHistory
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
    operationHistory.push_back(preState);

    // Xóa past và future_continuous khi bắt đầu một thao tác mới
    past.clear();
    future_continuous.clear();

    switch (op.type) {
        case 1: 
            opState.initCount = op.value1; 
            opState.initIndex = 0; 
            Clear(); 
            opState.current = nullptr;
            if (!opState.initValues.empty()) {
                opState.initCount = opState.initValues.size();
            }
            break;
        case 2: 
            opState.newNode = new Node(op.value1); 
            break;
        case 3: 
            opState.deleteValue = op.value1; 
            opState.current = head; 
            break;
        case 4: 
            opState.searchValue = op.value1; 
            opState.current = head; 
            opState.searchResult = false; 
            break;
        case 5: 
            opState.updateOld = op.value1; 
            opState.updateNew = op.value2; 
            opState.current = head; 
            break;
    }
}

void LinkedList::UpdateOperations() {
    if (activeFunction == 0) return;

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

    // Lưu trạng thái trước khi thực hiện bước lớn
    SaveState();

    switch (activeFunction) {
        case 1: UpdateInitialize(); break;
        case 2: UpdateAdd(); break;
        case 3: UpdateDelete(); break;
        case 4: UpdateSearch(); break;
        case 5: UpdateUpdate(); break;
    }

    // Lưu trạng thái sau khi thực hiện bước lớn và thêm vào future_continuous
    SaveState();
    future_continuous.push_back(past.back());
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
                    case 2: current->position = current->targetPosition; current->alpha = 1.0f; break;
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
                        current->position.y = Lerp(current->position.y, current->targetPosition.y, t);
                        current->alpha = t;
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

    const char* codeLines[5][7] = {
        {"Initialize(count):", "1. Clear list", "2. Loop i to count", "3. Add random node", "", "", ""},
        {"Add(value):", "1. Traverse to end", "2. Add new node", "", "", "", ""},
        {"Delete(value):", "1. while (current->next &&", "    current->next->value != value)", "2. current->next =", "    current->next->next", "", ""},
        {"Search(value):", "1. current = head", "2. while (current)", "3.     if (current->value == value) return", "", "", ""},
        {"Update(old, new):", "1. current = head", "2. while (current)", "3.     if (current->value == old)", "4.         current->value = new", "", ""}
    };

    int numLines = 0;
    for (int i = 0; i < 7; i++) {
        if (codeLines[activeFunction - 1][i][0] != '\0') numLines++;
    }

    float y = screenHeight - 200;
    float x = screenWidth - 400;
    float lineHeight = 30;
    float padding = 10;
    float frameWidth = 350;
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

void LinkedList::PreviousStep() {
    if (operationHistory.empty()) {
        Clear(); // Nếu không có trạng thái trước đó, xóa danh sách
        SetNotification("No previous state available. List cleared.");
        return;
    }

    // Khôi phục trạng thái trước đó từ operationHistory
    ListState preState = operationHistory.back();
    operationHistory.pop_back();
    LoadState(preState);
    SetNotification("Reverted to previous state.");

    // Xóa past và future_continuous, vì không còn thao tác hiện tại
    past.clear();
    future_continuous.clear();
    activeFunction = 0;
    activeLine = 0;
    opState = OperationState();
}

void LinkedList::NextStep() {
    if (future_continuous.empty()) {
        SetNotification("No next steps available.");
        return;
    }

    // Lưu trạng thái hiện tại vào past
    SaveState();

    // Lấy trạng thái từ future_continuous và khôi phục
    ListState state = future_continuous.front();
    future_continuous.pop_front();
    past.push_back(state);
    LoadState(state);
    SetNotification("Advanced to next step.");
}

void LinkedList::SkipToEnd() {
    if (activeFunction == 0) {
        SetNotification("No operation in progress.");
        return;
    }

    switch (activeFunction) {
        case 1: SkipInitialize(); break;
        case 2: SkipAdd(); break;
        case 3: SkipDelete(); break;
        case 4: SkipSearch(); break;
        case 5: SkipUpdate(); break;
    }
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
        activeLine = 1;
        if (opState.initIndex < opState.initCount) {
            activeLine = 2;
            activeLine = 3;
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
        } else {
            notification = "Initialized " + std::to_string(opState.initCount) + " nodes.";
            ResetOperation();
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
    if (opState.step == 1) {
        activeLine = 1;
        if (!head) {
            activeLine = 2;
            opState.newNode->position = {200, 100};
            opState.newNode->alpha = 0.0f;
            opState.newNode->animationType = 0;
            opState.newNode->animationProgress = 0.0f;
            opState.delayTimer = 1.0f;
            opState.step = 2;
        } else {
            if (!opState.current) opState.current = head;
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
            if (opState.current->next) {
                opState.current = opState.current->next;
            } else {
                activeLine = 2;
                opState.newNode->position = {opState.current->position.x + SPACING, 100};
                opState.newNode->alpha = 0.0f;
                opState.newNode->animationType = 0;
                opState.newNode->animationProgress = 0.0f;
                opState.delayTimer = 1.0f;
                opState.step = 2;
            }
        }
    } else if (opState.step == 2) {
        activeLine = 2;
        if (!head) {
            opState.newNode->targetPosition = {200, 300};
            opState.newNode->animationType = 2;
            opState.newNode->animationProgress = 0.0f;
            head = opState.newNode;
            notification = "Added node with value " + std::to_string(opState.newNode->value) + ".";
            opState.newNode = nullptr;
            ResetOperation();
        } else {
            opState.newNode->targetPosition = {opState.current->position.x + SPACING, 300};
            opState.newNode->animationType = 2;
            opState.newNode->animationProgress = 0.0f;
            opState.current->next = opState.newNode;
            opState.current->animationType = 5;
            opState.current->animationProgress = 0.0f;
            notification = "Added node with value " + std::to_string(opState.newNode->value) + ".";
            opState.newNode = nullptr;
            ResetOperation();
        }
    }
}

void LinkedList::UpdateDelete() {
    if (opState.step == 1) {
        activeLine = 1;
        if (!head) {
            notification = "List is empty. Cannot delete.";
            ResetOperation();
            return;
        }
        if (head->value == opState.deleteValue) {
            activeLine = 2;
            Node* temp = head;
            temp->animationType = 3;
            temp->animationProgress = 0.0f;
            head = head->next;
            delete temp;
            RealignNodes();
            notification = "Deleted node with value " + std::to_string(opState.deleteValue) + ".";
            ResetOperation();
            return;
        }

        if (!opState.current) opState.current = head;
        if (opState.current) {
            opState.current->animationType = 1;
            opState.current->animationProgress = 0.0f;
        }
        if (opState.current->next) {
            if (opState.current->next->value == opState.deleteValue) {
                activeLine = 2;
                Node* temp = opState.current->next;
                temp->animationType = 3;
                temp->animationProgress = 0.0f;
                opState.current->next = temp->next;
                delete temp;
                RealignNodes();
                notification = "Deleted node with value " + std::to_string(opState.deleteValue) + ".";
                ResetOperation();
            } else {
                opState.current = opState.current->next;
            }
        } else {
            notification = "Value " + std::to_string(opState.deleteValue) + " not found.";
            ResetOperation();
        }
    }
}

void LinkedList::UpdateSearch() {
    if (opState.step == 1) {
        if (opState.current) {
            activeLine = 1;
            activeLine = 2;
            if (opState.current) {
                opState.current->animationType = 1;
                opState.current->animationProgress = 0.0f;
            }
            activeLine = 3;
            if (opState.current->value == opState.searchValue) {
                opState.searchResult = true;
                foundNode = opState.current;
                notification = "Found value " + std::to_string(opState.searchValue) + ".";
                ResetOperation();
            } else {
                opState.current = opState.current->next;
                opState.traversalIndex++;
                if (!opState.current) {
                    opState.searchResult = false;
                    notification = "Value " + std::to_string(opState.searchValue) + " not found.";
                    ResetOperation();
                }
            }
        } else {
            opState.searchResult = false;
            notification = "List is empty. Cannot search.";
            ResetOperation();
        }
    }
}

void LinkedList::UpdateUpdate() {
    if (opState.step == 1) {
        if (opState.current) {
            activeLine = 1;
            activeLine = 2;
            if (opState.current) {
                opState.current->animationType = 1;
                opState.current->animationProgress = 0.0f;
            }
            activeLine = 3;
            if (opState.current->value == opState.updateOld) {
                activeLine = 4;
                opState.current->animationType = 3;
                opState.current->animationProgress = 0.0f;
                opState.current->value = opState.updateNew;
                opState.current->alpha = 0.0f;
                Vector2 originalPos = opState.current->position;
                opState.current->position = {originalPos.x, 100};
                opState.current->targetPosition = originalPos;
                opState.current->animationType = 2;
                opState.current->animationProgress = 0.0f;
                notification = "Updated value from " + std::to_string(opState.updateOld) + " to " + std::to_string(opState.updateNew) + ".";
                ResetOperation();
            } else {
                opState.current = opState.current->next;
                opState.traversalIndex++;
                if (!opState.current) {
                    notification = "Value " + std::to_string(opState.updateOld) + " not found.";
                    ResetOperation();
                }
            }
        } else {
            notification = "List is empty. Cannot update.";
            ResetOperation();
        }
    }
}

void LinkedList::SkipAdd() {
    if (activeFunction != 2) return;

    if (!head) {
        activeLine = 2;
        opState.newNode->position = {200, 300};
        opState.newNode->targetPosition = opState.newNode->position;
        head = opState.newNode;
    } else {
        Node* current = head;
        while (current->next) {
            current->animationType = 0;
            current->animationProgress = 0.0f;
            current->isActive = false;
            current = current->next;
        }
        current->animationType = 0;
        current->animationProgress = 0.0f;
        current->isActive = false;
        activeLine = 2;
        opState.newNode->position = {current->position.x + SPACING, 300};
        opState.newNode->targetPosition = {current->position.x + SPACING, 300};
        opState.newNode->alpha = 1.0f;
        current->next = opState.newNode;
    }
    notification = "Added node with value " + std::to_string(opState.newNode->value) + ".";
    opState.newNode = nullptr;
    ResetOperation();
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
        current->animationType = 4;
        current->animationProgress = 0.0f;
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
    past.push_back(state);
}

void LinkedList::LoadState(const ListState& state) {
    Clear();
    Node* current = nullptr;
    for (size_t i = 0; i < state.values.size(); i++) {
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
}

void LinkedList::ResetOperation() {
    activeFunction = 0;
    activeLine = 0;
    opState = OperationState();
    past.clear();
    future_continuous.clear();
}

float LinkedList::Lerp(float start, float end, float t) {
    return start + t * (end - start);
}