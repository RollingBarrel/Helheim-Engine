#include "Dependencies/Freetype/GLFont/GLFont.h"
#include "Dependencies/Freetype/GLFont/FTLabel.h"
#include "TextComponent.h"

TextComponent::TextComponent(GameObject* owner) : Component(owner, ComponentType::TEXT)
{
}

TextComponent::TextComponent(GameObject* owner, bool active) : Component(owner, ComponentType::TEXT)
{
}

TextComponent::~TextComponent()
{
}

bool TextComponent::CleanUp()
{
	return false;
}

Component* TextComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void TextComponent::Draw()
{
    shared_ptr<GLFont> glFont = shared_ptr<GLFont>(new GLFont("Assets/Fonts/ShineTypewriter-lgwzd.ttf"));

    // Some values we will use to create our labels
    int startX = 100;
    int startY = 100;
    int windowWidth = 800;
    int windowHeight = 600;

    unique_ptr<FTLabel> label = unique_ptr<FTLabel>(new FTLabel(
        glFont,         // Font face handle
        "Hello world", // Text to render
        startX,
        startY,
        windowWidth,
        windowHeight
    ));

    label->render();
}

void TextComponent::Save(Archive& archive) const
{
}

void TextComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
