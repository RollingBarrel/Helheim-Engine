#pragma once
#include "Component.h"
#include "float3.h"
#include "float2.h"

class ResourceTexture;
class ModuleResource;
class CanvasComponent;
class MaskComponent;
class Transform2DComponent;

class ENGINE_API ImageComponent : public Component
{
public:
    ImageComponent(GameObject* owner);
    ImageComponent(const ImageComponent& original, GameObject* owner);
    ImageComponent(GameObject* owner, bool active);
    ~ImageComponent();

    void Reset() override {}
    void Update();
    bool CleanUp();
    Component* Clone(GameObject* owner) const override;

    void Draw();

    void SetImage(ResourceTexture* image) { mImage = image; }
    void FillVBO();
    void FillSpriteSheetVBO();
    void CreateVAO();
    void ResizeByRatio();
    void RenderMask();

    unsigned int GetResourceId() const { return mResourceId; }
    ResourceTexture* GetImage() const { return mImage; }
    float3* GetColor() { return &mColor; }
    float* GetAlpha() { return &mAlpha; }
    const char* GetFileName() const { return mFileName; }
    bool* GetMantainRatio() { return &mMantainRatio; }
    bool GetIsMaskable() { return mIsMaskable; }
    bool GetShouldDraw() { return mShouldDraw; }

    inline void SetFileName(const char* fileName) { mFileName = fileName; }
    inline void SetImage(unsigned int resourceId);
    inline void SetMask(ImageComponent* mask) { mMask = mask; }
    inline void SetColor(float3 color) { mColor = color; }
    inline void SetAlpha(float alpha) { mAlpha = alpha; }
    inline void SetMantainRatio(bool ratio) { mMantainRatio = ratio; }
    inline void SetMaskable(bool maskable) { mIsMaskable = maskable; }
    inline void SetShouldDraw(bool draw) { mShouldDraw = draw; }

    bool IsSpritesheet() const { return mIsSpritesheet; }
    void SetIsSpritesheet(bool isSpritesheet) { mIsSpritesheet = isSpritesheet; }
    int GetColumns() const { return mColumns; }
    int GetRows() const { return mRows; }
    void SetSpritesheetLayout(int columns, int rows);
    int GetFrameDuration() const { return mFPS; }
    void SetFrameDuration(int frameDuration) { mFPS = frameDuration; }

    void PlayAnimation();
    void PauseAnimation();
    void StopAnimation();

    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;
    GameObject* FindCanvasOnParents(GameObject* gameObject);

private:
    std::vector<unsigned char> GetPixelData(ResourceTexture* texture);

    ResourceTexture* mImage = nullptr;
    ImageComponent* mMask = nullptr;
    MaskComponent* mMaskComponent = nullptr;
    unsigned int mResourceId = 148626881; // Default white texture

    //TODO: Handle filename when setting the image
    const char* mFileName = nullptr;

    math::float3 mColor = math::float3(1.0f, 1.0f, 1.0f);
    float mAlpha = 1.0f;

    float2 mTexOffset = float2::zero;
    bool mHasDiffuse = true;
    bool mMantainRatio = true;
    bool mShouldDraw = true;
    bool mIsMaskable = false;

    unsigned int mQuadVBO = 0;
    unsigned int mQuadVAO = 0;

    //Spritesheet
    bool mIsSpritesheet = false;
    int mColumns = 1;
    int mRows = 1;
    int mCurrentFrame = 0;
    long double mElapsedTime = 0;
    int mFPS = 30;
    bool mIsPlaying = false;

    CanvasComponent* mCanvas = nullptr;
    Transform2DComponent* mTransform = nullptr;
};
