#pragma once
#include "Component.h"

class ResourceTexture;
class ModuleResource;

class ImageComponent : public Component
{
public:
    ImageComponent(GameObject* owner);
    ImageComponent(GameObject* owner, bool active);
    ~ImageComponent();

    void Reset() override {}
    void Update() override {}
    Component* Clone(GameObject* owner) const override;

    void Draw() const;

    void SetImage(ResourceTexture* image) { mImage = image; }
    unsigned int GetResourceId() const { return mResourceId; }
    ResourceTexture* GetImage() const { return mImage; }

    void SetImage(unsigned int resourceId);

private:
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    ResourceTexture* mImage;
    float4 mColor;
    unsigned int texId = 0;
    unsigned int mResourceId = 818189439; // Temporary default image - Just a simple square made of four squares (Red, Green, Blue, White) 
};
