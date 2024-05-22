#pragma once
#include <typeinfo>
#include "Component.h"

struct Collider
{
	Collider(Component* component, ComponentType type) {
		mCollider = component;
		mTypeId = type;
	}

	void* mCollider;
	ComponentType mTypeId;
};