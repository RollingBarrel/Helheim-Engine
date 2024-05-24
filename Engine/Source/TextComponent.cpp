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

TextComponent::TextComponent(GameObject* owner) : Component(owner, ComponentType::TEXT) 
{
    InitFreeType();
    LoadFont("Assets\\Fonts\\13_5Atom_Sans_Regular.ttf");
    CreateBuffers();

    mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
}

TextComponent::TextComponent(const TextComponent& other, GameObject* owner)
    : Component(owner, ComponentType::TEXT),
    mCharacters(other.mCharacters),
    mFontSize(other.mFontSize),
    mLineSpacing(other.mLineSpacing),
    mLineWidth(other.mLineWidth),
    mColor(other.mColor),
    mAlpha(other.mAlpha),
    mText(other.mText),
    mQuadVAO(0),
    mQuadVBO(0),
    mCanvas(nullptr) {
    InitFreeType();
    CreateBuffers();

    mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));

    LoadFont("Assets\\Fonts\\13_5Atom_Sans_Regular.ttf");
}

TextComponent::~TextComponent() 
{
    glDeleteBuffers(1, &mQuadVBO);
    glDeleteVertexArrays(1, &mQuadVAO);

    // FreeType cleanup
    if (mFace) {
        FT_Done_Face(mFace);
    }
    if (mFt) {
        FT_Done_FreeType(mFt);
    }
}

Component* TextComponent::Clone(GameObject* owner) const
{
    return new TextComponent(*this, owner);
}

void TextComponent::InitFreeType() 
{
    if (FT_Init_FreeType(&mFt)) 
    {
        std::cerr << "Could not init FreeType Library" << std::endl;
        return;
    }
}

void TextComponent::LoadFont(const std::string& fontPath)
{
    if (FT_New_Face(mFt, fontPath.c_str(), 0, &mFace)) 
    {
        std::cerr << "Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(mFace, 0, mFontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    mCharacters.clear();
    for (unsigned char c = 0; c < 128; c++) 
    {
        if (FT_Load_Char(mFace, c, FT_LOAD_RENDER)) 
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
            mFace->glyph->bitmap.width,
            mFace->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            mFace->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        Character character = 
        {
            texture,
            float2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
            float2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
            mFace->glyph->advance.x
        };
        mCharacters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(mFace);
    FT_Done_FreeType(mFt);
}

void TextComponent::CreateBuffers() 
{
    glGenVertexArrays(1, &mQuadVAO);
    glGenBuffers(1, &mQuadVBO);
    glBindVertexArray(mQuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TextComponent::RenderText(const std::string& text)
{
    glBindVertexArray(mQuadVAO);
    int x = 0, y = 0;
    const int lineHeight = mFontSize + mLineSpacing; // Adjust line height based on your font size

    for (char c : text)
    {
        if (c == '\n')
        {
            y -= lineHeight;
            x = 0;
            continue;
        }

        Character ch = mCharacters[c];

        if (x + (ch.Advance >> 6) > mLineWidth && mLineWidth != 0)
        {
            y -= lineHeight;
            x = 0;
        }

        float xpos = x + ch.Bearing.x;
        float ypos = y - (ch.Size.y - ch.Bearing.y);
        float w = ch.Size.x;
        float h = ch.Size.y;

        if (c == ' ')
        {
            x += (ch.Advance >> 6);
            continue;
        }

        float vertices[] =
        {
             xpos,     ypos + h,   0.0f, 0.0f,
             xpos,     ypos,       0.0f, 1.0f,
             xpos + w, ypos,       1.0f, 1.0f,

             xpos + w, ypos + h,   1.0f, 0.0f,
             xpos,     ypos + h,   0.0f, 0.0f,
             xpos + w, ypos,       1.0f, 1.0f
        };

        glActiveTexture(GL_TEXTURE0);
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
    Component::Save(archive);

    archive.AddString("Text", mText.c_str());
    archive.AddFloat3("Color", mColor);
    archive.AddFloat("Alpha", mAlpha);
}

void TextComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    Component::LoadFromJSON(data, owner);

    if (data.HasMember("Text") && data["Text"].IsString())
    {
        mText = data["Text"].GetString();
    }

    if (data.HasMember("Color") && data["Color"].IsArray())
    {
        const rapidjson::Value& values = data["Color"];
        if (values.Size() == 3 && values[0].IsFloat() && values[1].IsFloat() && values[2].IsFloat())
        {
            mColor.x = values[0].GetFloat();
            mColor.y = values[1].GetFloat();
            mColor.z = values[2].GetFloat();
        }
    }

    if (data.HasMember("Alpha") && data["Alpha"].IsFloat())
    {
        mAlpha = data["Alpha"].GetFloat();
    }
}

void TextComponent::Draw() 
{
    unsigned int UIImageProgram = App->GetOpenGL()->GetTextProgram();
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

    glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());

    glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
    glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
    glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    RenderText(mText);

    // Clean
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
