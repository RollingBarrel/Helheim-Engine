#pragma once

class ResourceMaterial;

namespace Importer
{
	namespace Material
	{;
		void Save(const ResourceMaterial* ourMaterial);
		ResourceMaterial* Load(const char* fileName, const unsigned int uid);
	}
};