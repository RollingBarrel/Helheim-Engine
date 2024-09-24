#pragma once
#include "Component.h"
#include <vector>
#include "float3.h"
#define CROWD_OFF_INDEX 101

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

	void StartCrowdNavigation();
	void PauseCrowdNavigation();
	bool FleeFromTarget(const float3& target);

	const float GetRadius() const { return mRadius; };
	const float GetHeight() const { return mHeight; };
	const float GetMaxSpeed() const { return mMaxSpeed; };
	const float GetMaxAcceleration() const { return mMaxAcceleration; };

	void SetRadius(float radius) { mRadius = radius; };
	void SetHeight(float height) { mHeight = height; };
	void SetMaxSpeed(float speed) { mMaxSpeed = speed; };
	void SetMaxAcceleration(float acceleration) { mMaxAcceleration = acceleration; };

	const float3& GetDirection() const { return mDirection; }

	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;
private:

	void Enable() override;
	void Disable() override;

	float3 mDirection = float3::zero;

	//Agent Parameters:
	float mRadius = 0.6f;
	float mHeight = 2.0f;
	float mMaxSpeed = 5.0f;
	float mMaxAcceleration = 16.0f;

	std::vector<float3> mNavPositions;
	unsigned int mCrowdId = CROWD_OFF_INDEX;
};

