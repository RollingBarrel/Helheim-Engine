#pragma once
#include <Script.h>
#include "Macros.h"
#include "Math/float3.h"


GENERATE_BODY(Gun);
class Gun :public Script
{
	FRIEND(Gun)

public:
	Gun(GameObject* owner);

	void Shoot(bool isChargedShot, float chargeTime);

	void ShootLogic(int damage);

	void Reload();
	void Start() override;
	void Update() override;


private:

	//shooting variables
    int mDamage;
    int mMaxBullets;
    int mBullets;
	float mChargedShotMaxTime;
	float mBulletCostPerSecond;

	//reload variables
    float mReloadTime;

};

