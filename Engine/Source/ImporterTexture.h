#pragma once

struct ResourceTex
{
	int mWidth;
	int mHeight;

	int internalFormat;
	int format;
	int type;

	unsigned char* pixels;
};

namespace Importer
{
	namespace Texture
	{
		void Import(const char* filePath, ResourceTex& texture);

		void Load();
		void Save();
	}
};