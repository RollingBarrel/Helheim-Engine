#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(RotateObject);

class RotateObject : public Script
{
	FRIEND(RotateObject)

public:
	RotateObject(GameObject* owner);

	void Update() override;
	void Start() override;

private:
	float mSpeed = 1;
	float3 mDirection = float3::zero;

};


