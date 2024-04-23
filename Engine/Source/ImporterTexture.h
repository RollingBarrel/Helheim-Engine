#pragma once

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{
		ResourceTexture* Import(const char* filePath, const unsigned int uid);
	}
}