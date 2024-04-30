#pragma once

#include <map>
#include "float3.h"
#include "MathGeoLibFwd.h"
class GameObject;

struct Hit
{
	GameObject* mGameObject;
	float3 mHitPoint;
	float mDistance;
};

class ENGINE_API Physics
{
public:
	//Returns a map with the distance to the object and a pointer to the object
	static std::map<float, Hit> Raycast(Ray* ray);
	static Ray ScreenPointToRay(float2 mousePosition);
};

