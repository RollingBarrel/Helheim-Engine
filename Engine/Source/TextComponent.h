#pragma once
#include "Component.h"
#include "GameObject.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

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
    void RenderText(const std::string& text, float x, float y, float scale, const float3& color);

private:
    void InitFreeType();
    void LoadFont(const std::string& fontPath);
    void FillVBO();
    void CreateVAO();

    struct Character {
        unsigned int  TextureID; // ID handle of the glyph texture
        float2 Size;  // Size of glyph
        float2 Bearing;  // Offset from baseline to left/top of glyph
        unsigned int  Advance;  // Offset to advance to next glyph
    };

    std::map<char, Character> Characters;
    FT_Library ft;
    FT_Face face;

    unsigned int  mQuadVAO, mQuadVBO;
};