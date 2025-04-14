#ifndef SLLMAIN_H
#define SLLMAIN_H

#include "raylib.h"
#include "LinkedList.h"

class SLLmain {
public:
    SLLmain();
    ~SLLmain();
    void Run();
    bool ShouldClose(); 

private:
    LinkedList linkedList;
    bool shouldClose; 
};

#endif