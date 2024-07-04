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
	int GetDamage() { return mDamage; }
	virtual float GetAttackDuration() = 0;
	float GetAttackCooldown() { return mAttackCooldown; }
	
	void SetCurrentAmmo(int value) { mCurrentAmmo = value; }
	void SetDamage(int value) { mDamage = value; }
protected:
	virtual void PlayHitSound() = 0;
	WeaponType mType;

	float mDamage = 0.0f;
	float mAttackRange = 0.0f;

	float mAttackCooldown = 0.0f;
	float mAttackDuration = 0.0f;

	int mCurrentAmmo = 0;
	int mMaxAmmo = 0;
};

