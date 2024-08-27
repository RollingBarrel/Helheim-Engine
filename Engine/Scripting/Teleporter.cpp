#include "Teleporter.h"

#include "GameObject.h"
#include <BoxColliderComponent.h>



CREATE(Teleporter)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mDestination);
    END_CREATE;
}


Teleporter::Teleporter(GameObject* owner) : Script (owner)
{
}

Teleporter::~Teleporter()
{
}

void Teleporter::Start()
{

    // COLLIDER
    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&Teleporter::OnCollisionEnter, this, std::placeholders::_1)));
    }
}


void Teleporter::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag() == "Player")
    {
        collisionData->collidedWith->SetWorldPosition(mDestination->GetWorldPosition());
    }
}