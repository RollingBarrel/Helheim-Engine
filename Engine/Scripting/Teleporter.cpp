#include "Teleporter.h"

#include "GameObject.h"
#include <BoxColliderComponent.h>
#include "Application.h"
#include "ModuleDetourNavigation.h"


CREATE(Teleporter)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mDestination);
    MEMBER(MemberType::FLOAT, mDuration);
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
    /*
    mDistance = mGameObject->GetWorldPosition().Distance(mDestination->GetWorldPosition());
    mDirection = mDestination->GetWorldPosition().Sub(mGameObject->GetWorldPosition()).Normalized();
    mStartPos = mGameObject->GetWorldPosition();
    */
}

void Teleporter::Update()
{

    if (mIsTriggered)
    {
        mCurrentTime += App->GetDt();
        float3 position = LerpPosition(mDuration);
        mPlayer->SetWorldPosition(position);
        mGameObject->SetWorldPosition(position);
        if (mCurrentTime > mDuration)
        {
            mIsTriggered = false;
            mIsExiting = true;
            mCurrentTime = 0.0f;
            mGameObject->SetWorldPosition(mStartPos);

            //Compute nearest nav point to player after reaching elevator stop
            mStartPos = mPlayer->GetWorldPosition();
            float3 destination = App->GetNavigation()->FindNearestPoint(mStartPos, float3(1.0f));
            mDistance = mStartPos.Distance(destination);
            mDirection = destination.Sub(mStartPos).Normalized();

        }
    }
    else if (mIsEntering)
    {
        mCurrentTime += App->GetDt();
        float3 positon = LerpPosition(mEnterDuration);
        mPlayer->SetWorldPosition(positon);
        if (mCurrentTime > mEnterDuration)
        {
            mIsTriggered = true;
            mIsEntering = false;
            mCurrentTime = 0.0f;

            mDistance = mGameObject->GetWorldPosition().Distance(mDestination->GetWorldPosition());
            mDirection = mDestination->GetWorldPosition().Sub(mGameObject->GetWorldPosition()).Normalized();
            mStartPos = mGameObject->GetWorldPosition();


        }

    }
    else if (mIsExiting)
    {
        mCurrentTime += App->GetDt();
        float3 positon = LerpPosition(mEnterDuration);
        mPlayer->SetWorldPosition(positon);
        if (mCurrentTime > mEnterDuration)
        {
            mIsExiting = false;
            mCurrentTime = 0.0f;
            mPlayer->GetComponent(ComponentType::SCRIPT)->SetEnable(true);

        }
    }
}


void Teleporter::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag() == "Player")
    {
        // TODO: Pause player movement
        //collisionData->collidedWith->SetWorldPosition(mDestination->GetWorldPosition());
        if (mIsEntering || mIsTriggered || mIsExiting)
        {
            return;
        }
        mCurrentTime = 0.0f;
        mIsEntering = true;
        mPlayer = collisionData->collidedWith;

        
        mDistance = mGameObject->GetWorldPosition().Distance(mPlayer->GetWorldPosition());
        mDirection = mGameObject->GetWorldPosition().Sub(mPlayer->GetWorldPosition()).Normalized();
        mStartPos = mPlayer->GetWorldPosition();

        mPlayer->GetComponent(ComponentType::SCRIPT)->SetEnable(false);

    }
}

float3 Teleporter::LerpPosition(float duration)
{
    if (mCurrentTime < 0.0f)
    {
        return mStartPos;
    }
    else if (mCurrentTime > duration)
    {
        return mDestination->GetWorldPosition();
    }
    float halfDuration = duration / 2;
    float maxVelocity = mDistance / duration;

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
