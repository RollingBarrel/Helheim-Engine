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


static void ImportNode(ModelNode& node, const char* filePath, const tinygltf::Model& model, unsigned int index, unsigned int& uid, unsigned int& size, bool modifyAssets)
{
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
    }
    else
    {
        if (tinyNode.translation.size() == 3) {
            node.mTranslation = { static_cast<float>(tinyNode.translation[0]),static_cast<float>(tinyNode.translation[1]), static_cast<float>(tinyNode.translation[2]) };
        }
        else
        {
            node.mTranslation = float3::zero;
        }
        if (tinyNode.rotation.size() == 4) {
            node.mRotation = { static_cast<float>(tinyNode.rotation[0]),static_cast<float>(tinyNode.rotation[1]), static_cast<float>(tinyNode.rotation[2]),static_cast<float>(tinyNode.rotation[3]) };
        }
        else
        {
            node.mRotation = Quat::identity;
        }
        if (tinyNode.scale.size() == 3) {
            node.mScale = { static_cast<float>(tinyNode.scale[0]),static_cast<float>(tinyNode.scale[1]), static_cast<float>(tinyNode.scale[2]) };
        }
        else
        {
            node.mScale = float3(1.f);
        }
    }

    node.mMeshId = tinyNode.mesh;

    node.mCameraId = tinyNode.camera;

    node.mSkinId = tinyNode.skin;

    if (node.mMeshId != -1)
    {
        for (const auto& primitive : model.meshes[node.mMeshId].primitives)
        {
            ResourceMesh* rMesh = Importer::Mesh::Import(model, primitive, uid++);
            node.mMeshUID = rMesh->GetUID();
            if (primitive.material != -1)
            {
                ResourceMaterial* rMaterial = Importer::Material::Import(filePath, model, model.materials[primitive.material], uid, modifyAssets);
                node.mMaterialUID = rMaterial->GetUID();
                delete rMaterial;
            }
            else
            {
                node.mMaterialUID = 0;
            }

            delete rMesh;
        }
    }
    else
    {
        node.mMeshUID = 0;
    }

    size += sizeof(unsigned int) + node.mName.length() + 1         //Name
        + sizeof(float) * 3                 //Pos
        + sizeof(float) * 4                 //Rot
        + sizeof(float) * 3                 //Scale
        + sizeof(int) * 3                   //Mesh/Camera/Skin 
        + sizeof(unsigned int) * 2          //Uid Mesh & Material
        + sizeof(unsigned int);             //Size Children   

    for (const auto& child : tinyNode.children)
    {   
        ModelNode childNode;
        ImportNode(childNode, filePath, model, child, uid, size, modifyAssets);
        node.mChildren.push_back(childNode);
    }
}

static void SaveNode(const ModelNode& currentNode, char** cursor)
{
    unsigned int bytes = sizeof(unsigned int);
    unsigned int nameSize = currentNode.mName.length() + 1;
    memcpy((*cursor), &nameSize, bytes);
    *cursor += bytes;
    //Name
    bytes = currentNode.mName.length() + 1;
    memcpy((*cursor), currentNode.mName.c_str(), bytes);
    *cursor += bytes;
    //Translation
    bytes = sizeof(float) * 3;
    memcpy((*cursor), currentNode.mTranslation.ptr(), bytes);
    *cursor += bytes;
    //Rotation
    bytes = sizeof(float) * 4;
    memcpy((*cursor), currentNode.mRotation.ptr(), bytes);
    *cursor += bytes;
    //Scale
    bytes = sizeof(float) * 3;
    memcpy((*cursor), currentNode.mScale.ptr(), bytes);
    *cursor += bytes;
    //MeshId
    bytes = sizeof(int);
    memcpy((*cursor), &currentNode.mMeshId, bytes);
    *cursor += bytes;
    //CameraId
    bytes = sizeof(int);
    memcpy((*cursor), &currentNode.mCameraId, bytes);
    *cursor += bytes;
    //SkinId
    bytes = sizeof(int);
    memcpy((*cursor), &currentNode.mSkinId, bytes);
    *cursor += bytes;
    //Library Uids
    bytes = sizeof(unsigned int);
    memcpy((*cursor), &currentNode.mMeshUID, bytes);
    *cursor += bytes;
    bytes = sizeof(unsigned int);
    memcpy((*cursor), &currentNode.mMaterialUID, bytes);
    *cursor += bytes;
    
    //Children
    bytes = sizeof(unsigned int);
    unsigned int childSize = currentNode.mChildren.size();
    memcpy((*cursor), &childSize, bytes);
    *cursor += bytes;

    for (int i = 0; i < currentNode.mChildren.size(); ++i)
    {
        SaveNode(currentNode.mChildren[i], cursor);
    }
}

static void LoadNode(ModelNode& node, char** cursor)
{

    unsigned int bytes = sizeof(unsigned int);
    unsigned int nameSize = 0;
    memcpy(&nameSize, *cursor, bytes);
    *cursor += bytes;

    bytes = nameSize;
    char* name = new char[nameSize];
    memcpy(name, *cursor, bytes);
    *cursor += bytes;

    node.mName = name;

    bytes = sizeof(float) * 3;
    memcpy(node.mTranslation.ptr(), *cursor, bytes);
    *cursor += bytes;

    bytes = sizeof(float) * 4;
    memcpy(node.mRotation.ptr(), *cursor, bytes);
    *cursor += bytes;

    bytes = sizeof(float) * 3;
    memcpy(node.mScale.ptr(), *cursor, bytes);
    *cursor += bytes;

    bytes = sizeof(int);
    memcpy(&node.mMeshId, *cursor, bytes);
    *cursor += bytes;

    bytes = sizeof(int);
    memcpy(&node.mCameraId, *cursor, bytes);
    *cursor += bytes;

    bytes = sizeof(int);
    memcpy(&node.mSkinId, *cursor, bytes);
    *cursor += bytes;

    bytes = sizeof(unsigned int);
    memcpy(&node.mMeshUID, *cursor, bytes);
    *cursor += bytes;

    bytes = sizeof(unsigned int);
    memcpy(&node.mMaterialUID, *cursor, bytes);
    *cursor += bytes;

    unsigned int childSize = 0;
    bytes = sizeof(unsigned int);
    memcpy(&childSize, *cursor, bytes);
    *cursor += bytes;

    for (int i = 0; i < childSize; ++i)
    {
        ModelNode child;
        LoadNode(child, cursor);
        node.mChildren.push_back(child);
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
    
    unsigned int currentUid = uid;

    unsigned int bufferSize = 0;

    ModelNode rootNode;

    ImportNode(rootNode, filePath, model, 0, currentUid, bufferSize, modifyAssets);

    ResourceModel* rModel = new ResourceModel(currentUid++, rootNode);

    if (rModel)
        Importer::Model::Save(rModel, bufferSize);

    return rModel;
}

void Importer::Model::Save(const ResourceModel* rModel, unsigned int& size)
{
    char* fileBuffer = new char[size];
    char* ptr = fileBuffer;

    SaveNode(rModel->GetRoot(), &ptr);
   
    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rModel->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);
    
    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceModel* Importer::Model::Load(const char* fileName, unsigned int uid)
{
    char* fileBuffer;
    ResourceModel* rModel = nullptr;

    ModelNode root;

    if (App->GetFileSystem()->Load(fileName, &fileBuffer))
    { 
        char* cursor = fileBuffer;

        LoadNode(root, &cursor);

        rModel = new ResourceModel(uid, root);
    
        delete[] fileBuffer;
    }
    return rModel;
}

