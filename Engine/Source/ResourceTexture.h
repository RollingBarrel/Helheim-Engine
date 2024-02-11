#pragma once
#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	enum class Format
	{
		RGB,
		RGBA,
		BGR,
		BGRA,
		Luminance,
		Unknown
	};

	ResourceTexture();
	ResourceTexture(unsigned int id);
	~ResourceTexture();

	void Import(const char* filePath) override;
	void Save() override;
	unsigned int Load(const char* fileName) override;

	std::string GetExtension() override { return ".tex"; }

	unsigned int CreateTexture();	

private:
	unsigned int mWidth;
	unsigned int mHeight;

	unsigned int mInternalFormat;
	unsigned int mTexFormat;
	unsigned int mDataType;
	unsigned int mMipLevels;
	unsigned int mNumPixels;
	unsigned char* mPixels;

	bool mHasAlpha;

	unsigned int mOpenGLId;
};

