#include "PlayerStats.h"

void PlayerStats::SetInitLevelStats() 
{
	mIniHealth = mMaxHealth;
	mIniSpeed = mSpeed;
	mIniDamage = mDamageModifier;
	mIniDialogIdx = mDialogIndex;

	mInitHealthLvl = mHealthLvl;
	mInitSpeedLvl = mSpeedLvl;
	mInitDamageLvl = mDamageLvl;
}

void PlayerStats::TryAgainStats() 
{
	mMaxHealth = mIniHealth;
	mSpeed = mIniSpeed;
	mDamageModifier = mIniDamage;
	mDialogIndex = mIniDialogIdx;

	mHealthLvl = mInitHealthLvl;
	mSpeedLvl = mInitSpeedLvl;
	mDamageLvl = mInitDamageLvl;
}

void PlayerStats::ResetStats() 
{
	mMaxHealth = 100.0f;
	mSpeed = 5.5f;
	mDamageModifier = 1.0f;
	mDialogIndex = 0;

	mHealthLvl = 0;
	mSpeedLvl = 0;
	mDamageLvl = 0;

	mGameFinished = false;
}
