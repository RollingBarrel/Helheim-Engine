#pragma once
#include <map>
#include "Component.h"

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

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    void Draw();
    void RenderText(const std::string& text);

    float3* GetColor() { return &mColor; }
    float* GetAlpha() { return &mAlpha; }
    std::string* GetText() { return &text; }

private:
    void InitFreeType();
    void LoadFont(const std::string& fontPath);
    void CreateBuffers();

    GameObject* FindCanvasOnParents(GameObject* gameObject);

    struct Character {
        unsigned int  TextureID; // ID handle of the glyph texture
        float2 Size;  // Size of glyph
        float2 Bearing;  // Offset from baseline to left/top of glyph
        unsigned int  Advance;  // Offset to advance to next glyph
    };

    std::map<char, Character> Characters;
    FT_Library ft;
    FT_Face face;

    float3 mColor = float3(1.f,1.f,1.f);
    float mAlpha = 1.0f;
    std::string text = "Hello World!";

    unsigned int  mQuadVAO, mQuadVBO;

    CanvasComponent* mCanvas = nullptr;
};