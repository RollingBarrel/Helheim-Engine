#pragma once
#include "Script.h"
#include "Macros.h"

GENERATE_BODY(FirstTutorial);

class GameObject;

class FirstTutorial : public Script 
{
	FRIEND(FirstTutorial)
public:
	FirstTutorial(GameObject* owner);
	~FirstTutorial();

	void Start() override;
	void Update() override;

	void Tutorial();

private:
	//KEYBOARD
	GameObject* mMoveTutorial = nullptr;
	GameObject* mDashTutorial = nullptr;
	//CONTROLLER
	GameObject* mMoveTutorialCon = nullptr;
	GameObject* mDashTutorialCon = nullptr;

	GameObject* mDoor1 = nullptr;

	int mCurrentStep = 0;

	bool mCompleted = false;
	bool mActiveCon = false;
	bool mController = false;
	bool mKeyBoard = true;
};

