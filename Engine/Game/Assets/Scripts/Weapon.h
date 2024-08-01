	#pragma once

class Weapon
{
public:
	enum class WeaponType 
	{
		RANGE,
		MELEE,
		NONE
	};

	Weapon();
	~Weapon();

	virtual void Enter() {}
	virtual void Attack(float time) = 0;
	virtual void Exit() {}

	WeaponType GetType() { return mType; }
	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }
	float GetDamage() { return mDamage; }
	virtual float GetAttackDuration() = 0;
	float GetAttackCooldown() { return mAttackCooldown; }
	
	void SetCurrentAmmo(int value) { mCurrentAmmo = value; }
	void SetDamage(float value) { mDamage = value; }
protected:
	virtual void PlayHitSound() = 0;
	WeaponType mType = WeaponType::NONE;

	float mDamage = 0.0f;
	float mAttackRange = 0.0f;

	float mAttackCooldown = 0.0f;
	float mAttackDuration = 0.0f;

	int mCurrentAmmo = 0;
	int mMaxAmmo = 0;
};

