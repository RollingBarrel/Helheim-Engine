#pragma once
#include "fmod_studio.hpp"
class FmodUtils
{
public:
	static void CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line);
	#define CheckFmodError(_result) CheckFmodErrorFunction(_result, __FILE__, __LINE__)
};

