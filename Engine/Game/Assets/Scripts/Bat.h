#pragma once
#include "MeleeWeapon.h"
#include "Script.h"
#include "Macros.h"

class TrailComponent;

enum class WeaponUpgrade {
    NONE,
    RED,
    BLUE
};

class Bat : public MeleeWeapon
{
public:
	Bat();
	~Bat();

	void Enter() override;
    void Update(float deltaTime);
    void DealDamage(GameObject* enemy);
	void Attack() override;
	void Exit() override;


private:

	TrailComponent* mTrail = nullptr;

    float mEnergyCost = 10.0f;       
    float mCooldownModifier = 1.f;

    float mComboTimer = 0.0f;
    float mComboCurrentTime = 0.0f;
    int   mComboStep = 0;
    bool  mIsComboActive = false;
    bool  mIsNextComboStep = false;
    float mBreakCombo = 0.0f;

    float mComboMilestone1 = 1.0f;
    float mComboMilestone2 = 2.0f;
    float mComboDuration = 3.0f;

    // Player Dash
    float  mMoveRange = 15.f;
    float  mMoveDuration = 4.0f;



};
