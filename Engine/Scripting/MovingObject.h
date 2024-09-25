#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(MovingObject);

class MovingObject : public Script
{
	FRIEND(MovingObject)

public:
	MovingObject(GameObject* owner);

	void Update() override;
	void Start() override;

private:
	float mSpeed = 1;
	float3 mDirection = float3(1.0f,0.0f,0.0f);
	bool mLoopsPath = false;
	float mDistance = 20.0f;
	
	float3 mStartingPoint = float3::zero;
	float mCurrentDistance = 0.0f;
	bool mGoingForward = true;
};



