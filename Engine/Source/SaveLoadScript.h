#pragma once

class ResourceScript;

namespace Importer
{
	namespace Script
	{
		void Save(const ResourceScript* ourTexture);
		ResourceScript* Load(const char* filePath, unsigned int uid);
	}
}

