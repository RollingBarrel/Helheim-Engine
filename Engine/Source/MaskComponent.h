#pragma once
#include "Component.h"

class ImageComponent;

class ENGINE_API MaskComponent : public Component
{
public:
    enum class MaskingMode
    {
        Normal,    // Draw the image where the mask is
        Inverse    // Do not draw the image where the mask is
    };

    MaskComponent(GameObject* owner);
    MaskComponent(const MaskComponent& original, GameObject* owner);
    MaskComponent(GameObject* owner, bool active);
    ~MaskComponent();

    void Reset() override {}
    void Update() override {};
    bool CleanUp();
    Component* Clone(GameObject* owner) const override;

    ImageComponent* GetMask() { return mMask; }
    bool GetDrawMask() { return mDrawMask; }
    MaskingMode GetMaskingMode() { return mMaskingMode; }

    void SetMask(ImageComponent* mask) { mMask = mask; }
    void SetDrawMask(bool state);
    void SetMaskingMode(MaskingMode mode) { mMaskingMode = mode; }

    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
    void UpdateMaskToChilds();

    ImageComponent* mMask = nullptr;

    bool mDrawMask = true;
    MaskingMode mMaskingMode = MaskingMode::Normal;
};

