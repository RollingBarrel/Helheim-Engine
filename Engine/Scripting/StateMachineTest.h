#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

class AnimationComponent;
class NavMeshController;
class GameManager;

enum class AnimationState 
{
	IDLE,
	WALK,
	DIE
};

GENERATE_BODY(StateMachineTest);
class StateMachineTest: public Script
{
	FRIEND(StateMachineTest)
	public:
		StateMachineTest(GameObject* owner);

		~StateMachineTest(){}

		void Start() override;
		void Update() override;

	private:
		void Idle();
		void Moving();
		void Die();

		AnimationState mCurrentState = AnimationState::IDLE;
		AnimationState mPreviousState = AnimationState::IDLE;

		NavMeshController* mNavMeshControl = nullptr;
		GameObject* mAnimationComponentHolder = nullptr;
		AnimationComponent* mAnimationComponent = nullptr;

		GameManager* mGameManager = nullptr;
		GameObject* mGameManagerGO = nullptr;

		float mPlayerSpeed = 2.0f;
};

