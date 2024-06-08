#pragma once
#include "Resource.h"
#include <stdint.h>

class ENGINE_API ResourceTexture : public Resource
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
		unsigned int glTarget,
		unsigned int width, 
		unsigned int height, 
		unsigned int internalFormat, 
		unsigned int texFormat, 
		unsigned int dataType, 
		unsigned int mipLevels, 
		unsigned int numPixels,
		bool hasAlpha,
		unsigned int texelSize);

	~ResourceTexture();


	// Getters for attributes
	unsigned int GetWidth() const { return mWidth; }
	unsigned int GetHeight() const { return mHeight; }
	unsigned int GetInternalFormat() const { return mInternalFormat; }
	unsigned int GetTexFormat() const { return mTexFormat; }
	unsigned int GetDataType() const { return mDataType; }
	unsigned int GetMipLevels() const { return mMipLevels; }
	unsigned int GetPixelsSize() const { return mPixelsSize; }
	bool HasAlpha() const { return mHasAlpha; }
	unsigned int GetOpenGLId() const { return mOpenGLId; }
	unsigned int GetTextureHandle() const { return static_cast<unsigned int>(mTexHandle); }
	bool IsBindless() const { return mTexHandle != 0; }
	unsigned int GetTextureGLTarget() const { return mGLTarget; }
	bool IsCubemap() const;
	unsigned int GetTexelSize() const { return mTexelSize; }

	void MakeTextutureBindless() const;
	void GenerateMipmaps() const;

private:
	unsigned int mGLTarget;
	unsigned int mWidth;
	unsigned int mHeight;

	unsigned int mInternalFormat;
	unsigned int mTexFormat;
	unsigned int mDataType;
	mutable unsigned int mMipLevels;
	mutable unsigned int mPixelsSize;
	unsigned int mTexelSize;

	bool mHasAlpha;

	unsigned int mOpenGLId;
	mutable uint64_t mTexHandle;
};