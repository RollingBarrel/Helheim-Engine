	#pragma once

class Weapon
{
public:
	enum class WeaponType {
		RANGE,
		MELEE
	};

	Weapon();
	~Weapon();

	virtual void Enter() = 0;
	virtual void Attack(float time) = 0;
	virtual void Exit() = 0;

	WeaponType GetType() { return mType; }
	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }
	virtual float GetAttackTime() = 0;
	void SetCurrentAmmo(int value) { mCurrentAmmo = value; }

protected:
	WeaponType mType;

	float mDamage = 0.0f;
	//float mAttackRate = 0.0f;
	float mAttackTime = 0.0f;
	float mAttackRange = 0.0f;

	int mCurrentAmmo = 0;
	int mMaxAmmo = 0;
};

