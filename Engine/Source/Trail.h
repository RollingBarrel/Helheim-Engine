#pragma once
#include <deque>
#include "MathGeoLib.h"
#include "ColorGradient.h"

class TrailPoint
{
public:
	TrailPoint();
	~TrailPoint();


private:
	float3 mPosition;
	float3 mDirection;
	float mLifeTime;
};

class Trail
{
public:
	Trail();
	~Trail();

	void Update();
	void Init();
	void Draw();
	
	void AddTrailPositions(float3 position, float3 rotation);

private:
	std::deque<TrailPoint> mPoints;
	float mMaxLifeTime; // If maxLiftime is 0, it means infinite lifetime
	float mWidth;
	ColorGradient mGradient;
	float3 mDirection; // Fixed direction for the normal of the trailPoints

	unsigned int mVAO = 0;
	unsigned int mVBO = 0;
	unsigned int mUBO = 0;

};