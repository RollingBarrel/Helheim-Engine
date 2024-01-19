#pragma once
class Texture
{
public:
    Texture();
    ~Texture();

    void LoadTexture(const char* textureName);

    inline unsigned getTextureID() const { return mTextureID; }
    inline unsigned getWidth() const { return mImageWidth; }
    inline unsigned getHeight() const { return mImageHeight; }
    bool hasAlpha() const { return mHasAlpha; }

private:
    unsigned mTextureID;
    unsigned mImageWidth;
    unsigned mImageHeight;
    bool mHasAlpha = false;
};

