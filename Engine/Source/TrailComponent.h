#pragma once

#include "Component.h"
#include <deque>
#include "ColorGradient.h"
#include "BezierCurve.h"
#include "Archive.h"
#include "float3.h"

class ResourceTexture;

class ENGINE_API TrailComponent : public Component
{
	friend class InspectorPanel;
public:
	struct TrailPoint
	{
		TrailPoint(float3 position, float3 direction, float time, float distanceUV);
		float3 position;
		float3 direction;
		float creationTime;
		float distanceUV;
	};
	explicit TrailComponent(GameObject* ownerGameObject);
	TrailComponent(const TrailComponent& original, GameObject* owner);
	~TrailComponent() final;
	const char* GetFileName() const { return mFileName; }
	void Reset() override;

	void Init();
	void Update() override;
	void Draw() const;
	void Enable() override;
	void Disable() override;

	void AddFirstTrailPoint();
	void RemoveLastTrailPoint();


	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& archive) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

	void SetColorGradient(const ColorGradient& gradient) { mGradient = gradient; }

private:
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mFileName = fileName; }
	float3 RotationToVector(Quat rotation) const;

	unsigned int mResourceId = 148626881; // Default particle texture
	const char* mFileName = nullptr;

	float mMinDistance = 0.1f;
	int mMaxPoints = 1000.0f;

	std::deque<TrailPoint> mPoints;
	float mMaxLifeTime = 0; // If maxLiftime is 0, it means infinite lifetime
	ColorGradient mGradient;
	float3 mDirection = float3::unitY; // Fixed direction for the normal of the trailPoints
	float mTrailTime = 0.0f;
	bool mFixedDirection = false;

	bool mIsUVScrolling = true;
	float mUVScroll = 1.0f;

	ResourceTexture* mImage = nullptr;

	unsigned int mVAO = 0;
	unsigned int mVBO = 0;

	BezierCurve mWidth = BezierCurve(); // TODO: Make it a pointer so every particle that has a trail dont have a bezierCurve by itself
};
