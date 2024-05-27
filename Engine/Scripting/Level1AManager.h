#pragma once
#include "Script.h"
#include "Macros.h"

class AudioSourceComponent;
class PlayerController;

GENERATE_BODY(Level1AManager);

class Level1AManager :public Script
{
	// Level 1 ABOVE

	FRIEND(Level1AManager)

public:
	Level1AManager(GameObject* owner);
	~Level1AManager();
	void Start() override;
	void Update() override;

private:
	void UpdateBackgroundMusic();
	void UpdateEnemyFootStepMusic();
	void UpdateBackgroundStrangeMusic();

	GameObject* mPlayerControllerHolder = nullptr;
	PlayerController* mPlayerController = nullptr;

	GameObject* mLevel1AMainThemeHolder = nullptr;
	AudioSourceComponent* mLevel1AMainTheme = nullptr;

	GameObject* mLevel1AMainDeadHolder = nullptr;
	AudioSourceComponent* mLevel1ADeadTheme = nullptr;

	GameObject* mEnemyFootStepHolder = nullptr;
	AudioSourceComponent* mEnemyFootStep = nullptr;

	GameObject* mStangeBackgroudSoundHolder = nullptr;
	AudioSourceComponent* mStrangeBackgroundSound = nullptr;

	bool mReadyToStep = false;
	float mStepTimePassed = 0.0f;
	float mStepCoolDown = 0.5f;
};

