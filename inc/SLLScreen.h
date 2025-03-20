#ifndef SLL_SCREEN_H
#define SLL_SCREEN_H

#include "SinglyLinkedList.h"
#include "Button.h"
#include "Screen.h"

namespace SLLScreen {
    void DrawScreen(Font font, bool& buttonClicked, const char*& buttonMessage, Screen& currentScreen, SinglyLinkedList* sll);
}

#endif