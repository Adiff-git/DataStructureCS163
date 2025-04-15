#ifndef SLLMAIN_H
#define SLLMAIN_H

#include "raylib.h"
#include "LinkedList.h"

class SLLmain {
public:
    SLLmain();
    ~SLLmain();
    void Run();
    bool ShouldClose(); // Add a method to check if the visualization should close

private:
    LinkedList linkedList;
    bool shouldClose; // Flag to indicate if the visualization should close
    bool backToMainMenu; // Flag to indicate if the "Back" button was clicked
};

#endif