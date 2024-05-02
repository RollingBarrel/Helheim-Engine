#include "SaveLoadMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Math/float4.h"

void Importer::Material::Save(const ResourceMaterial* rMaterial)
{
    unsigned int texturesUID[3] = { (rMaterial->GetBaseColorTexture() != nullptr) ? rMaterial->GetBaseColorTexture()->GetUID() : 0,
                                    (rMaterial->GetMetallicRoughnessTexture() != nullptr) ? rMaterial->GetMetallicRoughnessTexture()->GetUID() : 0,
                                    (rMaterial->GetNormalTexture() != nullptr) ? rMaterial->GetNormalTexture()->GetUID() : 0 };

    bool enables[3] = {
        rMaterial->IsBaseColorEnabled(),
        rMaterial->IsMetallicRoughnessEnabled(),
        rMaterial->IsNormalMapEnabled() };

    unsigned int size = sizeof(texturesUID) +
        sizeof(enables) +
        sizeof(float) * 4 +
        sizeof(float) +
        sizeof(float);

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
    memcpy(cursor, &baseColorFactor, bytes);
    cursor += bytes;

    float metallicFactorFactor = rMaterial->GetMetallicFactor();
    bytes = sizeof(float);
    memcpy(cursor, &metallicFactorFactor, bytes);
    cursor += bytes;

    float roughnessFactor = rMaterial->GetRoughnessFactor();
    bytes = sizeof(float);
    memcpy(cursor, &roughnessFactor, bytes);
    cursor += bytes;

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
        unsigned int texturesUID[3];
        unsigned int bytes = sizeof(texturesUID);

        memcpy(texturesUID, cursor, bytes);
        cursor += bytes;

        bool enables[3];
        bytes = sizeof(enables);
        memcpy(enables, cursor, bytes);
        cursor += bytes;

        bool enablebaseColorTexture = enables[0];
        bool enableMetallicRoughnessTexture = enables[1];
        bool enableNormalTexture = enables[2];

        float4 baseColorFactor;
        bytes = sizeof(float) * 4;
        memcpy(&baseColorFactor, cursor, bytes);
        cursor += bytes;

        float metallicFactor;
        bytes = sizeof(float);
        memcpy(&metallicFactor, cursor, bytes);
        cursor += bytes;

        float roughnessFator;
        bytes = sizeof(float);
        memcpy(&roughnessFator, cursor, bytes);
        cursor += bytes;
        ret = new ResourceMaterial(uid, baseColorFactor.ptr(), metallicFactor, roughnessFator, texturesUID[0], texturesUID[1], texturesUID[2]);
        Importer::Material::Save(ret);

        delete[] fileBuffer;
    }

    return ret;
}