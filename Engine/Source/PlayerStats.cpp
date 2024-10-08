#include "PlayerStats.h"

void PlayerStats::SetInitLevelStats() 
{
	mIniHealth = mMaxHealth;
	mIniSpeed = mSpeed;
	mIniDamage = mDamageModifier;
	mIniDialogIdx = mDialogIndex;
}

void PlayerStats::TryAgainStats() 
{
	mMaxHealth = mIniHealth;
	mSpeed = mIniSpeed;
	mDamageModifier = mIniDamage;
	mDialogIndex = mIniDialogIdx;
}

void PlayerStats::ResetStats() 
{
	mMaxHealth = 100.0f;
	mSpeed = 5.5f;
	mDamageModifier = 1.0f;
	mDialogIndex = 0;

	int mHealthLvl = 0;
	int mSpeedLvl = 0;
	int mDamageLvl = 0;

	mGameFinished = false;
}
