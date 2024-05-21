#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

class AnimationComponent;
class NavMeshController;
class GameManager;
class AnimationStateMachine;


enum class AnimationStates
{
	IDLE,
	WALK,
	DIE
};

GENERATE_BODY(StateMachineTest);
class StateMachineTest : public Script
{
	FRIEND(StateMachineTest)
public:
	StateMachineTest(GameObject* owner);

	~StateMachineTest() {}

	void Start() override;
	void Update() override;

private:
	void Idle();
	void Moving();
	void Die();

	AnimationStates mCurrentState = AnimationStates::IDLE;
	AnimationStates mPreviousState = AnimationStates::IDLE;

	NavMeshController* mNavMeshControl = nullptr;
	AnimationComponent* mAnimationComponent = nullptr;
	AnimationStateMachine* mStateMachine = nullptr;

	GameManager* mGameManager = nullptr;
	GameObject* mGameManagerGO = nullptr;

	float mPlayerSpeed = 2.0f;
};