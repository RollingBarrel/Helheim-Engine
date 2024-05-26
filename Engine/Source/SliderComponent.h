#pragma once
#include "Component.h"
#include "GameObject.h"
#include "ImageComponent.h"
#include "Transform2DComponent.h"

class CanvasComponent;

class ENGINE_API SliderComponent :
    public Component
{
public:
    SliderComponent(GameObject* owner);
    SliderComponent(GameObject* owner, bool active);
    SliderComponent(const SliderComponent& original, GameObject* owner);
    ~SliderComponent();

    void Reset() override {}
    void Update() override {}

    Component* Clone(GameObject* owner) const override;
    void SetFillPercent(float fillPercent);

    void Save(JsonObject& obj) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
    GameObject* FindCanvasOnParents(GameObject* gameObject);

    GameObject* mFill = nullptr;
    GameObject* mBackground = nullptr;
    Transform2DComponent* mSliderTransform2D = nullptr;
    ImageComponent* mBgImage = nullptr;
    ImageComponent* mFillImage = nullptr;
    Transform2DComponent* mBgTransform2D = nullptr;
    Transform2DComponent* mFillTransform2D = nullptr;
    float mFillPercent = 0.75f;

    CanvasComponent* mCanvas = nullptr;
};

