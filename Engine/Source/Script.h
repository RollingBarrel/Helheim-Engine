#pragma once
#include "Globals.h"


class ENGINE_API Script
{
public:

	virtual ~Script() {}

	virtual void Start() = 0;
	virtual void Update() = 0;

private:

	const char* name = nullptr;

};

