#pragma once
#include "Component.h"
#include "GameObject.h"

class ImageComponent;
class ButtonComponent;

class SliderComponent :
    public Component
{
public:
    SliderComponent(GameObject* owner);
    SliderComponent(GameObject* owner, bool active);
    ~SliderComponent();

    void Reset() override {}
    void Update() override {}

    Component* Clone(GameObject* owner) const override;
    void SetFillPercent(float fillPercent);

private:
    void Init();
    float fillPercent = 0.75f;
};

