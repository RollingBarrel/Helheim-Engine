#pragma once
#include <deque>
#include "MathGeoLib.h"
#include "ColorGradient.h"

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
	~Trail();

	void Update();
	
	void AddTrailPositions(float3 position, float3 rotation);

	float3 GetLastPosition() const { return mPoints.back().mPosition; }
	float3 GetFirstPosition() const { return mPoints.front().mPosition; }
	int GetSize() const { return mPoints.size(); }


	void SetImage(ResourceTexture* image) { mImage = image; }

private:
	std::deque<TrailPoint> mPoints;
	float mMaxLifeTime; // If maxLiftime is 0, it means infinite lifetime
	float mWidth;
	ColorGradient mGradient;
	float3 mDirection; // Fixed direction for the normal of the trailPoints
	ResourceTexture* mImage = nullptr;
};