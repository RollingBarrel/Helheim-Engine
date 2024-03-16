#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"

class AnimationController;
class ResourceAnimation;

class AnimationComponent : public Component {
public:
	AnimationComponent(GameObject* owner);
	AnimationComponent(const AnimationComponent& other, GameObject* owner);
	~AnimationComponent();

	void Reset() override {}
	void Update() override; 
	Component* Clone(GameObject* owner) const override;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	const ResourceAnimation* GetAnimation() { return mAnimation; }
	const AnimationController* GetAnimationController() { return mController; }

	void CreateNodeTree(); 
	void DeleteNodeTree();

	void OnStart();
	void OnStop();
	void OnUpdate();

	void SetAnimation(unsigned int uid);

private:

	ResourceAnimation* mAnimation;
	AnimationController* mController;
};

#endif
