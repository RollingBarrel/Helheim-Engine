#pragma once
#include "Component.h"
#include "Math/float2.h"

class ImageComponent;

class CanvasComponent : public Component
{
public:
    CanvasComponent(bool active, GameObject* owner);
    CanvasComponent(GameObject* owner);
    ~CanvasComponent();

    void Update() override;
    void Init();
    Component* Clone(GameObject* owner) const override;
    void Reset() override;

    inline float2 GetSize() { return size; }
    inline float GetScreenFactor() { return screenFactor; }

    void RecalculateSizeAndScreenFactor();

private:
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    float2 size;
    float screenFactor;
    float2 screenReferenceSize;

    ImageComponent* mImageComponent;
};