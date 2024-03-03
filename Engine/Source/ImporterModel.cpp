#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "Algorithm/Random/LCG.h"

#include "Math/float4x4.h"

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
    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;

    std::string error, warning;
    bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, filePath);
    if (!loadOk)
    {
        LOG("[MODEL] Error loading %s: %s", filePath, error.c_str());
    }
    
    unsigned int currentUid = uid;

    ModelNode rootNode = ImportNode(filePath, model, 0, currentUid, modifyAssets);

    ResourceModel* rModel = new ResourceModel(currentUid++, rootNode);

    //for (const auto& srcMesh : model.meshes)
    //{
    //    for (const auto& primitive : srcMesh.primitives)
    //    {   
    //        ResourceMesh* rMesh = Importer::Mesh::Import(model, primitive, currUid++);
    //
    //        if (primitive.material != -1) {
    //            ResourceMaterial* rMaterial = Importer::Material::Import(filePath, model, model.materials[primitive.material], currUid++, modifyAssets);
    //
    //            rModel->SetUids(rMesh->GetUID(), rMaterial->GetUID());
    //            currUid = rMaterial->GetUID() + 1;
    //            delete rMaterial;
    //        }
    //
    //        delete rMesh;
    //    }
    //}

    if (rModel)
        Importer::Model::Save(rModel);

    return rModel;
}

void Importer::Model::Save(const ResourceModel* rModel)
{
    //unsigned int numModels = rModel->GetUids().size();
    //
    //unsigned int size = sizeof(numModels) + sizeof(ResourceModel) * numModels;
    //
    //char* fileBuffer = new char[size];
    //char* cursor = fileBuffer;
    //
    //unsigned int bytes = sizeof(numModels);
    //memcpy(cursor, &numModels, bytes);
    //cursor += bytes;
    //
    //for (auto it = rModel->GetUids().cbegin(); it != rModel->GetUids().cend(); ++it)
    //{
    //    bytes = sizeof(it->meshUID);
    //    memcpy(cursor, &it->meshUID, bytes);
    //    cursor += bytes;
    //    bytes = sizeof(it->materialUID);
    //    memcpy(cursor, &it->materialUID, bytes);
    //    cursor += bytes;
    //}
    //
    //const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rModel->GetUID(), true);
    //App->GetFileSystem()->Save(libraryPath, fileBuffer, size);
    //
    //delete[] libraryPath;
    //delete[] fileBuffer;
}

ResourceModel* Importer::Model::Load(const char* fileName, unsigned int uid)
{
    //char* fileBuffer;
    ResourceModel* rModel = nullptr;
    //if (App->GetFileSystem()->Load(fileName, &fileBuffer))
    //{
    //
    //    char* cursor = fileBuffer;
    //
    //    unsigned int bytes = sizeof(unsigned int);
    //    unsigned int size = 0;
    //    memcpy(&size, cursor, bytes);
    //    cursor += bytes;
    //
    //    rModel = new ResourceModel(uid);
    //
    //    for (int i = 0; i < size; ++i)
    //    {
    //        unsigned int meshId = 0;
    //        memcpy(&meshId, cursor, bytes);
    //        cursor += bytes;
    //        unsigned int materialId = 0;
    //        memcpy(&materialId, cursor, bytes);
    //        cursor += bytes;
    //        rModel->SetUids(meshId, materialId);
    //    }
    //
    //    delete[] fileBuffer;
    //}
    return rModel;
}

ModelNode& Importer::Model::ImportNode(const char* filePath, const tinygltf::Model& model, unsigned int index, unsigned int uid, bool modifyAssets)
{
    const auto& tinyNode = model.nodes[index];

    ModelNode node;

    node.mName = tinyNode.name.c_str(); //Change to Memcpy 

    math::float4x4 matrix;
    if (tinyNode.matrix.size() == 16)
    {
        matrix = matrix = float4x4(
            tinyNode.matrix[0], tinyNode.matrix[1], tinyNode.matrix[2], tinyNode.matrix[3],
            tinyNode.matrix[4], tinyNode.matrix[5], tinyNode.matrix[6], tinyNode.matrix[7],
            tinyNode.matrix[8], tinyNode.matrix[9], tinyNode.matrix[10], tinyNode.matrix[11],
            tinyNode.matrix[12], tinyNode.matrix[13], tinyNode.matrix[14], tinyNode.matrix[15]
        );

        matrix.Decompose(node.mTranslation, node.mRotation, node.mScale);
    }
    else
    {
        if (tinyNode.translation.size() == 3) {
           node.mTranslation = { static_cast<float>(tinyNode.translation[0]),static_cast<float>(tinyNode.translation[1]), static_cast<float>(tinyNode.translation[2]) };
        }
        if (tinyNode.rotation.size() == 4) {
            node.mRotation = { static_cast<float>(tinyNode.rotation[0]),static_cast<float>(tinyNode.rotation[1]), static_cast<float>(tinyNode.rotation[2]),static_cast<float>(tinyNode.rotation[3]) };
        }
        if (tinyNode.scale.size() == 3) {
            node.mScale = { static_cast<float>(tinyNode.scale[0]),static_cast<float>(tinyNode.scale[1]), static_cast<float>(tinyNode.scale[2]) };
        }
    }

    node.mMeshId = tinyNode.mesh;

    node.mCameraId = tinyNode.camera;

    node.mSkinId = tinyNode.skin;

    for (const auto& primitive : model.meshes[node.mMeshId].primitives)
    {
        ResourceMesh* rMesh = Importer::Mesh::Import(model, primitive, uid++);

        if (primitive.material != -1)
        {
            ResourceMaterial* rMaterial = Importer::Material::Import(filePath, model, model.materials[primitive.material], uid++, modifyAssets);
            node.mUids.push_back({ rMesh->GetUID(), rMaterial->GetUID() });
            uid++;
            delete rMaterial;
        }

        delete rMesh;
    }

    for (const auto& child : tinyNode.children)
    {
        node.mChildren.push_back(ImportNode(filePath, model, child, uid++, modifyAssets));
    }

    return node;
}
