#pragma once
#include "Resource.h"

#include "float4.h"

class ResourceTexture;

class ResourceMaterial : public Resource
{
public:
    ResourceMaterial(
        unsigned int uid,
        float4 baseColorFactor,
        float metallicFactor,
        float roughnessFactor,
        unsigned int baseColorTexUid,
        unsigned int metallicRoughTexUid,
        unsigned int normalTexUid);

    ~ResourceMaterial();

    // Getter functions
    float4 GetBaseColorFactor() const { return mBaseColorFactor; }
    float GetMetallicFactor() const { return mMetallicFactor; }
    float GetRoughnessFactor() const { return mRoughnessFactor; }
    ResourceTexture* GetBaseColorTexture() const { return mBaseColorTexture; }
    ResourceTexture* GetMetallicRoughnessTexture() const { return mMetallicRoughnessTexture; }
    ResourceTexture* GetNormalTexture() const { return mNormalTexture; }
    bool IsBaseColorEnabled() const { return mEnableBaseColorTexture; }
    bool IsMetallicRoughnessEnabled() const { return mEnableMetallicRoughnessTexture; }
    bool IsNormalMapEnabled() const { return mEnableNormalMap; }


private:
    float4 mBaseColorFactor;
    float mMetallicFactor;
    float mRoughnessFactor;

    ResourceTexture* mBaseColorTexture = nullptr;
    ResourceTexture* mMetallicRoughnessTexture = nullptr;
    ResourceTexture* mNormalTexture = nullptr;

    bool mEnableBaseColorTexture;
    bool mEnableMetallicRoughnessTexture;
    bool mEnableNormalMap;

    friend class InspectorPanel;
};

