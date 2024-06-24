#pragma once
#include "Component.h"

class ImageComponent;

class ENGINE_API MaskComponent : public Component
{
public:
    MaskComponent(GameObject* owner);
    MaskComponent(const MaskComponent& original, GameObject* owner);
    MaskComponent(GameObject* owner, bool active);
    ~MaskComponent();

    void Reset() override {}
    void Update();
    bool CleanUp();
    Component* Clone(GameObject* owner) const override;

    ImageComponent* GetMask() { return mMask; }
    bool GetDrawMask() { return mDrawMask; }

    void SetMask(ImageComponent* mask) { mMask = mask; }
    void SetDrawMask(bool state);

    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
    void UpdateMaskToChilds();

    ImageComponent* mMask = nullptr;
    bool mDrawMask = true;
};