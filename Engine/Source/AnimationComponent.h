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
	const AnimationController* GetAnimationController() const { return mController; }

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


	// Getter for mClipNames
	const std::vector<const char*>& getClipNames() const {
		return mClipNames;
	}

	// Setter for mClipNames
	void setClipNames(const std::vector<const char*>& clipNames) {
		mClipNames = clipNames;
	}

	// Getter for mClipTimes
	const std::vector<float>& getClipTimes() const {
		return mClipTimes;
	}

	// Setter for mClipTimes
	void setClipTimes(const std::vector<float>& clipTimes) {
		mClipTimes = clipTimes;
	}

	// Getter for mCurrentClip
	int getCurrentClip() const {
		return mCurrentClip;
	}

	// Setter for mCurrentClip
	void setCurrentClip(int currentClip) {
		if (currentClip > mClipNames.size())
		{
			currentClip = 0;
		}
		mCurrentClip = currentClip;
		SetStartTime(mClipTimes[mCurrentClip * 2]);
		SetEndTime(mClipTimes[mCurrentClip * 2 + 1]);

	}
	float getCurrentStartTime() const
	{
		return mClipTimes[mCurrentClip * 2];
	}
	float getCurrentEndTime() const
	{
		return mClipTimes[mCurrentClip * 2 + 1];
	}

	unsigned int getModelUUID() const { return mModelUid; }
	void setModelUUID(unsigned int modelUid) { mModelUid = modelUid; }

	float GetAnimSpeed() const { return mSpeed; }
	void SetAnimSpeed(float speed);


private:
	void AddJointNode(GameObject* node, ResourceModel* model);
	void UpdatePalette();
	ResourceAnimation* mAnimation;
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
