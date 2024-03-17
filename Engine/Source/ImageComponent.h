#pragma once
#include "Component.h"

class ResourceTexture;

class ImageComponent : public Component
{
public:
    ImageComponent( GameObject* owner,bool active);
    ImageComponent(GameObject* owner);
    ~ImageComponent();

    void Reset() override {}
    void Update() override {}
    Component* Clone(GameObject* owner) const override;

    void Draw() const;

private:
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    ResourceTexture* mImage;
    float4 mColor;
};
