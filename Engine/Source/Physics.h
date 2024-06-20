#pragma once

#include <map>
#include "float3.h"
#include "float2.h"
#include "MathGeoLibFwd.h"
class GameObject;

struct Hit
{
	const GameObject* mGameObject = nullptr;
	float3 mHitPoint;
	float mDistance = 0;
};

namespace Physics
{
	ENGINE_API void Raycast(const Ray& ray, std::map<float, Hit>& hits);
	ENGINE_API Ray ScreenPointToRay(float2 screenPoint);
};