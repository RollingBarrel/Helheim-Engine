#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterAnimation.h"
#include "ImporterTexture.h"

#include "Math/float4x4.h"

#include "ResourceAnimation.h"
#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "ResourceMaterial.h"
#include <map>

#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"


static void ImportNode(std::vector<ModelNode>& modelNodes, const char* filePath, const tinygltf::Model& model, unsigned int index, unsigned int& uid, unsigned int& size, bool modifyAssets, std::map<unsigned int, unsigned int>&importedMaterials, std::map<unsigned int,unsigned int>& importedTextures, std::map<std::pair<unsigned int, unsigned int>, unsigned int>& importedMeshes, int parentIndex = -1)
{
    ModelNode node;

    const auto& tinyNode = model.nodes[index];

    node.mName = tinyNode.name;

    math::float4x4 matrix = float4x4::identity;
    if (tinyNode.matrix.size() == 16)
    {
       matrix = float4x4(
            tinyNode.matrix[0], tinyNode.matrix[1], tinyNode.matrix[2], tinyNode.matrix[3],
            tinyNode.matrix[4], tinyNode.matrix[5], tinyNode.matrix[6], tinyNode.matrix[7],
            tinyNode.matrix[8], tinyNode.matrix[9], tinyNode.matrix[10], tinyNode.matrix[11],
            tinyNode.matrix[12], tinyNode.matrix[13], tinyNode.matrix[14], tinyNode.matrix[15]
        );

        matrix.Decompose(node.mTranslation, node.mRotation, node.mScale);

        node.mHasTransform = true;
    }
    else
    {
        if (tinyNode.translation.size() == 3) {
            node.mTranslation = { static_cast<float>(tinyNode.translation[0]),static_cast<float>(tinyNode.translation[1]), static_cast<float>(tinyNode.translation[2]) };

            node.mHasTransform = true;
        }
        else
        {
            node.mTranslation = float3::zero;
        }
        if (tinyNode.rotation.size() == 4) {
            node.mRotation = { static_cast<float>(tinyNode.rotation[0]),static_cast<float>(tinyNode.rotation[1]), static_cast<float>(tinyNode.rotation[2]),static_cast<float>(tinyNode.rotation[3]) };

            node.mHasTransform = true;
        }
        else
        {
            node.mRotation = Quat::identity;
        }
        if (tinyNode.scale.size() == 3) {
            node.mScale = { static_cast<float>(tinyNode.scale[0]),static_cast<float>(tinyNode.scale[1]), static_cast<float>(tinyNode.scale[2]) };

            node.mHasTransform = true;
        }
        else
        {
            node.mScale = float3::one;
        }
    }

    node.mMeshId = tinyNode.mesh;

    node.mCameraId = tinyNode.camera;

    node.mSkinId = tinyNode.skin;

    unsigned int meshId = 0;
    unsigned int materialId = 0;

    if (node.mMeshId != -1)
    {
        int i = 0;
        for (const auto& primitive : model.meshes[node.mMeshId].primitives)
        {
            if (importedMeshes.find({ node.mMeshId, i }) == importedMeshes.end())
            {
                ResourceMesh* rMesh = Importer::Mesh::Import(model, primitive, uid++);
                meshId = rMesh->GetUID();
                importedMeshes[{node.mMeshId, i}] = meshId;
                delete rMesh;
            }
            else
            {
                meshId = importedMeshes[{node.mMeshId, i}];
            }
            if (primitive.material != -1)
            {
                if (importedMaterials.find(primitive.material) == importedMaterials.end())
                {
                    ResourceMaterial* rMaterial = Importer::Material::Import(filePath, model, model.materials[primitive.material], uid, importedTextures, modifyAssets);
                    materialId = rMaterial->GetUID();
                    delete rMaterial;
                    importedMaterials[primitive.material] = materialId;
                }
                else
                {
                    materialId = importedMaterials[primitive.material];
                }
            }
            else
            {
                //Import default del material !!!
                ResourceMaterial* rMaterial = Importer::Material::ImportDefault();
                materialId = rMaterial->GetUID();
                delete rMaterial;
            }
            node.mUids.push_back({meshId, materialId});
            ++i;
        }
    }

    node.mParentIndex = parentIndex;

    size += node.mName.length() + 1         //Name
            + sizeof(int)                   //Parent Index in the vector
            + sizeof(int) * 3;              //Mesh/Camera/Skin 

    size += sizeof(bool);                   //Tranforms

    if (node.mHasTransform)
    {
        size += sizeof(float) * 3           //Pos
            + sizeof(float) * 4             //Rot
            + sizeof(float) * 3;            //Scale
    }

    if (node.mMeshId > -1)
    {
        size += sizeof(unsigned int)              //vector size
                + (sizeof(unsigned int) * 2) * node.mUids.size();  //Uid Mesh & Material
    }

    unsigned int currentIdx = modelNodes.size();

    modelNodes.push_back(node);

    for (int i = 0; i < tinyNode.children.size(); ++i)
    {
        ModelNode childNode;
        ImportNode(modelNodes, filePath, model, tinyNode.children[i], uid, size, modifyAssets, importedMaterials, importedTextures, importedMeshes, currentIdx);
    }

}

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

    std::map<std::pair<unsigned int,unsigned int>, unsigned int>importedMeshes;
    std::map<unsigned int, unsigned int>importedMaterials;
    std::map<unsigned int, unsigned int>importedTextures;
    unsigned int bufferSize = 0;

    unsigned int animationId = 0;

    ResourceModel* rModel = new ResourceModel(uid++);

    unsigned int currentUid = uid;

    for (int i = 0; i < model.scenes.size(); ++i)
    {
        for (int j = 0; j < model.scenes[i].nodes.size(); ++j)
        {
            ImportNode(rModel->modelNodes, filePath, model, model.scenes[i].nodes[j], currentUid, bufferSize, modifyAssets, importedMaterials, importedTextures, importedMeshes);
        }
    }

    if (!model.animations.empty())
    {
        for (const auto& srcAnimation : model.animations)
        {
            ResourceAnimation* ourAnimation = Importer::Animation::Import(model, srcAnimation, currentUid);
            animationId = ourAnimation->GetUID();

            delete ourAnimation;
        }
    }
    else
    {
        animationId = 0;
    }

    rModel->mAnimationUids.push_back(animationId); 

    bufferSize += sizeof(unsigned int);                                     //Nodes vector
    bufferSize += sizeof(unsigned int);                                     //Tamaño vector
    bufferSize += sizeof(unsigned int) * rModel->mAnimationUids.size();     //Animation UIDs

    if (rModel)
        Importer::Model::Save(rModel, bufferSize);

    return rModel;
}

