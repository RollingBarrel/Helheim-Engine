#pragma once

#include "Globals.h"
class ResourceStateMachine;

namespace Importer
{
	namespace StateMachine
	{
		ENGINE_API void Save(const ResourceStateMachine* ourStateMachine);

		ENGINE_API ResourceStateMachine* Load(const char* fileName, unsigned int uid);
	}
}