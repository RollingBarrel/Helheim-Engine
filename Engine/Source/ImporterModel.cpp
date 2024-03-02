#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterAnimation.h"


#include "Algorithm/Random/LCG.h"
#include "ResourceAnimation.h"
#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "ResourceMaterial.h"

#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

ResourceModel* Importer::Model::Import(const char* filePath, unsigned int uid, bool modifyAssets)
{
    unsigned int currUid = uid;
    ResourceModel* rModel = new ResourceModel(currUid++);

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;

    std::string error, warning;
    bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, filePath);
    if (!loadOk)
    {
        LOG("[MODEL] Error loading %s: %s", filePath, error.c_str());
    }

    if (!model.animations.empty())
    {

        for (const auto& srcAnimation : model.animations)
        {
            ResourceAnimation* ourAnimation = new ResourceAnimation();

            //Importer::Animation::Import(model, srcAnimation, ourAnimation);

                     
            delete ourAnimation;
            
        }
        
    }

    
    for (const auto& srcMesh : model.meshes)
    {
        for (const auto& primitive : srcMesh.primitives)
        {   
            ResourceMesh* rMesh = Importer::Mesh::Import(model, primitive, currUid++);

            if (primitive.material != -1) {
                ResourceMaterial* rMaterial = Importer::Material::Import(filePath, model, model.materials[primitive.material], currUid++, modifyAssets);

                rModel->SetUids(rMesh->GetUID(), rMaterial->GetUID());
                currUid = rMaterial->GetUID() + 1;
                delete rMaterial;
            }

            delete rMesh;
        }
    }

    if (rModel)
        Importer::Model::Save(rModel);

    return rModel;
}

void Importer::Model::Save(const ResourceModel* rModel)
{
    unsigned int numModels = rModel->GetUids().size();

    unsigned int size = sizeof(numModels) + sizeof(ResourceModel) * numModels;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(numModels);
    memcpy(cursor, &numModels, bytes);
    cursor += bytes;

    for (auto it = rModel->GetUids().cbegin(); it != rModel->GetUids().cend(); ++it)
    {
        bytes = sizeof(it->meshUID);
        memcpy(cursor, &it->meshUID, bytes);
        cursor += bytes;
        bytes = sizeof(it->materialUID);
        memcpy(cursor, &it->materialUID, bytes);
        cursor += bytes;
    }

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rModel->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceModel* Importer::Model::Load(const char* fileName, unsigned int uid)
{
    char* fileBuffer;
    ResourceModel* rModel = nullptr;
    if (App->GetFileSystem()->Load(fileName, &fileBuffer))
    {

        char* cursor = fileBuffer;

        unsigned int bytes = sizeof(unsigned int);
        unsigned int size = 0;
        memcpy(&size, cursor, bytes);
        cursor += bytes;

        rModel = new ResourceModel(uid);

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

        delete[] fileBuffer;
    }
    return rModel;
}
