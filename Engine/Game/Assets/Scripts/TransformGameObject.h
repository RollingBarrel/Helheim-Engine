#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(TransformGameObject);

class TransformGameObject : public Script
{
	FRIEND(TransformGameObject)

public:
	TransformGameObject(GameObject* owner);

	void Update() override;
	void Start() override;

private:
	bool mTransformPosition = false;
	bool mTransformRotation = false;
	bool mTransformScale = false;
	float3 mPosition = float3::zero;
	float3 mRotation = float3::zero;
	float3 mScale = float3::zero;
	float3 mPositionOffset;
	float3 mRotationOffset;
	float3 mScaleOffset;
	float mDuration = 0;
	float mTime = 0;
	float mEllapsedTime = 0;
};

