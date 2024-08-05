#include "BossLaserEyeBall.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Transform2DComponent.h"
#include "GameObject.h" 

#include <MathFunc.h>


BossLaserEyeBall::BossLaserEyeBall(GameObject* owner)
    : Script(owner), mCurrentAngle(0.0f), mRotationDirection(1.0f), mRotationSpeed(2.0f), mIsActive(false) {
}

void BossLaserEyeBall::Start() {
    GameObject* bombTemplate = App->GetScene()->InstantiatePrefab("Enemy_Ball", mGameObject);
}

void BossLaserEyeBall::Update() {
    if (mIsActive) {
        RotateLaser();
    }
}

void BossLaserEyeBall::SetPhase(bool isActive) {
    mIsActive = isActive;
    if (!mIsActive) {
        // Optionally stop or hide lasers or other effects
    }
}

void BossLaserEyeBall::RotateLaser() {
    mCurrentAngle += mRotationDirection * mRotationSpeed * App->GetDt(); // Use delta time for frame rate independence

    if (mCurrentAngle >= 45.0f || mCurrentAngle <= -45.0f) {
        mRotationDirection *= -1.0f; // Reverse direction when limits are reached
    }

    mGameObject->SetLocalRotation(float3(0, DegToRad(mCurrentAngle), 0));
}