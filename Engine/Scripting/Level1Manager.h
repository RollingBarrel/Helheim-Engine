#pragma once
#include "Script.h"
#include "Macros.h"

class AudioSourceComponent;
class PlayerController;

GENERATE_BODY(Level1Manager);

class Level1Manager :public Script
{

	FRIEND(Level1Manager)

public:
	Level1Manager(GameObject* owner);
	~Level1Manager();
	void Start() override;
	void Update() override;

private:
	void UpdateBackgroundMusic();
	void UpdateEnemyFootStepMusic();
	void UpdateBackgroundStrangeMusic();

	GameObject* mPlayerControllerHolder = nullptr;
	PlayerController* mPlayerController = nullptr;

	GameObject* mLevel1MainThemeHolder = nullptr;
	AudioSourceComponent* mLevel1MainTheme = nullptr;

	GameObject* mEnemyFootStepHolder = nullptr;
	AudioSourceComponent* mEnemyFootStep = nullptr;

	GameObject* mStangeBackgroudSoundHolder = nullptr;
	AudioSourceComponent* mStrangeBackgroundSound = nullptr;

	bool mReadyToStep = false;
	float mStepTimePassed = 0.0f;
	float mStepCoolDown = 0.5f;
};

