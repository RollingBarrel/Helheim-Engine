#include "LineComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "ResourceTexture.h"
#include "ModuleResource.h"
#include "glew.h"
#include "MathGeoLib.h"
#include "Timer.h"
#include "Trail.h"


LineComponent::LineComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::LINE)
{
    mTrail = new Trail();
    mFinalPoint = SearchLineFinal(ownerGameObject);
    if (mFinalPoint == nullptr) 
    {
        mFinalPoint = new GameObject("FinalPoint", ownerGameObject);
        mFinalPoint->SetTag("LineFinal");
    }

}

LineComponent::LineComponent(const LineComponent& original, GameObject* owner) : Component(owner, ComponentType::LINE),

mTrail(new Trail(*original.mTrail))
{
    mFinalPoint = SearchLineFinal(owner);
    if (mFinalPoint == nullptr)
    {
        mFinalPoint = new GameObject("FinalPoint", owner);
        mFinalPoint->SetTag("LineFinal");
    }
}

LineComponent::~LineComponent()
{
    delete mTrail;
}

GameObject* LineComponent::SearchLineFinal(GameObject* owner)
{
    for (GameObject* child : owner->GetChildren())
    {
        if (child->GetTag()=="LineFinal") {
            return child;
        }
    }
    return nullptr;
}

Component* LineComponent::Clone(GameObject* owner) const
{
    return new LineComponent(*this, owner);
}

void LineComponent::Draw() const
{
    if (IsEnabled()) mTrail->Draw();
}


void LineComponent::Update()
{
    if (IsEnabled())
    {
        mTrail->UpdateLineComponent(mOwner, mFinalPoint);
    }
}

void LineComponent::Reset()
{
    *this = LineComponent(mOwner);
}

void LineComponent::Enable()
{
    mTrail->Enable();
}

void LineComponent::Disable()
{
    mTrail->Disable();
}


void LineComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    mTrail->Save(obj);
}

void LineComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    Component::Load(data, uidPointerMap);
    mTrail->Load(data);
}
