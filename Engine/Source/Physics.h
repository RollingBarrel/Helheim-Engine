#pragma once

#include <map>
#include "float3.h"
#include "float2.h"
#include "MathGeoLibFwd.h"
class GameObject;

struct Hit
{
	GameObject* mGameObject;
	float3 mHitPoint;
	float mDistance;
};

namespace Physics
{
	ENGINE_API std::map<float, Hit> Raycast(Ray* ray);
	ENGINE_API Ray ScreenPointToRay(float2 mousePosition);
};