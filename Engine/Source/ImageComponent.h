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
    bool CleanUp();
    Component* Clone(GameObject* owner) const override;

    void Draw();

    void SetImage(ResourceTexture* image) { mImage = image; }
    void FillVBO();
    void CreateVAO();
    unsigned int GetResourceId() const { return mResourceId; }
    ResourceTexture* GetImage() const { return mImage; }

    void SetImage(unsigned int resourceId);

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
    ResourceTexture* mImage = nullptr;
    float4 mColor = float4(1.0f, 1.0f, 1.0f, 0.5f);
    unsigned int mTexId = 0;
    float2 mTexOffset = float2::zero;
    bool mHasDiffuse = true;
    unsigned int mQuadVBO = 0;
    unsigned int mQuadVAO = 0;
    unsigned int mResourceId = 818189439; // Temporary default image - Just a simple square made of four squares (Red, Green, Blue, White) 
    bool hasAlpha = false;
};
