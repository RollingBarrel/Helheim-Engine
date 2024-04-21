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

	ResourceTexture(
		unsigned int uid,
		unsigned int width, 
		unsigned int height, 
		unsigned int internalFormat, 
		unsigned int texFormat, 
		unsigned int dataType, 
		unsigned int mipLevels, 
		unsigned int numPixels,
		bool hasAlpha,
		unsigned int openGLId,
		unsigned int texHandle);

	~ResourceTexture();

	// Getters for attributes
	unsigned int GetWidth() const { return mWidth; }
	unsigned int GetHeight() const { return mHeight; }
	unsigned int GetInternalFormat() const { return mInternalFormat; }
	unsigned int GetTexFormat() const { return mTexFormat; }
	unsigned int GetDataType() const { return mDataType; }
	unsigned int GetMipLevels() const { return mMipLevels; }
	unsigned int GetNumPixels() const { return mNumPixels; }
	bool HasAlpha() const { return mHasAlpha; }
	unsigned int GetOpenGLId() const { return mOpenGLId; }
	unsigned int GetTextureHandle() const { return static_cast<unsigned int>(mTexHandle); }

private:
	unsigned int mWidth;
	unsigned int mHeight;

	unsigned int mInternalFormat;
	unsigned int mTexFormat;
	unsigned int mDataType;
	unsigned int mMipLevels;
	unsigned int mNumPixels;

	bool mHasAlpha;

	unsigned int mOpenGLId;
	uint64_t mTexHandle;
};