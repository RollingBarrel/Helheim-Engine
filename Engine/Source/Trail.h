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

	float3 TopPointPosition();
	float3 BotPointPosition();
	float2 TopPointTexCoords();
	float2 BotPointTexCoords();
	float4 Color();

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
	void Save(Archive& archive) const;
	void LoadFromJSON(const rapidjson::Value& data);

private:
	std::deque<TrailPoint> mPoints;
	float mMaxLifeTime = 3.0f; // If maxLiftime is 0, it means infinite lifetime
	float mWidth = 0.5;
	ColorGradient mGradient;
	float3 mDirection = float3::unitY; // Fixed direction for the normal of the trailPoints

	float4x4 mModel = float4x4::identity;
	ResourceTexture* mImage = nullptr;

	unsigned int mVAO = 0;
	unsigned int mVBO = 0;

};