#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"

class AnimationController;
class ResourceAnimation;

class ENGINE_API AnimationComponent : public Component {
public:
	AnimationComponent(GameObject* owner, unsigned int animationUid);
	AnimationComponent(const AnimationComponent& other, GameObject* owner);
	~AnimationComponent();

	const ResourceAnimation* GetAnimation() { return mAnimation; }
	const AnimationController* GetAnimationController() { return mController; }

	void CreateNodeTree(); 
	void DeleteNodeTree();

	void OnStart();
	void OnStop();
	void OnUpdate();

private:

	ResourceAnimation* mAnimation;
	AnimationController* mController;
};

#endif
