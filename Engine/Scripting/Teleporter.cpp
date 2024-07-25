#include "Teleporter.h"

#include "GameObject.h"
#include <BoxColliderComponent.h>
#include "Application.h"
#include "ModuleDetourNavigation.h"
#include "AnimationComponent.h"


CREATE(Teleporter)
{
    CLASS(owner);
    MEMBER(MemberType::FLOAT3, mStartPos);
    MEMBER(MemberType::FLOAT3, mEndPos);
    MEMBER(MemberType::FLOAT, mDuration);
    MEMBER(MemberType::GAMEOBJECT, mCamera);
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
    
}

void Teleporter::Update()
{

    if (mIsTriggered)
    {
        mCurrentTime += App->GetDt();
        float3 position = LerpPosition(mDuration, mIsAtStart ? mStartPos : mEndPos);
        mPlayer->SetWorldPosition(position);
        mGameObject->SetWorldPosition(position);
        if (mCurrentTime > mDuration)
        {
            mIsTriggered = false;
            mIsExiting = true;
            mCurrentTime = 0.0f;

            if (mPlayerAnimation)
            {
                mPlayerAnimation->SendTrigger("tWalkForward", 0.2f);
                mPlayerAnimation->SendSpineTrigger("tWalkForward", 0.2f);

            }

            if (mIsAtStart)
            {

                float3 destination = App->GetNavigation()->FindNearestPoint(mEndPos + float3(mCurrentDirection.x, 0.0f, mCurrentDirection.z) * 10, float3(10.0f));
                mDistance = mPlayer->GetWorldPosition().Distance(destination);
                mCurrentDirection = destination.Sub(mPlayer->GetWorldPosition()).Normalized();
                mPlayer->LookAt(destination);


            }
            else
            {

                float3 destination = App->GetNavigation()->FindNearestPoint(mStartPos + float3(mCurrentDirection.x, 0.0f, mCurrentDirection.z) * 10, float3(10.0f));
                mDistance = mPlayer->GetWorldPosition().Distance(destination);
                mCurrentDirection = destination.Sub(mPlayer->GetWorldPosition()).Normalized();
                mPlayer->LookAt(destination);

            }


        }
    }
    else if (mIsEntering)
    {
        mCurrentTime += App->GetDt();
        float3 positon = LerpPosition(mEnterDuration, mFirstPlayerPos);
        mPlayer->SetWorldPosition(positon);
        if (mCurrentTime > mEnterDuration)
        {
            mIsTriggered = true;
            mIsEntering = false;
            mCurrentTime = 0.0f;

            if (mPlayerAnimation)
            {
                mPlayerAnimation->SendTrigger("tIdle", 0.2f);
                mPlayerAnimation->SendSpineTrigger("tIdle", 0.2f);

            }

            if (mIsAtStart)
            {
                mDistance = mStartPos.Distance(mEndPos);
                mCurrentDirection = mEndPos.Sub(mStartPos).Normalized();
            }
            else
            {
                mDistance = mStartPos.Distance(mEndPos);
                mCurrentDirection = mStartPos.Sub(mEndPos).Normalized();

            }
        }

    }
    else if (mIsExiting)
    {
        mCurrentTime += App->GetDt();
        float3 positon = LerpPosition(mEnterDuration, mIsAtStart ? mEndPos : mStartPos);
        mPlayer->SetWorldPosition(positon);
        if (mCurrentTime > mEnterDuration)
        {
            if (mPlayerAnimation)
            {
                mPlayerAnimation->SendTrigger("tIdle", 0.2f);
                mPlayerAnimation->SendSpineTrigger("tIdle", 0.2f);

            }
            mIsExiting = false;
            mCurrentTime = 0.0f;
            mPlayer->GetComponent(ComponentType::SCRIPT)->SetEnable(true);
            mIsAtStart = !mIsAtStart;

        }
    }
}


void Teleporter::OnCollisionEnter(CollisionData* collisionData)
{
    if (!mGameObject->GetComponent(ComponentType::SCRIPT)->IsEnabled())
    {
        return;
    }
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
        if (mPlayer)
        {
            mPlayerAnimation = reinterpret_cast<AnimationComponent*>(mPlayer->GetComponent(ComponentType::ANIMATION));
            mPlayerAnimation->SendTrigger("tWalkForward", 0.2f);
            mPlayerAnimation->SendSpineTrigger("tWalkForward", 0.2f);
        }

        mFirstPlayerPos = mPlayer->GetWorldPosition();

        if (mIsAtStart)
        {
            mPlayer->LookAt(mStartPos);

            mDistance = mStartPos.Distance(mPlayer->GetWorldPosition());
            mCurrentDirection = mStartPos.Sub(mPlayer->GetWorldPosition()).Normalized();

        }
        else
        {
            mPlayer->LookAt(mEndPos);

            mDistance = mEndPos.Distance(mPlayer->GetWorldPosition());
            mCurrentDirection = mEndPos.Sub(mPlayer->GetWorldPosition()).Normalized();

        }

        mPlayer->GetComponent(ComponentType::SCRIPT)->SetEnable(false);

    }
}

float3 Teleporter::LerpPosition(float duration, float3 startPos)
{
    if (mCurrentTime < 0.0f)
    {
        return startPos;
    }
    else if (mCurrentTime > duration)
    {
        return startPos + mCurrentDirection * mDistance;
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

    return startPos + mCurrentDirection * lerpFactor;
}
