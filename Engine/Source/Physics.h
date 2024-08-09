#pragma once

#include <set>
#include "float3.h"
#include "float2.h"
#include "MathGeoLibFwd.h"
class GameObject;

struct Hit
{
	const GameObject* mGameObject = nullptr;
	float3 mHitPoint;
	float mDistance = 0;

	inline bool operator<(const Hit& otherHit)
	{
		return mDistance < otherHit.mDistance;
	}

	bool IsValid() const { return mGameObject && mDistance >= 0.0f; }
};

inline bool operator<(const Hit& hit, const Hit& otherHit)
{
	return hit.mDistance < otherHit.mDistance;
}


namespace Physics
{
	ENGINE_API void Raycast(std::multiset<Hit>& hits, const Ray& ray, float maxDistance = 10000.0f, std::vector<std::string>* ignoreTags = nullptr);
	ENGINE_API void Raycast(Hit& hit, const Ray& ray, float maxDistance = 10000.0f, std::vector<std::string>* ignoreTags = nullptr);
	ENGINE_API Ray ScreenPointToRay(float2 screenPoint);
};