#include "MaskComponent.h"
#include "ImageComponent.h"
#include "GameObject.h"

#include "glew.h"

MaskComponent::MaskComponent(GameObject* owner) : Component(owner, ComponentType::MASK)
{
    Component* mask = owner->GetComponent(ComponentType::IMAGE);
    if (mask != nullptr) {
        mMask = static_cast<ImageComponent*>(mask);
        mDrawMask = mMask->GetShouldDraw();
    }
    UpdateMaskToChilds();
}

MaskComponent::MaskComponent(const MaskComponent& original, GameObject* owner) : Component(owner, ComponentType::MASK)
{
    mMask = original.mMask;
    mDrawMask = original.mDrawMask;
}

MaskComponent::MaskComponent(GameObject* owner, bool active) : Component(owner, ComponentType::MASK)
{
    mMask = static_cast<ImageComponent*>(owner->GetComponent(ComponentType::IMAGE));
    mDrawMask = mMask->GetShouldDraw();
}

MaskComponent::~MaskComponent()
{
    CleanUp();
    mMask = nullptr;
}

void MaskComponent::Update()
{
    glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_ALWAYS, 1, 0x00);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    if(mMask) mMask->Draw();

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

bool MaskComponent::CleanUp()
{
    return true;
}

Component* MaskComponent::Clone(GameObject* owner) const
{
    return new MaskComponent(*this, owner);
}

void MaskComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    obj.AddBool("DrawMask", mDrawMask);
}

void MaskComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    Component::Load(data, uidPointerMap);
    mDrawMask = data.GetBool("DrawMask");
}

void MaskComponent::UpdateMaskToChilds()
{
    for (auto child : mOwner->GetChildren()) {
		Component* image = child->GetComponent(ComponentType::IMAGE);
        if (image != nullptr) {
			static_cast<ImageComponent*>(image)->SetMask(mMask);
		}
	}
}

void MaskComponent::SetDrawMask(bool state)
{
    mDrawMask = state;
    mMask->SetShouldDraw(state);
}
