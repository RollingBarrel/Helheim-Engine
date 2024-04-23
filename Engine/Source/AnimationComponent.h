#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"

class AnimationController;
class ResourceAnimation;

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

	std::vector<std::pair<GameObject*, float4x4>> mGameobjectsInverseMatrices;

private:

	ResourceAnimation* mAnimation;
	AnimationController* mController;

	bool mLoop = true;
	bool mIsPlaying = false;

};

#endif
