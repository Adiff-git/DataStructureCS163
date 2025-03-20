#ifndef PSEUDOCODE_H
#define PSEUDOCODE_H

#include "raylib.h"
#include <vector>
#include <string>

class Pseudocode {
private:
    std::vector<std::string> steps;

public:
    void SetSteps(const std::vector<std::string>& newSteps);
    void Draw(Font font, int currentStep);
};

#endif