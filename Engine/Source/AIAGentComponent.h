#pragma once
#include "Component.h"
#include <vector>
#include "float3.h"

class ENGINE_API AIAgentComponent :public Component
{
public:
	AIAgentComponent(GameObject* ownerGameObject);
	AIAgentComponent(const AIAgentComponent& original, GameObject* owner);
	void Reset();
	~AIAgentComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void SetNavigationPath(const float3& destination);

	//const float GetRadius() const { return mRadius; };
	//const float GetHeight() const { return mHeight; };
	//const float GetStepHeight() const { return mStepHeight; };
	//const unsigned int GetMaxSlope() const { return mMaxSlope; };
	//const float GetSpeed() const { return mSpeed; };
	//const float GetAngularSpeed() const { return mAngularSpeed; };
	//const float GetAcceleration() const { return mAcceleration; };
	//const float GetStoppingDistance() const { return mStoppingDistance; };
	//void SetRadius(float radius) { mRadius = radius; };
	//void SetHeight(float height) { mHeight = height; };
	//void SetStepHeight(float stepHeight) { mStepHeight = stepHeight; };
	//void SetMaxSlope(unsigned int maxSlope) { mMaxSlope = maxSlope; };
	//void SetSpeed(float speed) { mSpeed = speed; };
	//void SetAngularSpeed(float angularSpeed) { mAngularSpeed = angularSpeed; };
	//void SetAcceleration(float acceleration) { mAcceleration = acceleration; };
	//void SetStoppingDistance(float stoppingDistance) { mStoppingDistance = stoppingDistance; };


	void MoveAgent(float speed) const;
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;
private:

	//Agent Parameters:
	//float mRadius = 0.0f;
	//float mHeight = 0.0f;
	//float mStepHeight = 0.0f;
	//unsigned int mMaxSlope = 0;
	////Steering Parameters:
	//float mSpeed = 1.0f;
	//float mAngularSpeed = 0.0f; //Maximum speed of rotation
	//float mAcceleration = 0.0f; 
	//float mStoppingDistance = 0.0f; //The GO will stop when this close to the goal location.
	std::vector<float3> mNavPositions;
};

