#pragma once
#include "MeleeWeapon.h"
#include "Macros.h"

class TrailComponent;
class BoxColliderComponent;
class GameObject;
struct CollisionData;

class Bat : public MeleeWeapon
{
public:
	Bat(BoxColliderComponent* collider);
	~Bat();
};
