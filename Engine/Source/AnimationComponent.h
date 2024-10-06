#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"
#include "float4x4.h"
#include <vector>
#include <string>

class AnimationController;
class AnimationStateMachine;


class ENGINE_API AnimationComponent : public Component {
public:
	AnimationComponent(GameObject* owner);
	AnimationComponent(const AnimationComponent& other, GameObject* owner);
	~AnimationComponent();

	void Reset() override {}
	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

	bool GetLoop() const { return mLoop; }
	void SetLoop(bool loop);

	bool GetIsPlaying() const { return mIsPlaying; }
	void SetIsPlaying(bool isPlaying) { mIsPlaying = isPlaying; }

	void StartUp();
	void OnStop();
	void RestartStateAnimation();
	void ResetAnimationComponent();

	const std::vector<std::string> GetSMStateNames() const;
	const std::vector<std::string> GetSpineSMStateNames() const;

	AnimationStateMachine* GetStateMachine() const { return mStateMachine; }
	AnimationStateMachine* GetSpineStateMachine() const { return mSpineStateMachine; }

	void SetStateMachine(AnimationStateMachine* sm);
	void SetSpineStateMachine(AnimationStateMachine* sm);

	//Speed
	float GetAnimSpeed() const { return mSpeed; }
	void SetUpperAnimSpeed(float speed);
	void SetLowerAnimSpeed(float speed);
	void SetAnimSpeed(float speed);

	std::string GetCurrentStateName();
	void SendTrigger(const std::string& trigger, float transitionTime);
	void ChangeState(const std::string& stateName, float transitionTime);
	bool HasCurrentStateFinished(float deltaTime = 0.01f);
	
	std::string GetCurrentSpineStateName();
	void SendSpineTrigger(const std::string& trigger, float transitionTime);
	void ChangeSpineState(const std::string& stateName, float transitionTime);
	bool HasCurrentSpineStateFinished(float deltaTime = 0.01f);


	//Animations UUIDs
	void SetAnimationUid(const unsigned int uid) { mAnimationUID = uid; }
	const unsigned int GetAnimationUid() const { return mAnimationUID; }

	bool HasSpine() const { return mHasSpine; }

	void StartTransition(float transitionDuration);

	void SetSpineSMUID(unsigned int uid);
	void SetSMUID(unsigned int uid);

	void ReloadGameObjects();


	void SetControllerTime(float time);
	float GetControllerTime() const;

	void SetSpineControllerTime(float time);
	float GetSpineControllerTime() const;
	void SetObjects(const std::unordered_map<const GameObject*, GameObject*>& originalToNew);

private:

	void LoadGameObjects(GameObject* current);
	void LoadSpineChildren(GameObject* current);
	void OnResetGameObjects();

	AnimationController* mController;
	AnimationStateMachine* mStateMachine;
	int mCurrentState = 0;

	bool mLoop = true;
	bool mIsPlaying = false;

	float mSpeed;


	//Locomotion
	AnimationController* mSpineController;
	AnimationStateMachine* mSpineStateMachine;
	int mCurrentSpineState = 0;
	bool mHasSpine = false;

	std::vector<GameObject*> mSpineObjects;
	std::vector<GameObject*> mDefaultObjects;
	unsigned int mAnimationUID;

};

#endif
