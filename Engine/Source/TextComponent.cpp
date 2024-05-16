//https://github.com/MissclickStudios/Projecte3/blob/main/NULL%20Engine/Source/C_UI_Text.cpp

#include "TextComponent.h"

#include <string>
#include "glew.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include <iostream>

TextComponent::TextComponent(GameObject* owner) : Component(owner, ComponentType::TEXT) {
    InitFreeType();
    LoadFont("Assets\\Fonts\\13_5Atom_Sans_Regular.ttf");
    FillVBO();
    CreateVAO();
}

TextComponent::TextComponent(const TextComponent& other, GameObject* owner)
    : Component(owner, ComponentType::TEXT), Characters(other.Characters), ft(other.ft), face(other.face) {
    FillVBO();
    CreateVAO();
}

TextComponent::~TextComponent() {
    glDeleteBuffers(1, &mQuadVBO);
    glDeleteVertexArrays(1, &mQuadVAO);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

Component* TextComponent::Clone(GameObject* owner) const
{
    return new TextComponent(*this, owner);
}

void TextComponent::InitFreeType() {
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Could not init FreeType Library" << std::endl;
        return;
    }
}

void TextComponent::LoadFont(const std::string& fontPath) {
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load Glyph" << std::endl;
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Character character = {
            texture,
            float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
}

void TextComponent::FillVBO() {
    glGenBuffers(1, &mQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TextComponent::CreateVAO() {
    glGenVertexArrays(1, &mQuadVAO);
    glBindVertexArray(mQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextComponent::RenderText(const std::string& text, float x, float y, float scale, const float3& color) {
    GLuint program = App->GetOpenGL()->GetTextProgram();
    glUseProgram(program);
    glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(mQuadVAO);

    for (char c : text) {
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void TextComponent::Save(Archive& archive) const
{
}

void TextComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}

void TextComponent::Draw() {
    RenderText("Hello, World!", 0.0f, 0.0f, 1000.0f, float3(1.0f, 1.0f, 1.0f));
}
