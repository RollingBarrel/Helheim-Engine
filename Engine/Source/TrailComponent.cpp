#include "TrailComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "Timer.h"
#include "Trail.h"

#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define COLOR_LOCATION 2
#define DIRECTION_LOCATION 3

#define COLOR_BUFFER_BINDING 30
#define WIDTH_BUFFER_BINDING 31

#define VBO_FLOAT_SIZE 9 // number of floats for every vertex 3 (position) + 2 (texcoord) + 1 (colorPos) + 3 (direction)


TrailComponent::TrailComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::TRAIL)
{
    mTrail = new Trail();
}

TrailComponent::TrailComponent(const TrailComponent& original, GameObject* owner) : Component(owner, ComponentType::TRAIL), 

mTrail(new Trail(*original.mTrail))
{
}

TrailComponent::~TrailComponent()
{
    delete mTrail;
}

Component* TrailComponent::Clone(GameObject* owner) const
{
    return new TrailComponent(*this, owner);
}

void TrailComponent::Draw() const
{
    if (IsEnabled()) mTrail->Draw();
}


void TrailComponent::Update()
{
    if (IsEnabled())
    {
        mTrail->UpdateTrailComponent(mOwner);
    }
}

void TrailComponent::Reset()
{
    *this = TrailComponent(mOwner);
}

void TrailComponent::Enable()
{
    mTrail->Enable();
}

void TrailComponent::Disable()
{
    mTrail->Disable();
}


void TrailComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    mTrail->Save(obj);
}

void TrailComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    Component::Load(data, uidPointerMap);
    mTrail->Load(data);
}
