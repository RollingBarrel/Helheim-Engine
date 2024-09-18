#pragma once

#include <deque>
#include "ColorGradient.h"
#include "BezierCurve.h"
#include "float3.h"

class ResourceTexture;
class GameObject;

class ENGINE_API Trail
{
	friend class InspectorPanel;
public:
	struct BufferVertex
	{
		float3 mPosition;
		float2 mTexCoord;
		float mDistance;
		float3 mDirection;
	};
	struct TrailPoint
	{
		TrailPoint(float3 position, float3 direction, float time, float mDistanceUV);
		float3 mPosition;
		float3 mDirection;
		float mCreationTime;
		float mDistanceUV;
	};
	Trail();
	Trail(const Trail& original);
	~Trail();
	const char* GetFileName() const { return mImageName; }
	float GetLifeTime() const { return mMaxLifeTime; }
	size_t GetTrailSize() const { return mPoints.size(); }

	void Init();
	void Update();
	void UpdateTrailComponent(GameObject* owner);
	void UpdateLineComponent(GameObject* origin, GameObject* final);
	void UpdateTrailParticle(float3 position);
	void Draw() const;
	void Enable();
	void Disable();

	void AddFirstTrailPoint(GameObject* mOwner);
	void AddFirstTrailPoint(float3 position);
	void RemoveLastTrailPoint();

	void Save(JsonObject& archive) const;
	void Load(const JsonObject& data);

	void SetColorGradient(const ColorGradient& gradient) { mGradient = gradient; }
	void SetLifeTime(float time) { mMaxLifeTime = time; }

private:
	float3 CalculateDirection(const float3& position, const float3& norm) const;
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mImageName = fileName; }

	ResourceTexture* mImage = nullptr;
	const char* mImageName = nullptr;

	float mMinDistance = 0.1f;
	int mMaxPoints = 1000;

	std::deque<TrailPoint> mPoints;
	std::vector<BufferVertex> mBuffer;
	float3 mLastPointPosition;

	float mMaxLifeTime = 0; // If maxLiftime is 0, it means infinite lifetime
	float3 mDirection = float3::unitY; // Fixed direction for the normal of the trailPoints
	bool mFixedDirection = false;
	float mTrailTime = 0.0f;

	float mMinDist = 0.0f;
	float mMaxDist = 0.0f;

	bool mIsTilled = true;
	float mTilling = 1.0f;

	unsigned int mVAO = 0;
	unsigned int mVBO = 0;
	unsigned int mSSBOColor = 0;
	unsigned int mSSBOWidth = 0;

	ColorGradient mGradient;
	BezierCurve mWidth = BezierCurve(); //TODO: Make it a pointer so every particle that has a trail dont have a bezierCurve by itself
};