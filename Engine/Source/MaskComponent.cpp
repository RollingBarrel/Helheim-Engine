#include "MaskComponent.h"
#include "ImageComponent.h"
#include "GameObject.h"

MaskComponent::MaskComponent(GameObject* owner) : Component(owner, ComponentType::MASK)
{
    Component* mask = owner->GetComponent(ComponentType::IMAGE);
    if (mask != nullptr) {
        mMask = static_cast<ImageComponent*>(mask);
        mDrawMask = mMask->GetShouldDraw();
    }
    else {
        mMask = nullptr;
    }
}

MaskComponent::MaskComponent(const MaskComponent& original, GameObject* owner) : Component(owner, ComponentType::MASK)
{
    mMask = original.mMask;
}

MaskComponent::MaskComponent(GameObject* owner, bool active) : Component(owner, ComponentType::MASK)
{
    mMask = static_cast<ImageComponent*>(owner->GetComponent(ComponentType::IMAGE));
}

MaskComponent::~MaskComponent()
{
    CleanUp();
    mMask = nullptr;
}

void MaskComponent::Update()
{
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

void MaskComponent::SetDrawMask(bool state)
{
    mDrawMask = state;
    mMask->SetShouldDraw(state);
}
