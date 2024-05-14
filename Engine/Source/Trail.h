#pragma once
#include <deque>
#include "MathGeoLib.h"
#include "ColorGradient.h"
#include "BezierCurve.h"
#include "Archive.h"

class ResourceTexture;

class TrailPoint
{
public:
	TrailPoint();
	~TrailPoint();

	float3 TopPointPosition();
	float3 BotPointPosition();
	float2 TopPointTexCoords();
	float2 BotPointTexCoords();
	float4 Color();

	float3 GetPosition() const { return mPosition; }
	float3 GetDirection() const { return mDirection; }
	float GetLifeTime() const { return mLifeTime; }

private:
	float3 mPosition;
	float3 mDirection;
	float mLifeTime;
};

class Trail
{
	friend class InspectorPanel;
public:
	Trail();
	explicit Trail(const Trail& original); //Copy constructor
	~Trail();

	void Update();
	void Init();
	void Draw();
	
	void AddTrailPositions(float3 position, Quat rotation);

	float3 GetLastPosition() const { return mPoints.back().GetPosition(); }
	float3 GetFirstPosition() const { return mPoints.front().GetPosition(); }
	unsigned long long GetSize() const { return mPoints.size(); }

	void SetImage(ResourceTexture* image) { mImage = image; }

	void SaveJson(Archive& archive) const;
	void LoadJson(const rapidjson::Value& data);

private:
	std::deque<TrailPoint> mPoints;
	float mMaxLifeTime; // If maxLiftime is 0, it means infinite lifetime
	ColorGradient mGradient;
	float3 mDirection = float3::unitY; // Fixed direction for the normal of the trailPoints

	float4x4 mModel = float4x4::identity;
	ResourceTexture* mImage = nullptr;

	unsigned int mVAO = 0;
	unsigned int mVBO = 0;

	BezierCurve mWidth = BezierCurve();
};