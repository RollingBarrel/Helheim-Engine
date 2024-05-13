#pragma once

#include "LinearMath/btMotionState.h"
#include "LinearMath/btTransform.h"
#include "Math/float3.h"

class Component;

class MotionState : public btMotionState
{
public:
	MotionState(Component* componentCollider, float3 centerOffset, bool freezeRotation);
	~MotionState();

	void getWorldTransform(btTransform& worldTransform) const override;
	void setWorldTransform(const btTransform& worldTransform) override;

private:
	Component* mCollider = nullptr;
	btTransform mMassCenterOffset = btTransform::getIdentity();
	float3 mCenterOffset = float3::zero;
	bool mFreezeRotation = false;
};

