#pragma once
#include "Script.h"
#include "Macros.h"

GENERATE_BODY(AudioTester);

class AudioTester: public Script
{
	FRIEND(AudioTester)

public:
	AudioTester(GameObject* owner);
	~AudioTester();

	void Start();
	void Update();

private:
	void ControlBGM();
	void ControlSFX();
	void WalkingSimulation();
	void BGM1ParameterChange();
	
	int mBGM1ID = -1;
	bool mBGM1Play = false;

	int mBGM2ID = -1;
	bool mBGM2Play = false;

	int mBGM3ID = -1;
	bool mBGM3Play = false;
	
	int mSFX4sID = -1;
	bool mSFX4Play = false;

	// Footstep
	bool mIsMoving = false;
	bool mReadyToStep = false;
	float mStepTimePassed = 0.0f;
	float mStepCoolDown = 0.5f;
};

