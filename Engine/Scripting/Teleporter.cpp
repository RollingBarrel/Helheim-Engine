#include "Teleporter.h"

#include "GameObject.h"
#include <BoxColliderComponent.h>
#include "GameApp.h"


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
    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&Teleporter::OnCollisionEnter, this, std::placeholders::_1)));
    }

    mDistance = mGameObject->GetWorldPosition().Distance(mDestination->GetWorldPosition());
    mDirection = mDestination->GetWorldPosition().Sub(mGameObject->GetWorldPosition()).Normalized();
    mStartPos = mGameObject->GetWorldPosition();
}

void Teleporter::Update()
{
    if (mIsTriggered)
    {
        mCurrentTime += GameApp->GetDt();
        mGameObject->SetWorldPosition(LerpPosition());
        if (mCurrentTime > mDuration)
        {
            mIsTriggered = false;
            mCurrentTime = 0.0f;
        }
    }
}


void Teleporter::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag() == "Player")
    {
        // TODO: Pause player movement
        collisionData->collidedWith->SetWorldPosition(mDestination->GetWorldPosition());
        mIsTriggered = true;
    }
}

float3 Teleporter::LerpPosition()
{
    if (mCurrentTime < 0.0f)
    {
        return mStartPos;
    }
    else if (mCurrentTime > mDuration)
    {
        return mDestination->GetWorldPosition();
    }
    float halfDuration = mDuration / 2;
    float maxVelocity = mDistance / mDuration;

    float lerpFactor = 0.0f;

    if (mCurrentTime < halfDuration)
    {
        lerpFactor = 0.5f * (maxVelocity / halfDuration) * (mCurrentTime * mCurrentTime);
    }
    else
    {
        float distAcc = 0.5f * (maxVelocity / halfDuration) * (halfDuration * halfDuration);
        lerpFactor = distAcc + maxVelocity * (mCurrentTime - halfDuration);
    }

    return mStartPos + mDirection * lerpFactor;
}
