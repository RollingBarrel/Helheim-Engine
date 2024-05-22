//https://github.com/MissclickStudios/Projecte3/blob/main/NULL%20Engine/Source/C_UI_Text.cpp

#include "TextComponent.h"

#include <string>
#include "glew.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include <iostream>
#include "Transform2DComponent.h"
#include "CanvasComponent.h"
#include "CameraComponent.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "Math/TransformOps.h"

TextComponent::TextComponent(GameObject* owner) : Component(owner, ComponentType::TEXT) {
    InitFreeType();
    LoadFont("Assets\\Fonts\\13_5Atom_Sans_Regular.ttf");
    CreateBuffers();

    mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
}

TextComponent::TextComponent(const TextComponent& other, GameObject* owner)
    : Component(owner, ComponentType::TEXT), Characters(other.Characters), ft(other.ft), face(other.face) {
    CreateBuffers();
}

TextComponent::~TextComponent() {
    glDeleteBuffers(1, &mQuadVBO);
    glDeleteVertexArrays(1, &mQuadVAO);
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

void TextComponent::LoadFont(const std::string& fontPath)
{
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) 
    {
        std::cerr << "Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    Characters.clear();
    for (unsigned char c = 0; c < 128; c++) 
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
        {
            LOG("Failed to load Glyph");
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        Character character = 
        {
            texture,
            float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextComponent::CreateBuffers() {

    glGenVertexArrays(1, &mQuadVAO);
    glGenBuffers(1, &mQuadVBO);
    glBindVertexArray(mQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextComponent::RenderText(const std::string& text) 
{
    int x = 0, y = 0;

    for (char c : text) 
    {
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x;
        float ypos = y - (ch.Size.y - ch.Bearing.y);
        float w = ch.Size.x;
        float h = ch.Size.y;

        float vertices[6][4] = 
        {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6);
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

void TextComponent::Draw() 
{
    unsigned int UIImageProgram = App->GetOpenGL()->GetUIImageProgram();
    if (UIImageProgram == 0) return;

    glUseProgram(UIImageProgram);

    float4x4 proj = float4x4::identity;
    float4x4 model = float4x4::identity;
    float4x4 view = float4x4::identity;

    if (mCanvas->GetScreenSpace()) //Ortographic Mode
    {
        Transform2DComponent* component = reinterpret_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
        if (component != nullptr)
        {
            model = component->GetGlobalMatrix();

            //float2 windowSize = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->GetWindowsSize();
            float2 canvasSize = ((CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS)))->GetSize();

            model = float4x4::Scale(1 / canvasSize.x * 2, 1 / canvasSize.y * 2, 0) * model;

        }
        glEnable(GL_CULL_FACE);
    }
    else //World Mode
    {
        const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();

        proj = camera->GetProjectionMatrix();
        model = GetOwner()->GetWorldTransform();
        view = camera->GetViewMatrix();
        glDisable(GL_CULL_FACE);
    }

    glBindVertexArray(mQuadVAO);
    glActiveTexture(GL_TEXTURE0);

    glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());

    glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
    glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
    glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    RenderText(text);

    // Clean
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glUseProgram(0);
}

GameObject* TextComponent::FindCanvasOnParents(GameObject* gameObject)
{
    if (gameObject == nullptr)
    {
        return nullptr;
    }

    GameObject* currentObject = gameObject;

    while (currentObject != nullptr)
    {
        if (currentObject->GetComponent(ComponentType::CANVAS) != nullptr)
        {
            return currentObject;
        }
        currentObject = currentObject->GetParent();
    }

    return nullptr; // No canvas found on parents
}
