#pragma once
class PlayerStats
{
public:

	float GetMaxHealth() { return mMaxHealth; }
	float GetSpeed() { return mSpeed; }
	float GetDamageModifier() { return mDamageModifier; }
	int GetDialogIndex() { return mDialogIndex; }

	void SetMaxHealth(float maxHealth) { mMaxHealth = maxHealth; }
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetDamageModifier(float damageModifier) { mDamageModifier = damageModifier; }
	void SetDialogIndex(int index) { mDialogIndex = index; }

	void SetInitLevelStats() {
		mIniHealth = mMaxHealth;
		mIniSpeed = mSpeed;
		mIniDamage = mDamageModifier;
		mIniDialogIdx = mDialogIndex;
	}

	void TryAgainStats() {
		mMaxHealth = mIniHealth;
		mSpeed = mIniSpeed;
		mDamageModifier = mIniDamage;
		mDialogIndex = mIniDialogIdx;
	}

	void ResetStats() {
		mMaxHealth = 100.0f;
		mSpeed = 5.5f;
		mDamageModifier = 1.0f;
		mDialogIndex = 0;
	}

private:
	float mMaxHealth = 100.0f;
	float mSpeed = 5.5f;
	float mDamageModifier = 1.0f;
	int mDialogIndex = 0;

	float mIniHealth = 0.0f;
	float mIniSpeed = 0.0f;
	float mIniDamage = 0.0f;
	int mIniDialogIdx = 0;
};

