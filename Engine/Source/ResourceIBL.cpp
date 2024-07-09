#include "ResourceIBL.h"
#include "glew.h"

ResourceIBL::ResourceIBL(unsigned int uid, unsigned int environmentTextureId, unsigned int environmentSize, unsigned int irradianceTextureId, unsigned int irradianceSize, unsigned int specPrefilteredTexId, unsigned int specPrefilteredSize, unsigned int envBRDFTexId, unsigned int specEnvBRDFSize) : Resource(uid, Type::IBL),
mEnvironmentTextureId(environmentTextureId),
mEnvironmentTexSize(environmentSize),
mIrradianceTextureId(irradianceTextureId),
mIrradianceTexSize(irradianceSize),
mSpecPrefilteredTexId(specPrefilteredTexId),
mSpecPrefilteredTexSize(specPrefilteredSize),
mEnvBRDFTexId(envBRDFTexId),
mEnvBRDFTexSize(specEnvBRDFSize)
{
}

ResourceIBL::~ResourceIBL()
{
	glDeleteTextures(1, &mEnvironmentTextureId);
	glDeleteTextures(1, &mIrradianceTextureId);
	glDeleteTextures(1, &mSpecPrefilteredTexId);
	glDeleteTextures(1, &mEnvBRDFTexId);
}