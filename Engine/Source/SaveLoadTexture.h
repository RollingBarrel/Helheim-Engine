#pragma once
#include "Globals.h"

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{
		ENGINE_API void Save(const ResourceTexture* ourTexture, const unsigned char* pixels);
		ENGINE_API ResourceTexture* Load(const char* filePath, unsigned int uid);
	}
}