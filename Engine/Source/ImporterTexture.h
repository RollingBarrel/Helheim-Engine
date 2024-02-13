#pragma once

class ResourceTexture;

namespace Importer
{
	namespace Texture
	{

		ResourceTexture* Import(const char* filePath, const unsigned int uid);

		void Save(const ResourceTexture* ourTexture);

		unsigned int Load( const char* fileName = nullptr );
	}
};