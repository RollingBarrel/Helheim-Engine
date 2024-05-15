#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"
#include "float4x4.h"
#include <vector>
#include <string>

class AnimationController;
class AnimationStateMachine;
class ResourceAnimation;
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

	//Animation Resources
	//std::vector<ResourceAnimation*> GetAnimations() { return mAnimations; }
	ResourceAnimation* GetAnimation() { return mAnimation; }

	void ChangeAnimation(ResourceAnimation* animation);
	
	AnimationController* GetAnimationController() const { return mController; }

	bool GetLoop() const { return mLoop; }
	void SetLoop(bool loop);

	bool GetIsPlaying() const { return mIsPlaying; }
	void SetIsPlaying(bool isPlaying) { mIsPlaying = isPlaying; }

	void OnStart();
	void OnStop();
	void OnRestart();

	void SetAnimation(unsigned int uid);
	AnimationStateMachine* GetStateMachine() const { return mStateMachine;};
	
	//Pallete calculations
	const std::vector<float4x4> GetPalette() const { return mPalette; }

	void LoadAllChildJoints(GameObject* currentObject, ResourceModel* model);

	std::vector<std::pair<GameObject*, float4x4>> mGameobjectsInverseMatrices;

	//Speed
	float GetAnimSpeed() const { return mSpeed; }
	void SetAnimSpeed(float speed);

	std::string GetCurrentStateName();
	void SendTrigger(std::string trigger);
	void ChangeState(std::string stateName);
	//Model UUID
	unsigned int GetModelUUID() const { return mModelUid; }
	void SetModelUUID(unsigned int modelUid); 

	

	void StartTransition(float transitionDuration);

private:
	void AddJointNode(GameObject* node, ResourceModel* model);
	void UpdatePalette();

	//std::vector<ResourceAnimation*> mAnimations;
	ResourceAnimation* mAnimation;

	AnimationController* mController;
	AnimationStateMachine* mStateMachine;
	int mCurrentState = 0;

	bool mLoop = true;
	bool mIsPlaying = false;

	std::vector<float4x4> mPalette;

	float mSpeed;

	unsigned int mModelUid;
	
};

#endif
