#pragma once
class ResourceStateMachine;

namespace Importer
{
	namespace StateMachine
	{
		ResourceStateMachine* Import(const char* assetsPath, unsigned int uid);
	}
}
