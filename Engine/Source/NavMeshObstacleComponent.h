#pragma once
#include "Component.h"

class NavMeshObstacleComponent : public Component
{
public:
	NavMeshObstacleComponent(GameObject* ownerGameObject);

	~NavMeshObstacleComponent();

	void Update() override;
	void Reset();
	Component* Clone(GameObject* owner) const override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	float GetRadius() {
		return mRadius;
	}

	void SetRadius(float radius) {
		mRadius = radius;
	}

	float GetHeight() {
		return mHeight;
	}

	void SetHeight(float height) {
		mHeight = height;
	}

private:
	float mRadius;
	float mHeight;
	//TODO check if we need to implement Move Threshold and Carve
	//bool mMoveThreshold;
	//float mCarve;
};