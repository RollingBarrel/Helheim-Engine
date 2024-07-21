#pragma once
#include "Resource.h"

class ENGINE_API ResourceIBL : public Resource
{
public:
    ResourceIBL(unsigned int uid, unsigned int environmentTextureId, unsigned int environmentSize, unsigned int irradianceTextureId, unsigned int irradianceSize, unsigned int specPrefilteredTexId, unsigned int specPrefilteredSize, unsigned int envBRDFTexId, unsigned int specEnvBRDFSize);
    ~ResourceIBL();

    unsigned int GetEnvironmentTextureId() const { return mEnvironmentTextureId; }
    unsigned int GetEnvironmentTexSize() const { return mEnvironmentTexSize; }
    unsigned int GetIrradianceTextureId() const { return mIrradianceTextureId; }
    unsigned int GetIrradianceTexSize() const { return mIrradianceTexSize; }
    unsigned int GetSpecPrefilteredTexId() const { return mSpecPrefilteredTexId; }
    unsigned int GetSpecPrefilteredTexSize() const { return mSpecPrefilteredTexSize; }
    unsigned int GetEnvBRDFTexId() const { return mEnvBRDFTexId; }
    unsigned int GetEnvBRDFTexSize() const { return mEnvBRDFTexSize; }

private:
    unsigned int mEnvironmentTextureId;
    unsigned int mEnvironmentTexSize;
    unsigned int mIrradianceTextureId;
    unsigned int mIrradianceTexSize;
    unsigned int mSpecPrefilteredTexId;
    unsigned int mSpecPrefilteredTexSize;
    unsigned int mEnvBRDFTexId;
    unsigned int mEnvBRDFTexSize;
};

