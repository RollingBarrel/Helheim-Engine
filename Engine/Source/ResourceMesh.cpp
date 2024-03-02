#include "ResourceMesh.h"

#include "Globals.h"
#include <assert.h>

ResourceMesh::ResourceMesh(
    unsigned int uid, 
    unsigned int inNumIndices, 
    const unsigned int* indices, 
    unsigned int inNumVertices, 
    const std::vector<Attribute>& attributes, 
    const std::vector<float*>& attributesData) : Resource(uid, Type::Mesh),
    mNumVertices(inNumVertices), mNumIndices(inNumIndices), mIndices(new unsigned int[inNumIndices]), mVertexSize(0), mAttributes(attributes)
{
    memcpy(mIndices, indices, mNumIndices * sizeof(unsigned int));
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
    unsigned int inNumVertices,
    std::vector<Attribute>&& attributes,
    std::vector<float*>&& attributesData) : Resource(uid, Type::Mesh),
    mNumVertices(inNumVertices), mNumIndices(inNumIndices), mIndices(indices), mVertexSize(0), mAttributes(attributes), mAttributesData(attributesData)
{
    indices = nullptr;
    for (Attribute attribute : mAttributes)
        mVertexSize += attribute.size;
}

ResourceMesh::ResourceMesh(const ResourceMesh& other): Resource(other.GetUID(), Type::Mesh),
    mNumVertices(other.mNumVertices), mNumIndices(other.mNumIndices), mIndices(new unsigned int[other.mNumIndices]), mVertexSize(other.mVertexSize), mAttributes(other.mAttributes)
{
    memcpy(mIndices, other.mIndices, mNumIndices * sizeof(unsigned int));
    mAttributesData.reserve(other.mAttributesData.size());
    for (int i = 0; i < other.mAttributesData.size(); ++i)
    {
        float* newData = new float[mNumVertices * mAttributes[i].size / sizeof(float)];
        memcpy(newData, other.mAttributesData[i], mNumVertices * mAttributes[i].size);
        mAttributesData.push_back(newData);
    }
}

ResourceMesh::ResourceMesh(ResourceMesh&& other) : Resource(other.GetUID(), Type::Mesh),
    mNumVertices(other.mNumVertices), mNumIndices(other.mNumIndices), mIndices(new unsigned int[other.mNumIndices]), mVertexSize(other.mVertexSize), mAttributes(other.mAttributes), mAttributesData(other.mAttributesData)
{
    other.mIndices = nullptr;
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

bool ResourceMesh::LoadInterleavedAttribute(float* interleavedBuffer, const Attribute& attribute, unsigned int vertexSize) const
{
    assert(interleavedBuffer && "Passed a nullptr buffer");
    unsigned int idx = GetAttributeIdx(attribute.type);
    if (idx < 0)
        return false;
    const Attribute& myAttribute = mAttributes[idx];
    assert(attribute.size == myAttribute.size);
    unsigned int j = 0;
    for (int i = 0; i < mVertexSize * mNumVertices; i += vertexSize)
    {
        memcpy(&interleavedBuffer[(i + attribute.offset) / sizeof(float)], &((mAttributesData[idx])[j]), myAttribute.size);
        j += myAttribute.size / sizeof(float);
    }
    return true;
}

float* ResourceMesh::GetInterleavedData() const
{
    float* ret = new float[mNumVertices * mVertexSize / sizeof(float)];
    for (std::vector<Attribute>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
    {
        LoadInterleavedAttribute(ret, *it, mVertexSize);
    }
    return ret;
}