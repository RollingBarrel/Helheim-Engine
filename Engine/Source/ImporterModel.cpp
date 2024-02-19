#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "Algorithm/Random/LCG.h"

#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "ResourceMaterial.h"

#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

ResourceModel* Importer::Model::Import(const char* filePath, const char* assetsPath, unsigned int uid)
{
    ResourceModel* rModel = new ResourceModel(uid, assetsPath);

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;

    std::string error, warning;
    bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, assetsPath);
    if (!loadOk)
    {
        LOG("[MODEL] Error loading %s: %s", assetsPath, error.c_str());
    }
    
    for (int i = 0; i < model.images.size(); ++i)
    {
        std::string pngName = filePath;
        unsigned filePos = pngName.find_last_of('/');
        pngName = pngName.substr(0, filePos+1);
        pngName.append(model.images[i].uri);

        App->GetResource()->ImportFile(pngName.c_str());
    }
    
    for (const auto& srcMesh : model.meshes)
    {
        for (const auto& primitive : srcMesh.primitives)
        {   
            ResourceMesh* mesh = Importer::Mesh::Import(model, primitive, math::LCG().Int());
            Importer::Mesh::Save(mesh);

            if (primitive.material != -1) {
                ResourceMaterial* material = Importer::Material::Import(model, model.materials[primitive.material]);
                Importer::Material::Save(material);

                rModel->SetUids(mesh->GetUID(), material->GetUID());

                delete material;
                material = nullptr;
            }

            delete mesh;
            mesh = nullptr;
        }
    }

    return rModel;
}

void Importer::Model::Save(const ResourceModel* ourModel)
{
    unsigned int numModels = ourModel->GetUids().size();

    unsigned int size = sizeof(numModels) + sizeof(ResourceModel) * numModels;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(numModels);
    memcpy(cursor, &numModels, bytes);
    cursor += bytes;

    for (auto it = ourModel->GetUids().cbegin(); it != ourModel->GetUids().cend(); ++it)
    {
        bytes = sizeof(it->meshUID);
        memcpy(cursor, &it->meshUID, bytes);
        cursor += bytes;
        bytes = sizeof(it->materialUID);
        memcpy(cursor, &it->materialUID, bytes);
        cursor += bytes;
    }

    App->GetFileSystem()->Save(ourModel->GetLibraryFile().c_str(), fileBuffer, size);

    RELEASE_ARRAY(fileBuffer);
}

ResourceModel* Importer::Model::Load(const char* fileName, unsigned int uid)
{
    char* fileBuffer;
    App->GetFileSystem()->Load(fileName, &fileBuffer);

    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(unsigned int);
    unsigned int size = 0;
    memcpy(&size, cursor, bytes);
    cursor += bytes;

    ResourceModel* rModel = new ResourceModel(uid, fileName);
    
    for (int i = 0; i < size; ++i)
    {
        unsigned int meshId = 0;
        memcpy(&meshId, cursor, bytes);
        cursor += bytes;
        unsigned int materialId = 0;
        memcpy(&materialId, cursor, bytes);
        cursor += bytes;
        rModel->SetUids(meshId, materialId);
    }

    return rModel;
}
