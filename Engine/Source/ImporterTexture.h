#pragma once

struct ResourceTexture
{
	bool HasAlpha() const { return mHasAlpha; }

	unsigned int mWidth;
	unsigned int mHeight;

	unsigned int mInternalFormat;
	unsigned int mFormat;
	unsigned int mType;
	unsigned int mMipLevels;
	unsigned int mNumPixels;
	unsigned char* mPixels;
	const char* mTextureName;
	unsigned int mUID = 0;

	bool mHasAlpha;

	unsigned int openGlId = 0;

	unsigned int CreateTexture();

};

namespace Importer
{
	namespace Texture
	{
		void Import(const char* filePath, ResourceTexture* texture);

		void Save(const ResourceTexture* ourTexture);

		unsigned int Load(ResourceTexture* ourTexture, const char* fileName = nullptr );
	}
};