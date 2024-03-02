#pragma once
#include "Component.h"

class AIAgentComponent :public Component
{
public:
	AIAgentComponent(GameObject* ownerGameObject);
	AIAgentComponent(const AIAgentComponent& original, GameObject* owner);
	void Reset();
	~AIAgentComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;
	const float getRadius() const { return mRadius; };
	const float getHeight() const { return mHeight; };
	const float getStepHeight() const { return mStepHeight; };
	const unsigned int getMaxSlope() const { return mMaxSlope; };
	const float getSpeed() const { return mSpeed; };
	const float getAngularSpeed() const { return mAngularSpeed; };
	const float getAcceleration() const { return mAcceleration; };
	const float getStoppingDistance() const { return mStoppingDistance; };
private:
	//Agent Parameters:
	float mRadius = 0.0f;
	float mHeight = 0.0f;
	float mStepHeight = 0.0f;
	unsigned int mMaxSlope = 0;
	//Steering Parameters:
	float mSpeed = 0.0f;
	float mAngularSpeed = 0.0f; //Maximum speed of rotation
	float mAcceleration = 0.0f; 
	float mStoppingDistance = 0.0f; //The GO will stop when this close to the goal location.

};

