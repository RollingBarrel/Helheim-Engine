#pragma once

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{
		void Save(const ResourceTexture* ourTexture, const unsigned char* pixels);
		ResourceTexture* Load(const char* filePath, unsigned int uid);
	}
}