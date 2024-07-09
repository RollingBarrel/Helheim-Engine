#include "SaveLoadMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "Archive.h"

#include "Math/float4.h"
#include "Math/float3.h"

void Importer::Material::Save(const ResourceMaterial* rMaterial)
{
    unsigned int texturesUID[4] = { (rMaterial->GetBaseColorTexture() != nullptr) ? rMaterial->GetBaseColorTexture()->GetUID() : 0,
                                    (rMaterial->GetMetallicRoughnessTexture() != nullptr) ? rMaterial->GetMetallicRoughnessTexture()->GetUID() : 0,
                                    (rMaterial->GetNormalTexture() != nullptr) ? rMaterial->GetNormalTexture()->GetUID() : 0,
                                    (rMaterial->GetEmissiveTexture() != nullptr) ? rMaterial->GetEmissiveTexture()->GetUID() : 0 };

    bool enables[4] = {
        rMaterial->IsBaseColorEnabled(),
        rMaterial->IsMetallicRoughnessEnabled(),
        rMaterial->IsNormalMapEnabled(),
        rMaterial->IsEmissiveEnabled()
    };

    unsigned int size = sizeof(texturesUID) +
        sizeof(enables) +
        sizeof(float) * 4 +
        sizeof(float) +
        sizeof(float) +
        sizeof(float) * 3;
    unsigned int nameSize = 0;
    if (rMaterial->GetName() != nullptr)
    {
        nameSize = sizeof(char) * (strlen(rMaterial->GetName()) + 1);
        size += nameSize;
    }
    else
    {
        size += 1;
    }

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(texturesUID);
    memcpy(cursor, texturesUID, bytes);
    cursor += bytes;

    bytes = sizeof(enables);
    memcpy(cursor, enables, bytes);
    cursor += bytes;

    float4 baseColorFactor = rMaterial->GetBaseColorFactor();
    bytes = sizeof(float) * 4;
    memcpy(cursor, baseColorFactor.ptr(), bytes);
    cursor += bytes;

    float metallicFactorFactor = rMaterial->GetMetallicFactor();
    bytes = sizeof(float);
    memcpy(cursor, &metallicFactorFactor, bytes);
    cursor += bytes;

    float roughnessFactor = rMaterial->GetRoughnessFactor();
    bytes = sizeof(float);
    memcpy(cursor, &roughnessFactor, bytes);
    cursor += bytes;

    float3 emissiveFactor = rMaterial->GetEmissiveFactor();
    bytes = sizeof(float) * 3;
    memcpy(cursor, emissiveFactor.ptr(), bytes);
    cursor += bytes;

    if (nameSize)
    {
        bytes = nameSize;
        memcpy(cursor, rMaterial->GetName(), bytes);
        cursor += bytes;
    }
    else
    {
        *cursor++ = '\0';
    }

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rMaterial->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceMaterial* Importer::Material::Load(const char* fileName, const unsigned int uid)
{
    char* fileBuffer = nullptr;
    ResourceMaterial* ret = nullptr;
    if (App->GetFileSystem()->Load(fileName, &fileBuffer))
    {

        char* cursor = fileBuffer;
        unsigned int texturesUID[4];
        unsigned int bytes = sizeof(texturesUID);

        memcpy(texturesUID, cursor, bytes);
        cursor += bytes;

        bool enables[4];
        bytes = sizeof(enables);
        memcpy(enables, cursor, bytes);
        cursor += bytes;

        float4 baseColorFactor;
        bytes = sizeof(float) * 4;
        memcpy(baseColorFactor.ptr(), cursor, bytes);
        cursor += bytes;

        float metallicFactor;
        bytes = sizeof(float);
        memcpy(&metallicFactor, cursor, bytes);
        cursor += bytes;

        float roughnessFator;
        bytes = sizeof(float);
        memcpy(&roughnessFator, cursor, bytes);
        cursor += bytes;

        float3 emissiveFactor;
        bytes = sizeof(float) * 3;
        memcpy(emissiveFactor.ptr(), cursor, bytes);
        cursor += bytes;

        const char* name = nullptr;
        if (*cursor != '\0')
        {
            name = cursor;
            while (*cursor++ != '\0');
        }
        else
        {
            ++cursor;
        }

        ret = new ResourceMaterial(uid, name, baseColorFactor.ptr(), metallicFactor, roughnessFator, emissiveFactor.ptr(), texturesUID[0], texturesUID[1], texturesUID[2], texturesUID[3], enables[0], enables[2], enables[2], enables[3]);
        Importer::Material::Save(ret);

        delete[] fileBuffer;
    }

    return ret;
}

void Importer::Material::SaveMatFile(const ResourceMaterial& rMat, const char* name)
{
    Archive doc;
    JsonObject root = doc.GetRootObject();
    if(name == nullptr)
        root.AddString("Name", rMat.GetName());
    else
        root.AddString("Name", name);
    root.AddFloats("BaseColorFactor", rMat.GetBaseColorFactor().ptr(), 4);
    root.AddFloat("MetallicFactor", rMat.GetMetallicFactor());
    root.AddFloat("RoughnessFactor", rMat.GetRoughnessFactor());
    root.AddFloats("EmissiveFactor", rMat.GetEmissiveFactor().ptr(), 3);
    root.AddInt("BaseColorTex", (rMat.GetBaseColorTexture()) ? rMat.GetBaseColorTexture()->GetUID() : 0);
    root.AddInt("MetallicRoughTex", (rMat.GetMetallicRoughnessTexture()) ? rMat.GetMetallicRoughnessTexture()->GetUID() : 0);
    root.AddInt("NormalTex", (rMat.GetNormalTexture()) ? rMat.GetNormalTexture()->GetUID() : 0);
    root.AddInt("EmissiveTex", (rMat.GetEmissiveTexture()) ? rMat.GetEmissiveTexture()->GetUID() : 0);
    root.AddBool("BaseColorEnabled", rMat.IsBaseColorEnabled());
    root.AddBool("MetallicRoughnessEnabled", rMat.IsMetallicRoughnessEnabled());
    root.AddBool("NormalMapEnabled", rMat.IsNormalMapEnabled());
    root.AddBool("EmissiveEnabled", rMat.IsEmissiveEnabled());

    std::string buffer = doc.Serialize();
    std::string assetName = ASSETS_MATERIAL_PATH;
    assert(name != nullptr || rMat.GetName() != nullptr);
    if (name == nullptr)
        assetName += rMat.GetName();
    else
        assetName += name;
    assetName += ".mat";
    App->GetFileSystem()->Save(assetName.c_str(), buffer.c_str(), buffer.size());
}
