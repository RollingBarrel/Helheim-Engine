#pragma once
#include <map>
#include "Component.h"

#include "float2.h"
#include "float3.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class CanvasComponent;
class GameObject;
class Component;

class ENGINE_API TextComponent : public Component
{
public:
    TextComponent(GameObject* owner);
    TextComponent(const TextComponent& original, GameObject* owner);
    ~TextComponent();

    void Reset() override {}
    void Update() override {}

    Component* Clone(GameObject* owner) const override;

    void Save(JsonObject& archive) const override;
    void Load(const JsonObject& data) override;

    void Draw();

    float3* GetColor() { return &mColor; }
    float* GetAlpha() { return &mAlpha; }
    std::string* GetText() { return &mText; }
    int* GetFontSize() { return &mFontSize; }
    int* GetLineSpacing() { return &mLineSpacing; }
    int* GetLineWidth() { return &mLineWidth; }

private:
    void InitFreeType();
    void LoadFont(const std::string& fontPath);
    void CreateBuffers();

    void RenderText(const std::string& text);
    GameObject* FindCanvasOnParents(GameObject* gameObject);

    struct Character {
        unsigned int  TextureID; // ID handle of the glyph texture
        float2 Size;  // Size of glyph
        float2 Bearing;  // Offset from baseline to left/top of glyph
        unsigned int  Advance;  // Offset to advance to next glyph
    };

    std::map<char, Character> mCharacters;
    FT_Library mFt;
    FT_Face mFace;

    int mFontSize = 48;
    int mLineSpacing = 0;
    int mLineWidth = 0;

    float3 mColor = float3(1.f,1.f,1.f);
    float mAlpha = 1.0f;
    std::string mText = "Hello World!";

    unsigned int  mQuadVAO, mQuadVBO;

    CanvasComponent* mCanvas = nullptr;
};