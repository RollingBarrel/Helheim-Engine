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
    float3* GetColor() { return &mColor; }
    float* GetAlpha() { return &mAlpha; }
    const char* GetFileName() const { return mFileName; }
    
    void SetFileName(const char* fileName) { mFileName = fileName; }
    void SetImage(unsigned int resourceId);
	void SetColor(float3 color) { mColor = color; }
    void SetAlpha(float alpha) { mAlpha = alpha; }

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
    GameObject* FindCanvasOnParents(GameObject* gameObject);

private:
    ResourceTexture* mImage = nullptr;
    unsigned int mResourceId = 0;

    //TODO: Handle filename when setting the image
    const char* mFileName = nullptr;

    float3 mColor = float3(1.0f, 1.0f, 1.0f);
    float mAlpha = 1.0f;
    bool mHasAlpha = false;

    float2 mTexOffset = float2::zero;
    bool mHasDiffuse = true;

    unsigned int mQuadVBO = 0;
    unsigned int mQuadVAO = 0;
};
