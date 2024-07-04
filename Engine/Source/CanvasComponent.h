#pragma once
#include "Component.h"
#include "Math/float2.h"

class ImageComponent;

enum class RenderSpace
{
    World,
    Screen,
    Billboard,
    WorldAxisBillboard
};
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
    inline RenderSpace GetRenderSpace() { return mRenderSpace; }

    inline void SetSize(float2 size) { mSize = size; }
    void SetRenderSpace(RenderSpace space) { mRenderSpace = space; }

    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
    float2 mSize;
    RenderSpace mRenderSpace = RenderSpace::Screen;
};
