#include "DataStructureLogic.h"
#include "Animation.h"
#include "raylib.h"

DataStructureLogic::DataStructureLogic() {}
DataStructureLogic::~DataStructureLogic() {
    ClearSteps();
}

void DataStructureLogic::DrawAnimation(Font font, int x, int y) {
    if (currentStep >= 0 && currentStep < static_cast<int>(stepStates.size())) {
        Draw(font, stepStates[currentStep], x, y);
    }
}

void DataStructureLogic::UpdateAnimation() {
    Animation::UpdateAnimation();
}