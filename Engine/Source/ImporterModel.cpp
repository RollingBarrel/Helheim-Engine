#include "Globals.h"
#include "SaveLoadModel.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterAnimation.h"

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

        matrix = matrix.Transposed();
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
            if (importedMeshes.find({ node.mMeshId, i}) == importedMeshes.end())
            {
                ResourceMesh* rMesh = Importer::Mesh::Import(model, primitive, uid++);
                meshId = rMesh->GetUID();
                importedMeshes[{ node.mMeshId, i}] = meshId;
                delete rMesh;
            }
            else
            {
                meshId = importedMeshes[{ node.mMeshId, i}];
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

    std::map<std::pair<unsigned int, unsigned int>, unsigned int>importedMeshes;
    std::map<unsigned int, unsigned int>importedMaterials;
    std::map<unsigned int, unsigned int>importedTextures;
    unsigned int bufferSize = 0;

    unsigned int animationId = 0;

    ResourceModel* rModel = new ResourceModel(uid++);

    if (!model.skins.empty())
    {
        for (const auto& skins : model.skins)
        {
            const int inverseBindMatricesIndex = skins.inverseBindMatrices;
            const tinygltf::Accessor& inverseBindMatricesAccesor = model.accessors[inverseBindMatricesIndex];

            const tinygltf::BufferView& inverseBindMatricesBufferView = model.bufferViews[inverseBindMatricesAccesor.bufferView];

            const unsigned char* inverseBindMatricesBuffer = &model.buffers[inverseBindMatricesBufferView.buffer].data[inverseBindMatricesBufferView.byteOffset + inverseBindMatricesAccesor.byteOffset];

            const float* inverseBindMatricesPtr = reinterpret_cast<const float*>(inverseBindMatricesBuffer);

            size_t num_inverseBindMatrices = inverseBindMatricesAccesor.count;

            for (size_t i = 0; i < num_inverseBindMatrices; i++)
            {
                const float* matrixPtr = &inverseBindMatricesPtr[i * 16];

                float4x4 inverseBindMatrix;

                for (size_t row = 0; row < 4; row++)
                {
                    for (size_t col = 0; col < 4; col++)
                    {
                        inverseBindMatrix[col][row] = matrixPtr[row * 4 + col];
                    }
                }
                rModel->mInvBindMatrices.push_back({ model.nodes[skins.joints[i]].name, inverseBindMatrix });

            }
        }
    }


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
    bufferSize += sizeof(unsigned int);                                     //Tama�o vector
    bufferSize += sizeof(unsigned int) * rModel->mAnimationUids.size();     //Animation UIDs
    bufferSize += sizeof(unsigned int);
    for (const auto& invBindMatrix : rModel->mInvBindMatrices) {
        bufferSize += sizeof(float) * 16;                                   // Size of the float array
        bufferSize += sizeof(unsigned int);                                 // Size of the string length
        //bufferSize += sizeof(char) * invBindMatrix.first.size();                // Size of the string characters
        bufferSize += sizeof(invBindMatrix.first);
    }

    if (rModel)
        Importer::Model::Save(rModel, bufferSize);

    return rModel;
}
