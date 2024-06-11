#pragma once
#include "Component.h"
#include "Math/float2.h"

class ImageComponent;

class CanvasComponent : public Component
{
public:
    CanvasComponent(bool active, GameObject* owner);
    CanvasComponent(GameObject* owner);
    CanvasComponent(const CanvasComponent& original, GameObject* owner);
    ~CanvasComponent();

    void Update() override;
    void Init();
    Component* Clone(GameObject* owner) const override;
    void Reset() override;

    inline float2 GetSize() { return mSize; }
    inline bool GetScreenSpace() { return mScreenSpace; }

    inline void SetSize(float2 size) { mSize = size; }
    void SetScreenSpace(bool screen) { mScreenSpace = screen; }

    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
    float2 mSize;

    bool mScreenSpace = true;
};
