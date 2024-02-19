#include "ResourceMesh.h"

#include "ModuleFileSystem.h"

#include "mikktspace.h"
#include "weldmesh.h"

#include "glew.h"

ResourceMesh::ResourceMesh(
    unsigned int uid, 
    const char* path, 
    unsigned int numIndices,
    unsigned int numVertices, 
    unsigned int* indices) :
    mNumIndices(numIndices),
    mNumVertices(numVertices),
    mIndices(indices)
{
    mUID = uid;
    mType = Type::Mesh;

    mAssetsFile = path;
    mLibraryFile = LIBRARY_MESH_PATH + std::to_string(mUID) + ".mesh";
}

typedef struct {
    int numVertices;
    int posOffset;
    int texCoordOffset;
    int normOffset;
    int vertexSize;
    char* vertices;
    char* tVertices;
} MikkTSpaceStruct;

static int GetNumFaces(const SMikkTSpaceContext* pContext)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    return ptr->numVertices / 3;
}
static int GetNumVerticesOfFace(const SMikkTSpaceContext* pContext, const int iFace) {
    return 3;
}
static void GetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    float* posOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->posOffset];
    fvPosOut[0] = posOut[0];
    fvPosOut[1] = posOut[1];
    fvPosOut[2] = posOut[2];
}
static void GetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    float* texCOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->texCoordOffset];
    fvTexcOut[0] = texCOut[0];
    fvTexcOut[1] = texCOut[1];
}
static void GetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    float* normalOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->normOffset];
    fvNormOut[0] = normalOut[0];
    fvNormOut[1] = normalOut[1];
    fvNormOut[2] = normalOut[2];
}

static void SetTSpaceBasic(const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    //Escriure tota la info del vertex + les tangents
    const unsigned int vertexIdx = (iFace * 3 + iVert) * ptr->vertexSize;
    const unsigned int vertexTIdx = (iFace * 3 + iVert) * (ptr->vertexSize + sizeof(float) * 4);
    memcpy(&ptr->tVertices[vertexTIdx], &ptr->vertices[vertexIdx], ptr->vertexSize);
    memcpy(&ptr->tVertices[vertexTIdx + ptr->vertexSize], fvTangent, 3 * sizeof(float));
    memcpy(&ptr->tVertices[vertexTIdx + ptr->vertexSize + 3 * sizeof(float)], &fSign, sizeof(float));
}

void ResourceMesh::AddIndices(unsigned int* indices)
{
    mIndices = indices;
}

const float* ResourceMesh::GetAttributeData(Attribute::Type type) const
{
    unsigned int idx = 0;
    for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
    {
        if (type == (*it)->type)
            return mAttributesData[idx];
        ++idx;
    }
    return nullptr;
}

int ResourceMesh::GetAttributeIdx(Attribute::Type type) const
{
    unsigned int ret = 0;
    for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
    {
        if (type == (*it)->type)
        {
            return ret;
        }
        ++ret;
    }
    return -1;
}

void ResourceMesh::AddAttribute(const Attribute& attribute, float* attributeData)
{
    assert(attributeData != nullptr && "Adding null data to new attribute");
    int idx = GetAttributeIdx(attribute.type);
    if (idx >= 0)
    {
        //mVertexSize += mAttributes.back()->size;
        assert(attribute.size != mAttributes[idx]->size);
        unsigned int offset = mAttributes[idx]->offset;
        delete mAttributes[idx];
        mAttributes[idx] = new Attribute(attribute);
        mAttributes[idx]->offset = offset;
        delete[] mAttributesData[idx];
        mAttributesData[idx] = attributeData;
    }
    else
    {
        mAttributes.push_back(new Attribute(attribute));
        mAttributes.back()->offset = GetVertexSize();
        mAttributesData.push_back(attributeData);
        mVertexSize += mAttributes.back()->size;
    }
}

float* GetAttributeDataFromInterleavedBuffer(Attribute attr, float* interleavedBuffer, unsigned int bufferSize, unsigned int vertexSize)
{
    unsigned int numVertices = (bufferSize / vertexSize);
    unsigned int attributeNumFloats = (attr.size / sizeof(float));
    float* ret = new float[numVertices * attributeNumFloats];
    unsigned int floatOffset = attr.offset / sizeof(float);
    unsigned int j = 0;
    for (int i = 0; i < numVertices; ++i)
    {
        const float* vert = &reinterpret_cast<const float*>(interleavedBuffer)[i * vertexSize / sizeof(float) + floatOffset];
        for (unsigned int j = 0; j < attributeNumFloats; ++j)
        {
            ret[i * attributeNumFloats + j] = vert[j];
            //LOG("%f", ret[i * attributeNumFloats + j]);
        }

    }
    return ret;
}

bool ResourceMesh::LoadInterleavedAttribute(float* fillBuffer, const Attribute& attribute, unsigned int vertexSize) const
{
    unsigned int idx = GetAttributeIdx(attribute.type);
    if (idx < 0)
        return false;
    const Attribute& myAttribute = *mAttributes[idx];
    assert(attribute.size == myAttribute.size);
    unsigned int j = 0;
    for (int i = 0; i < mVertexSize * mNumVertices; i += vertexSize)
    {
        memcpy(&fillBuffer[(i + attribute.offset) / sizeof(float)], &((mAttributesData[idx])[j]), myAttribute.size);
        j += myAttribute.size / sizeof(float);
    }
    return true;
}

