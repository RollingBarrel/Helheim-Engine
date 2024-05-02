#include "SaveLoadModel.h"
#include "ResourceModel.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

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

    //Joints
    unsigned int jointsSize = rModel->mInvBindMatrices.size();
    bytes = sizeof(unsigned int);
    memcpy(cursor, &jointsSize, bytes);
    cursor += bytes;

    for (unsigned int i = 0; i < jointsSize; ++i)
    {
        bytes = sizeof(int);
        int lenString = sizeof(rModel->mInvBindMatrices[i].first);
        memcpy(cursor, &lenString, bytes);
        cursor += bytes;

        bytes = sizeof(char) * lenString;
        memcpy(cursor, &rModel->mInvBindMatrices[i].first, bytes);
        cursor += bytes;

        bytes = sizeof(float4x4);
        memcpy(cursor, &rModel->mInvBindMatrices[i].second, bytes);
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
            cursor += bytes;

            rModel->mAnimationUids.push_back({ animationUID });
        }

        //Joints
        unsigned int jointsSize = 0;
        bytes = sizeof(unsigned int);
        memcpy(&jointsSize, cursor, bytes);
        cursor += bytes;

        rModel->mInvBindMatrices.resize(jointsSize);

        for (unsigned int i = 0; i < jointsSize; ++i)
        {
            bytes = sizeof(int);
            int lenString;
            memcpy(&lenString, cursor, bytes);
            cursor += bytes;

            bytes = sizeof(char) * lenString;
            memcpy(&rModel->mInvBindMatrices[i].first, cursor, bytes);
            cursor += bytes;

            bytes = sizeof(float4x4);
            memcpy(&rModel->mInvBindMatrices[i].second, cursor, bytes);
            cursor += bytes;
        }

        delete[] fileBuffer;
    }

    return rModel;
}
