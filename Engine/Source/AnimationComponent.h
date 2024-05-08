#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"
#include "float4x4.h"
#include <vector>

class AnimationController;
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

	const ResourceAnimation* GetAnimation() const { return mAnimation; }
	AnimationController* GetAnimationController() const { return mController; }

	bool GetLoop() const { return mLoop; }
	void SetLoop(bool loop);

	bool GetIsPlaying() const { return mIsPlaying; }
	void SetIsPlaying(bool isPlaying) { mIsPlaying = isPlaying; }

	void OnStart();
	void OnStop();
	void OnRestart();

	void SetAnimation(unsigned int uid);

	void SetStartTime(float time);
	void SetEndTime(float time);
	const std::vector<float4x4> GetPalette() const { return mPalette; }


	void LoadAllChildJoints(GameObject* currentObject, ResourceModel* model);


	std::vector<std::pair<GameObject*, float4x4>> mGameobjectsInverseMatrices;


	const std::vector<const char*>& GetClipNames() const { return mClipNames; }

	void SetClipNames(const std::vector<const char*>& clipNames) { mClipNames = clipNames; }

	const std::vector<float>& GetClipTimes() const { return mClipTimes; }

	void SetClipTimes(const std::vector<float>& clipTimes) { mClipTimes = clipTimes; }

	int GetCurrentClip() const { return mCurrentClip; }

	void SetCurrentClip(int currentClip);
	float GetCurrentStartTime() const { return mClipTimes[mCurrentClip * 2]; }
	float GetCurrentEndTime() const { return mClipTimes[mCurrentClip * 2 + 1]; }

	unsigned int GetModelUUID() const { return mModelUid; }
	void SetModelUUID(unsigned int modelUid) { mModelUid = modelUid; }

	float GetAnimSpeed() const { return mSpeed; }
	void SetAnimSpeed(float speed);

	void StartTransition(float transitionDuration);

private:
	void AddJointNode(GameObject* node, ResourceModel* model);
	void UpdatePalette();
	//This will be a vector containing all the ResourceAnimations of the model
	ResourceAnimation* mAnimation;
	ResourceAnimation* mNextAnimation;

	AnimationController* mController;

	bool mLoop = true;
	bool mIsPlaying = false;
	std::vector<float4x4> mPalette;
	std::vector<const char*> mClipNames;
	std::vector<float> mClipTimes;
	int mCurrentClip;
	unsigned int mModelUid;
	float mSpeed;


};

#endif
