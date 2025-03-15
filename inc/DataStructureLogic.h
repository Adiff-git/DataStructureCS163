#ifndef DATA_STRUCTURE_LOGIC_H
#define DATA_STRUCTURE_LOGIC_H

#include "Animation.h"
#include "raylib.h"
#include <vector>

class DataStructureLogic : public Animation {
protected:
    virtual void* CopyState() = 0;
    virtual void Draw(Font font, void* state, int x, int y) = 0;
    virtual void ClearStates() = 0;

public:
    DataStructureLogic();
    virtual ~DataStructureLogic() override;
    virtual void Initialize(int param) = 0;
    virtual void Add(int value) = 0;
    virtual void Delete(int value) = 0;
    virtual void Update(int oldValue, int newValue) = 0;
    virtual bool Search(int value) = 0;
    void DrawAnimation(Font font, int x, int y) override;
    void UpdateAnimation() override;
};

#endif