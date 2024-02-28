#pragma once
#include "Component.h"

class CanvasComponent : public Component
{
public:
    CanvasComponent(bool active, GameObject* owner);
    ~CanvasComponent();

private:
    std::pair<int, int> screenReferenceSize;
    std::pair<int, int> size;
    float screenFactor;
};

