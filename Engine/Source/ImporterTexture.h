#pragma once

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{
		void Import(const char* filePath, ResourceTexture* texture);

		void Save(const ResourceTexture* ourTexture);

		unsigned int Load(ResourceTexture* ourTexture, const char* fileName = nullptr );
	}
};