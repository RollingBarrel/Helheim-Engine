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
		void Import(const char* filePath);

		void LoadTexture(const char* path, ResourceTex& texture);

	}
};