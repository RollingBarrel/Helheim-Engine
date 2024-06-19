#pragma once
#include "MeleeWeapon.h"
#include "Script.h"
#include "Macros.h"

class TrailComponent;
class GameObject;

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

    float mRange = 1.0f;

    GameObject* mPlayerGO = nullptr;



};
