#pragma once
#include "Component.h"

class ResourceTexture;
class ModuleResource;
class CanvasComponent;

class ENGINE_API ImageComponent : public Component
{
public:
    ImageComponent(GameObject* owner);
    ImageComponent(const ImageComponent& original, GameObject* owner);
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
    void ResizeByRatio();

    unsigned int GetResourceId() const { return mResourceId; }
    ResourceTexture* GetImage() const { return mImage; }
    float3* GetColor() { return &mColor; }
    float* GetAlpha() { return &mAlpha; }
    const char* GetFileName() const { return mFileName; }
    bool* GetMantainRatio() { return &mMantainRatio; }
    
    inline void SetFileName(const char* fileName) { mFileName = fileName; }
    inline void SetImage(unsigned int resourceId);
    inline void SetColor(float3 color) { mColor = color; }
    inline void SetAlpha(float alpha) { mAlpha = alpha; }
    inline void SetMantainRatio(bool ratio) { mMantainRatio = ratio; }

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
    GameObject* FindCanvasOnParents(GameObject* gameObject);

private:
    ResourceTexture* mImage = nullptr;
    unsigned int mResourceId = 148626881; // Default white texture

    //TODO: Handle filename when setting the image
    const char* mFileName = nullptr;

    float3 mColor = float3(1.0f, 1.0f, 1.0f);
    float mAlpha = 1.0f;
    bool mHasAlpha = false;

    float2 mTexOffset = float2::zero;
    bool mHasDiffuse = true;
    bool mMantainRatio = true;

    unsigned int mQuadVBO = 0;
    unsigned int mQuadVAO = 0;

    CanvasComponent* mCanvas = nullptr;
};
