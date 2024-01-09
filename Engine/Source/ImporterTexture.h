#pragma once

struct ResourceTex
{
	int widthTex, heightTex, colorTex;

	int internalFormat, format, type;
};

namespace Importer
{
	namespace Texture
	{
		void Import(const char* filePath);

		void LoadTexture(const char* path, ResourceTex& texture);

	}
};