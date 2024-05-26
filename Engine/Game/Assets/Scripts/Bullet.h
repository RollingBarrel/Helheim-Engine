#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

GENERATE_BODY(Bullet);

class Bullet : public Script
{
	FRIEND(Bullet)
public:
	Bullet(GameObject* owner);
	~Bullet();
	void Update() override;

	static unsigned int GetNumBullets() { return mNumBullets; }

private: 

	static unsigned int mNumBullets;

	float mRange = 15.0f;
	float mSpeed = 1.0f;

	float mTotalMovement = 0.0f;

};
