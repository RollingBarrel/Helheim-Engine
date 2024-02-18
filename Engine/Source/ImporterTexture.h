#pragma once

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{

		ResourceTexture* Import(const char* filePath, const unsigned int uid);

		void Save(const ResourceTexture* ourTexture);

		ResourceTexture* Load(const char* filePath, unsigned int uid);
	}
};