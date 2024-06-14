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

	virtual void BasicAttack() = 0;
	virtual void SpecialAttack() = 0;

	WeaponType GetType() { return mType; }
	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }
	int GetDamage() { return mDamage; }

	void SetCurrentAmmo(int value) { mCurrentAmmo = value; }
	void SetDamage(int value) { mDamage = value; }
protected:
	WeaponType mType;

	int mDamage = 0.0f;
	float mAttackRate = 0.0f;

	int mCurrentAmmo = 0;
	int mMaxAmmo = 0;
};

