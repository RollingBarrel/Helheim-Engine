#pragma once
#include "Script.h"
#include "Macros.h"

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

private:
	//KEYBOARD
	GameObject* mShootTutorial = nullptr;
	GameObject* mSecondaryTutorial = nullptr;
	GameObject* mGrenadeTutorial = nullptr;
	GameObject* mUltimateTutorial = nullptr;
	//CONTROLLER
	GameObject* mShootTutorialCon = nullptr;
	GameObject* mSecondaryTutorialCon = nullptr;
	GameObject* mGrenadeTutorialCon = nullptr;
	GameObject* mUltimateTutorialCon = nullptr;

	GameObject* mTutorialAreaGO = nullptr;
	BattleArea* mTutorialArea = nullptr;

	int mCurrentStep = 0;

	bool mCompleted = false;
	bool mActiveCon = false;
	bool mController = false;
	bool mKeyBoard = true;
};

