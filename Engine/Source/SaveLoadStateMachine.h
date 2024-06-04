#pragma once
#include "Globals.h"

class AnimationStateMachine;

namespace Importer
{
	namespace StateMachine
	{
		ENGINE_API void Save(AnimationStateMachine* ourStateMachine);
		ENGINE_API AnimationStateMachine* Load(const char* fileName, unsigned int uid);

	}
}