float* ResourceMesh::GetInterleavedData() const
{
    float* ret = new float[mNumVertices * GetVertexSize() / sizeof(float)];
    for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
    {
        LoadInterleavedAttribute(ret, *(*it), GetVertexSize());
    }
    return ret;
}

unsigned int ResourceMesh::LoadToMemory()
{
    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);
    glBindVertexArray(mVao);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, mNumVertices * mVertexSize, GetInterleavedData(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(unsigned int), mIndices, GL_STATIC_DRAW);
    unsigned int idx = 0;
    for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
    {
        glVertexAttribPointer(idx, (*it)->size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it)->offset);
        glEnableVertexAttribArray(idx);
        ++idx;
    }
    glBindVertexArray(0);
    return mVao;
}

void ResourceMesh::UnloadFromMemory()
{
    glDeleteBuffers(1, &mVbo);
    glDeleteBuffers(1, &mEbo);
    glDeleteVertexArrays(1, &mVao);
}


void ResourceMesh::GenerateTangents()
{
    if (GetAttributeIdx(Attribute::POS) < 0 || GetAttributeIdx(Attribute::UV) < 0 || GetAttributeIdx(Attribute::NORMAL) < 0)
        return;

    unsigned int* indices = reinterpret_cast<unsigned int*>(mIndices);
    const char* vertices = reinterpret_cast<const char*>(GetInterleavedData());
    char* unweldedVertices = new char[mNumIndices * GetVertexSize()];

    for (int i = 0; i < mNumIndices; ++i)
    {
        memcpy(&unweldedVertices[i * GetVertexSize()], &vertices[indices[i] * GetVertexSize()], GetVertexSize());
    }

    SMikkTSpaceInterface interfaceInput = {};
    interfaceInput.m_getNumFaces = GetNumFaces;
    interfaceInput.m_getNumVerticesOfFace = GetNumVerticesOfFace;
    interfaceInput.m_getNormal = GetNormal;
    interfaceInput.m_getPosition = GetPosition;
    interfaceInput.m_getTexCoord = GetTexCoord;
    interfaceInput.m_setTSpaceBasic = SetTSpaceBasic;
    MikkTSpaceStruct mikkInput = {};
    mikkInput.numVertices = mNumIndices;
    mikkInput.posOffset = 0;
    mikkInput.texCoordOffset = 3 * sizeof(float);
    mikkInput.normOffset = 5 * sizeof(float);
    mikkInput.vertexSize = 8 * sizeof(float);
    mikkInput.vertices = unweldedVertices;
    //Les mikktangents son vec4
    char* unweldedTVertices = new char[mNumIndices * (mVertexSize + 4 * sizeof(float))];
    mikkInput.tVertices = unweldedTVertices;
    SMikkTSpaceContext tangContext = {};
    tangContext.m_pInterface = &interfaceInput;
    tangContext.m_pUserData = &mikkInput;
    if (!genTangSpaceDefault(&tangContext))
    {
        //LOG("ERROR: Could not generate the tangent space");
    }

    int* piRemapTable = new int[mikkInput.numVertices];
    float* pfVertexDataOut = new float[mikkInput.numVertices * 12];
    unsigned int uniqueVertices = WeldMesh(piRemapTable, pfVertexDataOut, reinterpret_cast<float*>(mikkInput.tVertices), mikkInput.numVertices, 12);
    delete[] unweldedTVertices;
    delete[] unweldedVertices;

    CleanUp();
    mNumVertices = uniqueVertices;
    mNumIndices = mikkInput.numVertices;
    Attribute newAttribute = Attribute(Attribute::POS, sizeof(float) * 3, 0);
    AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    newAttribute = Attribute(Attribute::UV, sizeof(float) * 2, sizeof(float) * 3);
    AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    newAttribute = Attribute(Attribute::NORMAL, sizeof(float) * 3, sizeof(float) * 5);
    AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    newAttribute = Attribute(Attribute::TANGENT, sizeof(float) * 4, sizeof(float) * 8);
    AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));

    mNumIndices = mikkInput.numVertices;
    mIndices = reinterpret_cast<unsigned int*>(piRemapTable);
    delete[] pfVertexDataOut;
}

void ResourceMesh::CleanUp()
{
    mNumVertices = 0;
    mNumIndices = 0;
    if (mIndices != nullptr)
    {
        delete[] mIndices;
        mIndices = nullptr;
    }
    for (std::vector<Attribute*>::iterator it = mAttributes.begin(); it != mAttributes.end(); ++it)
    {
        if (*it != nullptr)
        {
            delete* it;
            *it = nullptr;
        }
    }
    mAttributes.clear();
    for (std::vector<float*>::iterator it = mAttributesData.begin(); it != mAttributesData.end(); ++it)
    {
        if (*it != nullptr)
        {
            delete[] * it;
            *it = nullptr;
        }
    }
    mAttributesData.clear();

    //TODO: delete EBO/VBO...
    UnloadFromMemory();
}