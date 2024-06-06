#pragma once
#include <deque>
#include "ColorGradient.h"
#include "BezierCurve.h"
#include "Archive.h"
#include "float3.h"

class ResourceTexture;

struct TrailPoint
{
	float3 position;
	float3 direction;

	float creationTime = 0.0f;
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
	void Draw() const;
	void Clear() { mPoints.clear(); }
	
	void AddTrailPositions(float3 position, Quat rotation);

	const float3& GetLastPosition() const;
	const float3& GetFirstPosition() const;
	unsigned long long GetSize() const { return mPoints.size(); }

	void SetImage(ResourceTexture* image) { mImage = image; }

	void Save(JsonObject& archive) const;
	void Load(const JsonObject& data);

private:
	std::deque<TrailPoint> mPoints;
	float mMaxLifeTime = 0; // If maxLiftime is 0, it means infinite lifetime
	ColorGradient mGradient;
	float3 mDirection = float3::unitY; // Fixed direction for the normal of the trailPoints
	float mTrailTime = 0.0f;
	bool mFixedDirection = false;

	ResourceTexture* mImage = nullptr;

	unsigned int mVAO = 0;
	unsigned int mVBO = 0;

	BezierCurve mWidth = BezierCurve(); // TODO: Make it a pointer so every particle that has a trail dont have a bezierCurve by itself
	float mInitialWith;
};