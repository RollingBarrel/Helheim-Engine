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
    void Draw();
    void Init();
    Component* Clone(GameObject* owner) const override;

private:
    GameObject* mBackground = nullptr;
    GameObject* mFill = nullptr;
    //ButtonComponent* mSliderButton = nullptr;
	//ImageComponent* mSliderFill = nullptr;
};

