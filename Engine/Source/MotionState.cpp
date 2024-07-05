#include "MotionState.h"
#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScriptManager.h"
#include "BoxColliderComponent.h"

#include "Math/float4x4.h"

MotionState::MotionState(Component* componentCollider, float3 centerOffset, bool freezeRotation) :
	mCollider(componentCollider)
{
	mMassCenterOffset = btTransform(btMatrix3x3::getIdentity(), btVector3(centerOffset.x, centerOffset.y, centerOffset.z)).inverse();
}

MotionState::~MotionState()
{
}

void MotionState::getWorldTransform(btTransform& worldTransform) const
{
	float3 position = mCollider->GetOwner()->GetWorldPosition();
	Quat rotation = mCollider->GetOwner()->GetWorldRotation();

	worldTransform = btTransform(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w), btVector3(position.x, position.y, position.z)) * mMassCenterOffset.inverse();
}

void MotionState::setWorldTransform(const btTransform& worldTransform)
{
	if (false) // TODO: Check game is running App->GetScriptManager()->IsPlaying()
	{
		// TODO: generalize collider common properties on super class
		bool freezeRotation = static_cast<BoxColliderComponent*>(mCollider)->GetFreezeRotation();
		
		btTransform transform = worldTransform * (freezeRotation ? btTransform::getIdentity() : mMassCenterOffset);
		float3 parentScale = mCollider->GetOwner()->GetParent()->GetWorldScale();
		float3 parentPosition = mCollider->GetOwner()->GetParent()->GetWorldPosition();
		Quat parentRotation = mCollider->GetOwner()->GetParent()->GetWorldRotation().Inverted();

		// Set Local Position
		float3 position = (float3)(transform.getOrigin() + (freezeRotation ? mMassCenterOffset.getOrigin() : btVector3(0, 0, 0)));
		mCollider->GetOwner()->SetLocalPosition(parentRotation.Transform(((position).Div(parentScale) - parentPosition)));

		// Set Local Rotation
		if (!freezeRotation) {
			btQuaternion rotation;
			transform.getBasis().getRotation(rotation);
			mCollider->GetOwner()->SetLocalRotation(parentRotation * (Quat)rotation);
		}
	}
}

void MotionState::SetCenterOffset(float3 centerOffset)
{
	mMassCenterOffset = btTransform(btMatrix3x3::getIdentity(), btVector3(centerOffset.x, centerOffset.y, centerOffset.z)).inverse();
}
