#pragma once

#include <map>
#include "MathGeoLibFwd.h"
class GameObject;

class ENGINE_API Physics
{
public:
	//Returns a map with the distance to the object and a pointer to the object
	static std::map<float, GameObject*> Raycast(Ray* ray);

};

