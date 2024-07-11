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
#include <unordered_map>

#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"


static void ImportNode(std::vector<ModelNode>& modelNodes, const char* filePath, const tinygltf::Model& model, unsigned int index, unsigned int& uid, unsigned int& size, bool modifyAssets, std::unordered_map<unsigned int, unsigned int>& importedMaterials, std::unordered_map<unsigned int, unsigned int>& importedTextures, std::unordered_map<unsigned int, std::vector<unsigned int>>& importedMeshes, int parentIndex = -1)
{
    ModelNode node;

    node.mGltfId = index;
    const tinygltf::Node& tinyNode = model.nodes[node.mGltfId];
    node.mParentVecIdx = parentIndex;

    node.mName = tinyNode.name;

    float4x4 matrix = float4x4::identity;
    if (tinyNode.matrix.size() == 16)
    {
        matrix = float4x4(
            tinyNode.matrix[0], tinyNode.matrix[4], tinyNode.matrix[8], tinyNode.matrix[12],
            tinyNode.matrix[1], tinyNode.matrix[5], tinyNode.matrix[9], tinyNode.matrix[13],
            tinyNode.matrix[2], tinyNode.matrix[6], tinyNode.matrix[10], tinyNode.matrix[14],
            tinyNode.matrix[3], tinyNode.matrix[7], tinyNode.matrix[11], tinyNode.matrix[15]
        );

        matrix.Decompose(node.mTranslation, node.mRotation, node.mScale);
        node.mHasTransform = true;
    }
    else
    {
        if (tinyNode.translation.size() == 3) 
        {
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

    node.mLightId = tinyNode.light;

    if (node.mLightId > -1)
    {
        node.mLight.mType = model.lights[tinyNode.light].type;

        if (!model.lights[tinyNode.light].color.empty())
        {
            node.mLight.mColor = math::float3(model.lights[tinyNode.light].color[0], model.lights[tinyNode.light].color[1], model.lights[tinyNode.light].color[2]);
        }
        else
        {
            node.mLight.mColor = math::float3(1.0f);
        }

        node.mLight.mIntensity = model.lights[tinyNode.light].intensity;
        //TODO Lights: Range is infinite if is 0;
        node.mLight.mRange = (model.lights[tinyNode.light].range == 0.0f) ? 100.0f : model.lights[tinyNode.light].range;

        if (node.mLight.mType.compare("spot") == 0)
        {
            node.mLight.mInnerConeAngle = model.lights[tinyNode.light].spot.innerConeAngle;
            node.mLight.mOuterConeAngle = model.lights[tinyNode.light].spot.outerConeAngle;
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
        bool imported = true;
        if (importedMeshes.find(node.mMeshId) == importedMeshes.end())
        {
            std::vector<unsigned int>vec;
            importedMeshes[node.mMeshId] = vec;
            imported = false;
        }
        for (const tinygltf::Primitive& primitive : model.meshes[node.mMeshId].primitives)
        {
            if (!imported)
            {
                ResourceMesh* rMesh = Importer::Mesh::Import(model, primitive, uid++);
                meshId = rMesh->GetUID();
                std::vector<unsigned int>& vec = importedMeshes[node.mMeshId];
                vec.push_back(meshId);
                delete rMesh;
            }
            else
            {
                meshId = importedMeshes[node.mMeshId][i];
            }
            if (primitive.material != -1)
            {
                if (importedMaterials.find(primitive.material) == importedMaterials.end())
                {
                    ResourceMaterial* rMaterial = Importer::Material::GltfImport(filePath, model, model.materials[primitive.material], uid, importedTextures, modifyAssets);
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
                if (rMaterial)
                {
                    delete rMaterial;
                }
                materialId = 999999999;
            }
            node.mUids.emplace_back(meshId, materialId);
            ++i;
        }
    }

    size += node.mName.length() + 1         //Name
            + sizeof(int)                   //Parent Index in the vector
            + sizeof(int)                   //Gltf Index
            + sizeof(int) * 4;              //Mesh/Camera/Skin/Light

    size += sizeof(bool);                   //Tranforms

    if (node.mHasTransform)
    {
        size += sizeof(float) * 3           //Pos
            + sizeof(float) * 4             //Rot
            + sizeof(float) * 3;            //Scale
    }

    if (node.mLightId > -1)
    {
        size += sizeof(unsigned int)
                + node.mLight.mType.length() + 1
                + sizeof(float) * 5;

        if (node.mLight.mType.compare("spot") == 0)
        {
            size += sizeof(float) * 2;
        }
    }

    if (node.mMeshId > -1)
    {
        size += sizeof(unsigned int)              //vector size
                + (sizeof(unsigned int) * 2) * node.mUids.size();  //Uid Mesh & Material
    }

    const unsigned int currentIdx = modelNodes.size();
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
        assert("Error parsing the gltf");
    }

    std::unordered_map<unsigned int, std::vector<unsigned int>>importedMeshes;
    std::unordered_map<unsigned int, unsigned int>importedMaterials;
    std::unordered_map<unsigned int, unsigned int>importedTextures;
    unsigned int bufferSize = 0;

    ResourceModel* rModel = new ResourceModel(uid++);

    unsigned int currentUid = uid;
    assert(model.scenes.size() <= 1 && "No support for multiple scenes gltf");
    for (int i = 0; i < model.scenes.size(); ++i)
    {
        for (int j = 0; j < model.scenes[0].nodes.size(); ++j)
        {
            ImportNode(rModel->modelNodes, filePath, model, model.scenes[0].nodes[j], currentUid, bufferSize, modifyAssets, importedMaterials, importedTextures, importedMeshes);
        }
    }

    if (!model.skins.empty())
    {
        std::vector<std::pair<unsigned int, float4x4>> vec;
        for (const tinygltf::Skin& skin : model.skins)
        {
            const tinygltf::Accessor& inverseBindMatricesAccesor = model.accessors[skin.inverseBindMatrices];
            const tinygltf::BufferView& inverseBindMatricesBufferView = model.bufferViews[inverseBindMatricesAccesor.bufferView];

            const float* inverseBindMatricesPtr = reinterpret_cast<const float*>(&model.buffers[inverseBindMatricesBufferView.buffer].data[inverseBindMatricesBufferView.byteOffset + inverseBindMatricesAccesor.byteOffset]);
            const size_t num_inverseBindMatrices = inverseBindMatricesAccesor.count;

            vec.reserve(skin.joints.size());
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
                vec.emplace_back(skin.joints[i], inverseBindMatrix);
            }
            rModel->mInvBindMatrices.push_back(vec);
            vec.clear();
        }
    }

    if (!model.animations.empty())
    {
        for (const tinygltf::Animation& srcAnimation : model.animations)
        {
            ResourceAnimation* ourAnimation = Importer::Animation::Import(model, srcAnimation, currentUid);
            rModel->mAnimationUids.push_back(ourAnimation->GetUID());
            delete ourAnimation;
        }
    }

    bufferSize += sizeof(unsigned int);                                     //Nodes vector
    bufferSize += sizeof(unsigned int);                                     //Size vector
    bufferSize += sizeof(unsigned int) * rModel->mAnimationUids.size();     //Animation UIDs
    bufferSize += sizeof(unsigned int);                                     //num of invBindMatrix vectors
    for (int j = 0; j < rModel->mInvBindMatrices.size(); ++j) 
    {
        bufferSize += sizeof(unsigned int);                                 //num of elements in the array
        for(int i = 0; i < rModel->mInvBindMatrices[j].size(); ++i)
        {
            bufferSize += sizeof(unsigned int);                                  //gltfId
            bufferSize += sizeof(float) * 16;                                    // Matrix
        }
    }

    Importer::Model::Save(rModel, bufferSize);

    return rModel;
}
