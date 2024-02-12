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
	ResourceTexture(
		unsigned int width, 
		unsigned int height, 
		unsigned int internalFormat, 
		unsigned int texFormat, 
		unsigned int dataType, 
		unsigned int mipLevels, 
		unsigned int numPixels,
		unsigned char* pixels, 
		bool hasAlpha, 
		unsigned int openGLId);

	~ResourceTexture();

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

