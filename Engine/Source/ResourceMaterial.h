#pragma once
#include "Resource.h"

#include "float4.h"
#include "float3.h"

class ResourceTexture;

class ENGINE_API ResourceMaterial : public Resource
{
public:
    ResourceMaterial(
        unsigned int uid,
        float* baseColorFactor,
        float metallicFactor,
        float roughnessFactor,
        float* emissiveFactor,
        unsigned int baseColorTexUid,
        unsigned int metallicRoughTexUid,
        unsigned int normalTexUid,
        unsigned int emissiveTexUid);

    ResourceMaterial(
        unsigned int uid,
        const char* name,
        float* baseColorFactor,
        float metallicFactor,
        float roughnessFactor,
        float* emissiveFactor,
        unsigned int baseColorTexUid,
        unsigned int metallicRoughTexUid,
        unsigned int normalTexUid,
        unsigned int emissiveTexUid,
        bool enableBaseColorTexture,
        bool enableMetallicRoughnessTexture,
        bool enableNormalMap,
        bool enableEmissiveTexture);

    ~ResourceMaterial();

    // Getter functions
    float4 GetBaseColorFactor() const { return mBaseColorFactor; }
    float GetMetallicFactor() const { return mMetallicFactor; }
    float GetRoughnessFactor() const { return mRoughnessFactor; }
    float3 GetEmissiveFactor() const { return mEmissiveFactor; }
    const ResourceTexture* GetBaseColorTexture() const { return mBaseColorTexture; }
    const ResourceTexture* GetMetallicRoughnessTexture() const { return mMetallicRoughnessTexture; }
    const ResourceTexture* GetNormalTexture() const { return mNormalTexture; }
    const ResourceTexture* GetEmissiveTexture() const { return mEmissiveTexture; }
    bool IsBaseColorEnabled() const { return mEnableBaseColorTexture; }
    bool IsMetallicRoughnessEnabled() const { return mEnableMetallicRoughnessTexture; }
    bool IsNormalMapEnabled() const { return mEnableNormalMap; }
    bool IsEmissiveEnabled() const { return mEnableEmissiveTexture; }

    const char* GetName() const { return mName; }
    void SetName(const char* newName);

private:
    char* mName;

    float4 mBaseColorFactor;
    float mMetallicFactor;
    float mRoughnessFactor;
    float3 mEmissiveFactor;

    ResourceTexture* mBaseColorTexture = nullptr;
    ResourceTexture* mMetallicRoughnessTexture = nullptr;
    ResourceTexture* mNormalTexture = nullptr;
    ResourceTexture* mEmissiveTexture = nullptr;

    bool mEnableBaseColorTexture;
    bool mEnableMetallicRoughnessTexture;
    bool mEnableNormalMap;
    bool mEnableEmissiveTexture;

    friend class InspectorPanel;
};

