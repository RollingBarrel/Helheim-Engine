#pragma once

#include <unordered_map>
#include <string>

#include "dxgiformat.h"

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{

		ResourceTexture* Import(const char* filePath, const unsigned int uid);

		void Save(const ResourceTexture* ourTexture, const unsigned char* pixels);

		ResourceTexture* Load(const char* filePath, unsigned int uid);

		bool endsWith(const std::string& str, const std::string& suffix);
		
		extern const std::unordered_map<std::string, DXGI_FORMAT> compressionFormatNaming; // Map of texture filename ending to DXGI_FORMAT
	}
};