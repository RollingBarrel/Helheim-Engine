#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

GENERATE_BODY(SecondTutorial);

class GameObject;
class BattleArea;

class SecondTutorial : public Script
{
	FRIEND(SecondTutorial)
public:
	SecondTutorial(GameObject* owner);
	~SecondTutorial();

	void Start() override;
	void Update() override;

	void Tutorial();
	void UltTutorial();
	void DisableFirstPart();

private:
	//KEYBOARD
	GameObject* mShootTutorial = nullptr;
	GameObject* mSecondaryTutorial = nullptr;
	GameObject* mGrenadeTutorial = nullptr;
	GameObject* mUltimateTutorial = nullptr;
	GameObject* mCollectibleTut = nullptr;
	GameObject* mSkipTutorial = nullptr;
	//CONTROLLER
	GameObject* mShootTutorialCon = nullptr;
	GameObject* mSecondaryTutorialCon = nullptr;
	GameObject* mGrenadeTutorialCon = nullptr;
	GameObject* mUltimateTutorialCon = nullptr;
	GameObject* mCollectibleTutCon = nullptr;
	GameObject* mSkipTutorialCon = nullptr;

	TimerScript mTutorialTimer;

	GameObject* mTutorialAreaGO = nullptr;
	BattleArea* mTutorialArea = nullptr;

	int mCurrentStep = 0;

	bool mPart1Completed = false;\
	bool mPart2Completed = false;
	bool mActiveCon = false;
	bool mController = false;
	bool mKeyBoard = true;

	bool mSecondaryUsed = false;
	bool mGrenadeUsed = false;
	bool mUltimateUsed = false;

	bool mUltTutorialStarted = false;
};

