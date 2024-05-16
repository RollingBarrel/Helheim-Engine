#pragma once
#include <typeinfo>

struct Collider
{
	void* mCollider;
	const std::type_info& mTypeId;
};