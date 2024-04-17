#include "TextComponent.h"

TextComponent::TextComponent(GameObject* owner) : Component(owner, ComponentType::TEXT)
{
	mFont.init("Assets\\Fonts\\ShineTypewriter-lgwzd.ttf", 16);
}

TextComponent::TextComponent(GameObject* owner, bool active) : Component(owner, ComponentType::TEXT)
{
	mFont.init("Assets\\Fonts\\ShineTypewriter-lgwzd.ttf", 16);
}

TextComponent::~TextComponent()
{
}

bool TextComponent::CleanUp()
{
	mFont.clean();

	return false;
}

Component* TextComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void TextComponent::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                 // Clear Screen And Depth Buffer
    glLoadIdentity();                                   // Reset The Current Modelview Matrix
    glTranslatef(0.0f, 0.0f, -1.0f);                              // Move One Unit Into The Screen

    // Blue text
    glColor3ub(0, 0, 0xff);

    // Position the WGL Text On The Screen
    //glRasterPos2f(-0.40f, 0.35f);
    //glPrint("Active WGL Bitmap Text With NeHe - %7.2f", cnt1);  // Print GL Text To The Screen

    // Here We Print Some Text Using Our Freetype Font
    // The Only Really Important Command Is The Actual Print() Call,
    // But For The Sake Of Making The Results A Bit More Interesting
    // I Have Put In Some Code To Rotate And Scale The Text.

    // Red Text
    glColor3ub(0xff, 0, 0);

    glPushMatrix();
    glLoadIdentity();
    //glRotatef(cnt1, 0, 0, 1);
    //glScalef(1, .8 + .3 * cos(cnt1 / 5), 1);
    //glTranslatef(-180, 0, 0);
    printText(mFont, 0, 0, "Active FreeType Text");
    glPopMatrix();

    // Uncomment This To Test Out Print's Ability To Handle Newlines.
    // freetype::print(our_font, 320, 200, "Here\nthere\nbe\n\nnewlines\n.", cnt1);

    //cnt1 += 0.051f;                                       // Increase The First Counter
    //cnt2 += 0.005f;                                       // Increase The First Counter
}

void TextComponent::Save(Archive& archive) const
{
}

void TextComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
