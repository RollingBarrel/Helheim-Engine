#pragma once

class ResourceScript;

namespace Importer
{
	namespace Script
	{

		ResourceScript* Import(const char* filePath, const unsigned int uid);

		void Save(const ResourceScript* ourTexture);

		ResourceScript* Load(const char* filePath, unsigned int uid);
	}
};