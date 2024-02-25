#pragma once
#include "Component.h"

class AgentProfile;

class AIAGentComponent :public Component
{
public:
	AIAGentComponent(GameObject* ownerGameObject, AgentProfile* agentType);
	AIAGentComponent(const AIAGentComponent& original, GameObject* owner);
	void Reset();
	~AIAGentComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;
private:
	AgentProfile* mType;

	//Steering Parameters:
	float mSpeed = 0.0f;
	float mAngularSpeed = 0.0f; //Maximum speed of rotation
	float mAcceleration = 0.0f; 
	float mStoppingDistance = 0.0f; //The GO will stop when this close to the goal location.

};

