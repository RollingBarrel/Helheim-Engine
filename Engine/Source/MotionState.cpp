#include "MotionState.h"
#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScriptManager.h"

#include "Math/float4x4.h"

MotionState::MotionState(Component* componentCollider, float3 centerOffset, bool freezeRotation) :
	mCollider(componentCollider), mCenterOffset(centerOffset), mFreezeRotation(freezeRotation)
{
	mMassCenterOffset = btTransform(btMatrix3x3::getIdentity(), btVector3(centerOffset.x, centerOffset.y, centerOffset.z)).inverse();
}

MotionState::~MotionState()
{
}

void MotionState::getWorldTransform(btTransform& worldTransform) const
{
	float3 position = mCollider->GetOwner()->GetWorldPosition();
	Quat rotation = mCollider->GetOwner()->GetRotationQuat();

	worldTransform = btTransform(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w), btVector3(position.x, position.y, position.z)) * mMassCenterOffset.inverse();
}

void MotionState::setWorldTransform(const btTransform& worldTransform)
{
	if (App->GetScriptManager()->IsPlaying())
	{
		btTransform mTransform = worldTransform * (mFreezeRotation ? btTransform::getIdentity() : mMassCenterOffset);
		float3 parentScale = mCollider->GetOwner()->GetParent()->GetScale();
		float3 parentPosition = mCollider->GetOwner()->GetParent()->GetPosition();
		Quat parentRotation = mCollider->GetOwner()->GetParent()->GetRotationQuat().Inverted();

		// Set Local Position
		float3 position = (float3)(mTransform.getOrigin() + (mFreezeRotation ? mMassCenterOffset.getOrigin() : btVector3(0, 0, 0)));
		mCollider->GetOwner()->SetPosition(parentRotation.Transform(((position).Div(parentScale) - parentPosition)));

		// Set Local Rotation
		if (!mFreezeRotation) {
			btQuaternion rotation;
			mTransform.getBasis().getRotation(rotation);
			mCollider->GetOwner()->SetRotation(parentRotation * (Quat)rotation);
		}
	}
}
