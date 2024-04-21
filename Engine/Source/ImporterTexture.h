#pragma once

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{

		ResourceTexture* Import(const char* filePath, const unsigned int uid);

		void Save(const ResourceTexture* ourTexture, const unsigned char* pixels);

		ResourceTexture* Load(const char* filePath, unsigned int uid);

		bool endsWith(const std::string& str, const std::string& suffix);
	}
};