#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

class ObjectPool;

GENERATE_BODY(Bullet);

class Bullet : public Script
{
	FRIEND(Bullet)
public:
	Bullet(GameObject* owner);
	~Bullet() {}
	void Start() override;
	void Update() override;

	//object pool for bullets
	void SetRange(float range) { mRange = range; }

	ObjectPool* objectPool = nullptr;
	float mTimePassed = 0.0f;

private: 
	void Move();
	bool Delay(float delay);


	float mRange = 15.0f;
	float mSpeed = 1.0f;
	bool mActive = false;
	float3 mStartPosition; //to check the range of the bullet

};
