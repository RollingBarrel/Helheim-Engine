#include "BossLaser.h"
#include "Application.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "MathFunc.h"
#include "State.h"
#include "Geometry/Ray.h"
#include "BossLaserEyeBall.h"

CREATE(BossLaser)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mDamage);
	MEMBER(MemberType::FLOAT, mRange);

	SEPARATOR("GAME OBJECTS");
	MEMBER(MemberType::GAMEOBJECT, mLaserOrigin);
	MEMBER(MemberType::GAMEOBJECT, mLaserTrail);
	MEMBER(MemberType::GAMEOBJECT, mLaserEnd);
	MEMBER(MemberType::GAMEOBJECT, mLaserCharge);

	END_CREATE;
}

BossLaser::BossLaser(GameObject* owner) : Script(owner)
{
}

BossLaser::~BossLaser()
{
}

void BossLaser::Start()
{

	ChangeState(LaserState::CHARGING);

}


void BossLaser::Update()
{
	float dt = App->GetDt();

	switch (mCurrentState) {
	case LaserState::CHARGING:
		UpdateCharging(dt);
		break;
	case LaserState::FIRING:
		UpdateFiring(dt);
		break;
	case LaserState::COOLDOWN:
		UpdateCooldown(dt);
		break;
	}

}

void BossLaser::Init(float damage, float range)
{
	mDamage = damage;
	mRange = range;
	ChangeState(LaserState::CHARGING);
}

void BossLaser::ChangeState(LaserState newState) {
	mCurrentState = newState;
	mStateTimer = 0.0f;

	switch (newState) {
	case LaserState::CHARGING:
		// Start charging VFX here
		if (mLaserCharge) {
			mLaserCharge->SetEnabled(true);
		}
		break;
	case LaserState::FIRING:
		// Start firing VFX and initialize laser parameters here
		if (mLaserCharge) {
			mLaserCharge->SetEnabled(false);
		}
		if (mLaserOrigin && mLaserTrail && mLaserEnd) {
			mLaserOrigin->SetEnabled(true);
			mLaserTrail->SetEnabled(true);
			mLaserEnd->SetEnabled(true);
		}
		break;
	case LaserState::COOLDOWN:
		// Disable laser VFX here
		if (mLaserOrigin && mLaserTrail && mLaserEnd) {
			mLaserOrigin->SetEnabled(false);
			mLaserTrail->SetEnabled(false);
			mLaserEnd->SetEnabled(false);
		}
		break;
	}
}

void BossLaser::UpdateCharging(float dt) {
	mStateTimer += dt;
	if (mStateTimer >= mChargeTime) {
		ChangeState(LaserState::FIRING);
	}
}

void BossLaser::UpdateFiring(float dt) {
	mStateTimer += dt;
	mMoveLaser(dt);
	mApplyDamage();
	if (mStateTimer >= mDuration) {
		ChangeState(LaserState::COOLDOWN);
	}
}

void BossLaser::UpdateCooldown(float dt) {
	mStateTimer += dt;
	if (mStateTimer >= mCooldownTime) {
		ChangeState(LaserState::CHARGING);
	}
}

void BossLaser::mMoveLaser(float dt) {
	mCurrentAngle += mDirection * mRotationSpeed * dt;
	if (mCurrentAngle >= 45.0f || mCurrentAngle <= -45.0f) {
		mDirection *= -1.0f;
	}
	mGameObject->SetLocalRotation(float3(0, DegToRad(mCurrentAngle), 0));
}

void BossLaser::mApplyDamage() {
	// Implement laser damage application logic here
	Ray ray;
	ray.pos = mLaserOrigin->GetWorldPosition();
	ray.pos.y++;
	ray.dir = mGameObject->GetFront();

	Hit hit;
	Physics::Raycast(hit, ray, mRange);
	if (hit.IsValid()) {
		if (hit.mGameObject->GetTag() == "Player") {
			ScriptComponent* playerScript = static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
			PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());
			if (player->GetPlayerLowerState()->GetType() != StateType::DASH) {
				player->TakeDamage(mDamage);
			}
		}
		mLaserEnd->SetWorldPosition(hit.mHitPoint);
	}
	else {
		float3 originPosition = mLaserOrigin->GetWorldPosition();
		mLaserEnd->SetWorldPosition(originPosition + mGameObject->GetFront() * mRange);
	}
}

void BossLaser::SpawnEyeballs() {
	const int numEyeballs = 5;
	float radius = 10.0f;

	for (int i = 0; i < numEyeballs; ++i) {
		float angle = (i * 360.0f / numEyeballs) * DegToRad(1.0f);
		float3 pos = float3(cos(angle) * radius, 0.0f, sin(angle) * radius);
		pos += mGameObject->GetWorldPosition();

		GameObject* eyeball = App->GetScene()->InstantiatePrefab("Eyeball");
		eyeball->SetWorldPosition(pos);

		// Initialize the eyeball (for example, set its script component)
		BossLaserEyeBall* eyeballScript = static_cast<BossLaserEyeBall*>(static_cast<ScriptComponent*>(eyeball->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		if (eyeballScript) {
			eyeballScript->Start(); // Initialize if necessary
		}
	}
}