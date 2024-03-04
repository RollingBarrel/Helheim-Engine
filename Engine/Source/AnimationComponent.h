#ifndef _COMPONENT_ANIMATION_H_
#define _COMPONENT_ANIMATION_H_
#include "Component.h"

class AnimationController;

class PointLightComponent : public Component {
public:
	void OnStart();
	void OnStop();
	void OnUpdate();
private:
	AnimationController* controller;
	unsigned int uID;
};

#endif
