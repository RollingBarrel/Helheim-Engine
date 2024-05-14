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
	
	void AddTrailPositions(float3 position, float3 rotation);

	float3 GetLastPosition() const { return mPoints.back().mPosition; }
	float3 GetFirstPosition() const { return mPoints.front().mPosition; }
	unsigned long long GetSize() const { return mPoints.size(); }

	void SetImage(ResourceTexture* image) { mImage = image; }

	void SaveJson(Archive& archive) const;
	void LoadJson(const rapidjson::Value& data);

private:
	std::deque<TrailPoint> mPoints;
	float mMaxLifeTime; // If maxLiftime is 0, it means infinite lifetime
	ColorGradient mGradient;
	float3 mDirection; // Fixed direction for the normal of the trailPoints
	ResourceTexture* mImage = nullptr;
	BezierCurve mWidth = BezierCurve();
};