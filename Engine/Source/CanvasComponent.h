#pragma once
#include "Component.h"

class CanvasComponent :
    public Component
{
public:
    CanvasComponent(bool active, GameObject* owner);
    ~CanvasComponent();
};

