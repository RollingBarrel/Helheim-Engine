#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"
#include "float4x4.h"
#include <vector>
#include <string>

class AnimationController;
class AnimationStateMachine;
class ResourceModel;


class ENGINE_API AnimationComponent : public Component {
public:
	AnimationComponent(GameObject* owner);
	AnimationComponent(const AnimationComponent& other, GameObject* owner);
	~AnimationComponent();

	void Reset() override {}
	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
	
	bool GetLoop() const { return mLoop; }
	void SetLoop(bool loop);

	bool GetIsPlaying() const { return mIsPlaying; }
	void SetIsPlaying(bool isPlaying) { mIsPlaying = isPlaying; }

	void OnStart();
	void OnStop();
	void OnRestart();

	AnimationStateMachine* GetStateMachine() const { return mStateMachine; }
	AnimationStateMachine* GetSpineStateMachine() const { return mSpineStateMachine; }

	//Speed
	float GetAnimSpeed() const { return mSpeed; }
	void SetAnimSpeed(float speed);

	std::string GetCurrentStateName();
	void SendTrigger(std::string trigger, float transitionTime);
	void ChangeState(std::string stateName, float transitionTime);
	
	std::string GetCurrentSpineStateName();
	void SendSpineTrigger(std::string trigger, float transitionTime);
	void ChangeSpineState(std::string stateName, float transitionTime);

	//Model UUID
	unsigned int GetModelUUID() const { return mModelUid; }
	void SetModelUUID(unsigned int modelUid);
	void SetModel(ResourceModel* model);

	

	void StartTransition(float transitionDuration);

private:

	void LoadSpine(ResourceModel* model);
	void LoadGameObjects(GameObject* current);
	void LoadSpineChildren(GameObject* current);

	AnimationController* mController;
	AnimationStateMachine* mStateMachine;
	int mCurrentState = 0;

	bool mLoop = true;
	bool mIsPlaying = false;

	float mSpeed;

	unsigned int mModelUid;

	//Locomotion
	AnimationController* mSpineController;
	AnimationStateMachine* mSpineStateMachine;
	int mCurrentSpineState = 0;
	bool mHasSpine = false;

	std::vector<GameObject*> mSpineObjects;
	std::vector<GameObject*> mDefaultObjects;

};

#endif
