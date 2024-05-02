#pragma once

class ResourceScript;

namespace Importer
{
	namespace Script
	{
		ResourceScript* Import(const char* filePath, const unsigned int uid);
	}
}

