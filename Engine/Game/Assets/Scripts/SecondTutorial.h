#pragma once
#include "Script.h"
#include "Macros.h"

GENERATE_BODY(SecondTutorial);

class GameObject;

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
	GameObject* mSecondaryTutorial = nullptr;
	GameObject* mGrenadeTutorial = nullptr;
	GameObject* mUltimateTutorial = nullptr;
	//CONTROLLER
	GameObject* mSecondaryTutorialCon = nullptr;
	GameObject* mGrenadeTutorialCon = nullptr;
	GameObject* mUltimateTutorialCon = nullptr;

	int mCurrentStep = 0;

	bool mCompleted = false;
	bool mActiveCon = false;
	bool mController = false;
	bool mKeyBoard = true;
};

