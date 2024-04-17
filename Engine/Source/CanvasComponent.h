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

    inline float2 GetSize() { return mSize; }

    inline void SetSize(float2 size) { mSize = size; }

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:

    float2 mSize;

    //float mScreenFactor;
    //float2 mScreenReferenceSize;
};
