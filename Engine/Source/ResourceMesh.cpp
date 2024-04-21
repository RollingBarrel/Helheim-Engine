#include "ResourceMesh.h"

#include "Globals.h"
#include <assert.h>

ResourceMesh::ResourceMesh(
    unsigned int uid, 
    unsigned int inNumIndices, 
    const unsigned int* indices,
    unsigned int numJoints,
    const unsigned int* joints,
    unsigned int numWeights,
    const float* weights,
    unsigned int inNumVertices, 
    const std::vector<Attribute>& attributes, 
    const std::vector<float*>& attributesData) : Resource(uid, Type::Mesh),
    mNumVertices(inNumVertices), mNumIndices(inNumIndices), mIndices(new unsigned int[inNumIndices]), mJoints(new unsigned int[numJoints]), mNumJoints(numJoints), mWeights(new float[numWeights]), mNumWeights(numWeights), mVertexSize(0), mAttributes(attributes)
{
    memcpy(mIndices, indices, mNumIndices * sizeof(unsigned int));
    memcpy(mJoints, joints, mNumIndices * sizeof(unsigned int));
    memcpy(mWeights, weights, mNumIndices * sizeof(float));
    mAttributesData.reserve(attributesData.size());
    for (int i = 0; i < attributesData.size(); ++i)
    {
        float* newData = new float[mNumVertices * mAttributes[i].size / sizeof(float)];
        memcpy(newData, attributesData[i], mNumVertices * mAttributes[i].size);
        mAttributesData.push_back(newData);
    }
}

ResourceMesh::ResourceMesh(
    unsigned int uid,
    unsigned int inNumIndices,
    unsigned int*&& indices,
    unsigned int numJoints, 
    unsigned int*&& joints, 
    unsigned int numWeights, 
    float*&& weights,
    unsigned int inNumVertices,
    std::vector<Attribute>&& attributes,
    std::vector<float*>&& attributesData) : Resource(uid, Type::Mesh),
    mNumVertices(inNumVertices), mNumIndices(inNumIndices), mIndices(indices), mNumJoints(numJoints), mJoints(joints), mNumWeights(numWeights), mWeights(weights), mVertexSize(0), mAttributes(std::move(attributes)), mAttributesData(std::move(attributesData))
{
    indices = nullptr;
    joints = nullptr;
    weights = nullptr;
    for (Attribute attribute : mAttributes)
        mVertexSize += attribute.size;
}

ResourceMesh::ResourceMesh(const ResourceMesh& other): Resource(other.GetUID(), Type::Mesh),
    mNumVertices(other.mNumVertices), mNumIndices(other.mNumIndices), mIndices(new unsigned int[other.mNumIndices]), mNumJoints(other.mNumJoints), mJoints(new unsigned int[other.mNumJoints]), mNumWeights(other.mNumWeights), mWeights(new float[other.mNumWeights]), mVertexSize(other.mVertexSize), mAttributes(other.mAttributes)
{
    memcpy(mIndices, other.mIndices, mNumIndices * sizeof(unsigned int));
    memcpy(mJoints, other.mJoints, mNumJoints * sizeof(unsigned int));
    memcpy(mWeights, other.mWeights, mNumWeights * sizeof(float));
    mAttributesData.reserve(other.mAttributesData.size());
    for (int i = 0; i < other.mAttributesData.size(); ++i)
    {
        float* newData = new float[mNumVertices * mAttributes[i].size / sizeof(float)];
        memcpy(newData, other.mAttributesData[i], mNumVertices * mAttributes[i].size);
        mAttributesData.push_back(newData);
    }
}

ResourceMesh::ResourceMesh(ResourceMesh&& other) : Resource(other.GetUID(), Type::Mesh),
    mNumVertices(other.mNumVertices), mNumIndices(other.mNumIndices), mIndices(other.mIndices), mNumJoints(other.mNumJoints), mJoints(other.mJoints), mNumWeights(other.mNumWeights), mWeights(other.mWeights), mVertexSize(other.mVertexSize), mAttributes(std::move(other.mAttributes)), mAttributesData(std::move(other.mAttributesData))
{
    other.mIndices = nullptr;
    other.mJoints = nullptr;
    other.mWeights = nullptr;
    //TODO: Needed ??
    //mAttributesData.clear();
}

ResourceMesh::~ResourceMesh()
{
    if (mIndices != nullptr)
    {
        delete[] mIndices;
    }
    mAttributes.clear();
    for (std::vector<float*>::iterator it = mAttributesData.begin(); it != mAttributesData.end(); ++it)
    {
        if (*it != nullptr)
        {
            delete[] * it;
        }
    }
    mAttributesData.clear();
}

void ResourceMesh::GetAttributes(std::vector<Attribute>& attributes) const
{
    for (const Attribute attribute : mAttributes)
        attributes.push_back(attribute);
}

bool ResourceMesh::HasAttribute(Attribute::Type type) const
{
    for (Attribute attribute : mAttributes)
        if (attribute.type == type)
            return true;
    return false;
}

const float* ResourceMesh::GetAttributeData(Attribute::Type type) const
{
    unsigned int idx = 0;
    for (std::vector<Attribute>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
    {
        if (type == it->type)
            return mAttributesData[idx];
        ++idx;
    }
    return nullptr;
}

int ResourceMesh::GetAttributeIdx(Attribute::Type type) const
{
    unsigned int ret = 0;
    for (std::vector<Attribute>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
    {
        if (type == it->type)
        {
            return ret;
        }
        ++ret;
    }
    return -1;
}

float* ResourceMesh::GetInterleavedData() const
{
    float* ret = new float[mNumVertices * mVertexSize / sizeof(float)];
    unsigned int vertexFloats = mVertexSize / sizeof(float);
    for (unsigned int vertexIdx = 0; vertexIdx < mNumVertices; ++vertexIdx)
    {
        for (unsigned int attributeIdx = 0; attributeIdx < mAttributes.size(); ++attributeIdx)
        {
            unsigned int attributeSize = mAttributes[attributeIdx].size / sizeof(float);
            unsigned int attributeOffset = mAttributes[attributeIdx].offset / sizeof(float);
            const float* attributeData = mAttributesData[attributeIdx];
            memcpy(&ret[vertexIdx * vertexFloats + attributeOffset], &attributeData[vertexIdx * attributeSize], attributeSize * sizeof(float));
        }
    }
    return ret;
}