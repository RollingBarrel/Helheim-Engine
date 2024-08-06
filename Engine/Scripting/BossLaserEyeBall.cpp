#include "BossLaserEyeBall.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include <MathFunc.h>


BossLaserEyeBall::BossLaserEyeBall(GameObject* owner) : Script(owner)
{
}

void BossLaserEyeBall::Start()
{
    mElapsedTime = 0.0f;
}

void BossLaserEyeBall::Update()
{
    float deltaTime = App->GetDt();
    mElapsedTime += deltaTime;

    if (mElapsedTime >= mDuration)
    {
        mGameObject->SetEnabled(false); // or Destroy(mGameObject)
    }
    else
    {
        RotateLaser();
    }
}

void BossLaserEyeBall::Init(float damage, float distance, float duration, float rotationSpeed)
{
    mDamage = damage;
    mDistance = distance;
    mDuration = duration;
    mRotationSpeed = rotationSpeed;
    mCurrentRotation = 0.0f;
    mRotatingRight = true;
    mElapsedTime = 0.0f;
}

void BossLaserEyeBall::RotateLaser()
{
    float deltaTime = App->GetDt();
    float rotationAmount = mRotationSpeed * deltaTime;

    if (mRotatingRight)
    {
        mCurrentRotation += rotationAmount;
        if (mCurrentRotation >= 45.0f)
        {
            mRotatingRight = false;
        }
    }
    else
    {
        mCurrentRotation -= rotationAmount;
        if (mCurrentRotation <= -45.0f)
        {
            mRotatingRight = true;
        }
    }

    // Rotate the laser visually here based on mCurrentRotation
    // This is where you'd update the laser's direction in your game engine
}