void Importer::Model::Save(const ResourceModel* rModel, unsigned int& size)
{
    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int nodesSize = rModel->modelNodes.size();
    unsigned int bytes = sizeof(int);
    memcpy(cursor, &nodesSize, bytes);
    cursor += bytes;
   
    for (int i = 0; i < rModel->modelNodes.size(); ++i)
    {
        ModelNode currentNode = rModel->modelNodes[i];
        //Name
        bytes = currentNode.mName.length() + 1;
        memcpy(cursor, currentNode.mName.c_str(), bytes);
        cursor += bytes;

        bytes = sizeof(bool);
        memcpy(cursor, &currentNode.mHasTransform, bytes);
        cursor += bytes;

        if (currentNode.mHasTransform)
        {
            //Translation
            bytes = sizeof(float) * 3;
            memcpy(cursor, currentNode.mTranslation.ptr(), bytes);
            cursor += bytes;
            //Rotation
            bytes = sizeof(float) * 4;
            memcpy(cursor, currentNode.mRotation.ptr(), bytes);
            cursor += bytes;
            //Scale
            bytes = sizeof(float) * 3;
            memcpy(cursor, currentNode.mScale.ptr(), bytes);
            cursor += bytes;
        }

        //Parent Index
        bytes = sizeof(int);
        memcpy(cursor, &currentNode.mParentIndex, bytes);
        cursor += bytes;
        //MeshId
        bytes = sizeof(int);
        memcpy(cursor, &currentNode.mMeshId, bytes);
        cursor += bytes;
        //CameraId
        bytes = sizeof(int);
        memcpy(cursor, &currentNode.mCameraId, bytes);
        cursor += bytes;
        //SkinId
        bytes = sizeof(int);
        memcpy(cursor, &currentNode.mSkinId, bytes);
        cursor += bytes;

        if (currentNode.mMeshId > -1)
        {
            //Library Uids
            unsigned int uidsSize = currentNode.mUids.size();
            bytes = sizeof(unsigned int);
            memcpy(cursor, &uidsSize, bytes);
            cursor += bytes;
            for (unsigned int i = 0; i < uidsSize; ++i)
            {
                bytes = sizeof(unsigned int);
                memcpy(cursor, &currentNode.mUids[i].first, bytes);
                cursor += bytes;
                bytes = sizeof(unsigned int);
                memcpy(cursor, &currentNode.mUids[i].second, bytes);
                cursor += bytes;
            }
        }
    }
    //Animation Uids
    unsigned int uidsSize = rModel->mAnimationUids.size();
    bytes = sizeof(unsigned int);
    memcpy(cursor, &uidsSize, bytes);
    cursor += bytes;
    for (unsigned int i = 0; i < uidsSize; ++i)
    {
        bytes = sizeof(unsigned int);
        memcpy(cursor, &rModel->mAnimationUids[i], bytes);
        cursor += bytes;
    }
    
    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rModel->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);
    
    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceModel* Importer::Model::Load(const char* fileName, unsigned int uid)
{
    char* fileBuffer = nullptr;
    ResourceModel* rModel = new ResourceModel(uid);
    
    if (App->GetFileSystem()->Load(fileName, &fileBuffer))
    { 
        char* cursor = fileBuffer;
    
        //Size of Nodes Vector
        unsigned int nodesSize = 0;
        unsigned int bytes = sizeof(int);
        memcpy(&nodesSize, cursor, bytes);
        cursor += bytes;

        rModel->modelNodes.reserve(nodesSize);

        //Nodes Data
        for (unsigned int i = 0; i < nodesSize; ++i)
        {
            ModelNode node;

            unsigned int count = 0;
            while (*(cursor)++ != '\0')
            {
                count++;
            }
            count++;
            cursor -= count;
            char* name = new char[count];
            char* ptr = name;
            while (count--)
            {
                *ptr++ = *cursor++;
            }

            node.mName = name;

            bytes = sizeof(bool);
            memcpy(&node.mHasTransform, cursor, bytes);
            cursor += bytes;

            if (node.mHasTransform)
            {
                bytes = sizeof(float) * 3;
                memcpy(node.mTranslation.ptr(), cursor, bytes);
                cursor += bytes;

                bytes = sizeof(float) * 4;
                memcpy(node.mRotation.ptr(), cursor, bytes);
                cursor += bytes;

                bytes = sizeof(float) * 3;
                memcpy(node.mScale.ptr(), cursor, bytes);
                cursor += bytes;
            }
            else
            {
                node.mTranslation = float3::zero;
                node.mRotation = Quat::identity;
                node.mScale = float3::one;
            }

            bytes = sizeof(int);
            memcpy(&node.mParentIndex, cursor, bytes);
            cursor += bytes;

            bytes = sizeof(int);
            memcpy(&node.mMeshId, cursor, bytes);
            cursor += bytes;

            bytes = sizeof(int);
            memcpy(&node.mCameraId, cursor, bytes);
            cursor += bytes;

            bytes = sizeof(int);
            memcpy(&node.mSkinId, cursor, bytes);
            cursor += bytes;

            if (node.mMeshId > -1)
            {
                unsigned int meshId = 0;
                unsigned int materialId = 0;
                //Library Uids
                unsigned int uidsSize = 0;
                bytes = sizeof(unsigned int);
                memcpy(&uidsSize, cursor, bytes);
                cursor += bytes;
                for (unsigned int i = 0; i < uidsSize; ++i)
                {
                    bytes = sizeof(unsigned int);
                    memcpy(&meshId, cursor, bytes);
                    cursor += bytes;
                    bytes = sizeof(unsigned int);
                    memcpy(&materialId, cursor, bytes);
                    cursor += bytes;

                    node.mUids.push_back({ meshId, materialId });
                }
            }

            rModel->modelNodes.push_back(node);
        }
    
        unsigned int uidsSize = 0;
        bytes = sizeof(unsigned int);
        memcpy(&uidsSize, cursor, bytes);
        cursor += bytes;
        for (unsigned int i = 0; i < uidsSize; ++i)
        {
            unsigned int animationUID = 0;
            bytes = sizeof(unsigned int);
            memcpy(&animationUID, cursor, bytes);
            *cursor += bytes;
    
            rModel->mAnimationUids.push_back({ animationUID });
        }
        delete[] fileBuffer;
    }

    return rModel;
}

