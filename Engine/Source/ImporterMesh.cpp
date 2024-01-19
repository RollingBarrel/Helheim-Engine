#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterMesh.h"

#include "glew.h"

#include "float2.h"
#include "float3.h"
#include "float4.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

#include "mikktspace.h"
#include "weldmesh.h"

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

const float* ResourceMesh::GetAttributData(Attribute::Type type) const
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
    unsigned int idx = GetAttributeIdx(attribute.type);
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
        mAttributesData.push_back(attributeData);
        mAttributes[idx]->offset = GetVertexSize();
        mVertexSize += mAttributes.back()->size;
    }
}

//ResourceMesh::ResourceMesh(const ResourceMesh& other): mNumVertices(other.mNumVertices), mNumIndices(other.mNumIndices), mMeshName(new char[strlen(other.mMeshName)+1]), mUID(other.mUID), mVao(other.mVao), mVbo(other.mVbo), mEbo(other.mEbo)
//{
//    strcpy(const_cast<char*>(mMeshName), other.mMeshName);
//    unsigned int idx = 0;
//    for (std::vector<Attribute*>::const_iterator it = other.mAttributes.cbegin(); it != other.mAttributes.cend(); ++it)
//    {
//        const Attribute& attribute = *(*it);
//        mAttributes.push_back(new Attribute(attribute));
//        mAttributesData.push_back(new float[mNumVertices * attribute.size/sizeof(float)]);
//    }
//}

float* GetAttributeDataFromInterleavedBuffer(Attribute attr, float* interleavedBuffer, unsigned int bufferSize, unsigned int vertexSize)
{
    float* ret = new float[(bufferSize / vertexSize) / sizeof(float)];
    int j = 0;
    for (int i = 0; i < bufferSize; i += vertexSize)
    {
        memcpy(&ret[j], &interleavedBuffer[i + attr.offset], attr.size);
        j += attr.size / sizeof(float);
    }
    return ret;
}

static void GenerateTangents(ResourceMesh*& rMesh)
{
    //assert(rMesh->AttributeIdx(Attribute::POS) >= 0 && "No positions to generate tangents");
    //assert(rMesh->AttributeIdx(Attribute::UV) >= 0 && "No texture coords to generate tangents");
    //assert(rMesh->AttributeIdx(Attribute::NORMAL) >= 0 && "No normals to generate tangents");
    if (rMesh->GetAttributeIdx(Attribute::POS) < 0 || rMesh->GetAttributeIdx(Attribute::UV) < 0 || rMesh->GetAttributeIdx(Attribute::NORMAL) < 0)
        return;

    unsigned int* indices = reinterpret_cast<unsigned int*>(rMesh->mIndices);
    const char* vertices = reinterpret_cast<const char*>(rMesh->GetInterleavedData());
    char* unweldedVertices = new char[rMesh->mNumIndices * rMesh->GetVertexSize()];

    for (int i = 0; i < rMesh->mNumIndices; ++i)
    {
        memcpy(&unweldedVertices[i * rMesh->GetVertexSize()], &vertices[indices[i] * rMesh->GetVertexSize()], rMesh->GetVertexSize());
    }
    
    SMikkTSpaceInterface interfaceInput = {};
    interfaceInput.m_getNumFaces = GetNumFaces;
    interfaceInput.m_getNumVerticesOfFace = GetNumVerticesOfFace;
    interfaceInput.m_getNormal = GetNormal;
    interfaceInput.m_getPosition = GetPosition;
    interfaceInput.m_getTexCoord = GetTexCoord;
    interfaceInput.m_setTSpaceBasic = SetTSpaceBasic;
    MikkTSpaceStruct mikkInput = {};
    mikkInput.numVertices = rMesh->mNumIndices;
    mikkInput.posOffset = 0;
    mikkInput.texCoordOffset = 3 * sizeof(float);
    mikkInput.normOffset = 5 * sizeof(float);
    mikkInput.vertexSize = 8 * sizeof(float);
    mikkInput.vertices = unweldedVertices;
    //Les mikktangents son vec4
    char* unweldedTVertices = new char[rMesh->mNumIndices * (rMesh->GetVertexSize() + 4 * sizeof(float))];
    mikkInput.tVertices = unweldedTVertices;
    SMikkTSpaceContext tangContext = {};
    tangContext.m_pInterface = &interfaceInput;
    tangContext.m_pUserData = &mikkInput;
    if (!genTangSpaceDefault(&tangContext))
        LOG("ERROR: Could not generate the tangent space");

    int* piRemapTable = new int[mikkInput.numVertices];
    float* pfVertexDataOut = new float[mikkInput.numVertices * 12];
    unsigned int uniqueVertices = WeldMesh(piRemapTable, pfVertexDataOut, reinterpret_cast<float*>(mikkInput.tVertices), mikkInput.numVertices, 12);
    delete[] unweldedTVertices;
    delete[] unweldedVertices;
    
    ResourceMesh* retMesh = new ResourceMesh();
    Attribute newAttribute = Attribute(Attribute::POS, sizeof(float) * 3, 0);
    retMesh->AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, mikkInput.numVertices * 12* sizeof(float), 12*sizeof(float)));
    newAttribute = Attribute(Attribute::UV, sizeof(float) * 2, sizeof(float) * 3);
    retMesh->AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, mikkInput.numVertices * 12 * sizeof(float), 12 * sizeof(float)));
    newAttribute = Attribute(Attribute::NORMAL, sizeof(float) * 3, sizeof(float) * 5);
    retMesh->AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, mikkInput.numVertices * 12 * sizeof(float), 12 * sizeof(float)));
    newAttribute = Attribute(Attribute::TANGENT, sizeof(float) * 4, sizeof(float) * 8);
    retMesh->AddAttribute(newAttribute, GetAttributeDataFromInterleavedBuffer(newAttribute, pfVertexDataOut, mikkInput.numVertices * 12 * sizeof(float), 12 * sizeof(float)));

    delete rMesh;
    rMesh = retMesh;

    retMesh->mNumIndices = mikkInput.numVertices;
    retMesh->mIndices = reinterpret_cast<unsigned int*>(piRemapTable);
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
    for (std::vector<float*>::iterator it = mAttributesData.begin(); it != mAttributesData.end(); ++it)
    {
        delete[] *it;
    }
    for (std::vector<Attribute*>::iterator it = mAttributes.begin(); it != mAttributes.end(); ++it)
    {
        delete *it;
    }

    //TODO: delete EBO/VBO...
    mAttributes.clear();
    mAttributesData.clear();
    glDeleteBuffers(1, &mVbo);
    glDeleteBuffers(1, &mEbo);
    glDeleteVertexArrays(1, &mVao);
}

void Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* mesh)
{
    
    const auto& itPos = primitive.attributes.find("POSITION");
    const auto& itTexCoord = primitive.attributes.find("TEXCOORD_0");
    const auto& itNorm = primitive.attributes.find("NORMAL");
    const auto& itTang = primitive.attributes.find("TANGENT");

    unsigned int attrOffset = 0;
    if (itPos != primitive.attributes.end())
    {
        const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
        assert(posAcc.type == TINYGLTF_TYPE_VEC3);
        assert(posAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

        posBuffer.uri;

        //App->GetFileSystem()->Copy();

        mesh->mAttributes.push_back(new Attribute(Attribute::POS, sizeof(float)*3, attrOffset));
        attrOffset += sizeof(float) * 3;

        mesh->mNumVertices = posAcc.count;

        const unsigned char* bufferPos = &posBuffer.data[posView.byteOffset + posAcc.byteOffset];


        //Add vertices Pos to this buffer taking into acc byteStride
        mesh->mAttributesData.push_back(reinterpret_cast<float*>(const_cast<unsigned char*>(bufferPos)));
        for (auto i = 0; i < posAcc.count; ++i)
        {
            reinterpret_cast<float3*>(mesh->mAttributesData.back())[i] = *reinterpret_cast<const float3*>(bufferPos);

            if (posView.byteStride != 0) {
                bufferPos += posView.byteStride;
            }
            else {
                bufferPos += sizeof(float) * 3;
            }

            LOG("%f %f %f", reinterpret_cast<float3*>(mesh->mAttributesData.back())[i].x, reinterpret_cast<float3*>(mesh->mAttributesData.back())[i].y, reinterpret_cast<float3*>(mesh->mAttributesData.back())[i].z);
        }

    }

    if (itTexCoord != primitive.attributes.end())
    {
        const tinygltf::Accessor& texCoordAcc = model.accessors[itTexCoord->second];
        assert(texCoordAcc.type == TINYGLTF_TYPE_VEC2);
        assert(texCoordAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAcc.bufferView];
        const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];

        const unsigned char* bufferTexCoord = &texCoordBuffer.data[texCoordView.byteOffset + texCoordAcc.byteOffset];


        mesh->mAttributes.push_back(new Attribute(Attribute::UV, sizeof(float) * 2, attrOffset));
        attrOffset += sizeof(float) * 2;
        //Add vertices TexCoord to this buffer taking into acc byteStride
        mesh->mAttributesData.back() = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferTexCoord));
        for (auto i = 0; i < texCoordAcc.count; ++i)
        {
            reinterpret_cast<float2*>(mesh->mAttributesData.back())[i] = *reinterpret_cast<const float2*>(bufferTexCoord);

            if (texCoordView.byteStride != 0)
            {
                bufferTexCoord += texCoordView.byteStride;
            }
            else
            {
                bufferTexCoord += sizeof(float) * 2;
            }

            LOG("%f %f", reinterpret_cast<float2*>(mesh->mAttributesData.back())[i].x, reinterpret_cast<float2*>(mesh->mAttributesData.back())[i].y);
        }
    }

    if (itNorm != primitive.attributes.end())
    {
        const tinygltf::Accessor& normAcc = model.accessors[itNorm->second];
        assert(normAcc.type == TINYGLTF_TYPE_VEC3);
        assert(normAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& normView = model.bufferViews[normAcc.bufferView];
        const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];

        const unsigned char* bufferNorm = &normBuffer.data[normView.byteOffset + normAcc.byteOffset];

        mesh->mAttributes.push_back(new Attribute(Attribute::NORMAL, sizeof(float) * 3, attrOffset));
        attrOffset += sizeof(float) * 3;

        //Add vertices Normal to this buffer taking into acc byteStride
        mesh->mAttributesData.back() = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferNorm));

        for (auto i = 0; i < normAcc.count; ++i)
        {
            reinterpret_cast<float3*>(mesh->mAttributesData.back())[i] = *reinterpret_cast<const float3*>(bufferNorm);

            if (normView.byteStride != 0)
            {
                bufferNorm += normView.byteStride;
            }
            else
            {
                bufferNorm += sizeof(float) * 3;
            }

            LOG("%f %f %f", reinterpret_cast<float3*>(mesh->mAttributesData.back())[i].x, reinterpret_cast<float3*>(mesh->mAttributesData.back())[i].y, reinterpret_cast<float3*>(mesh->mAttributesData.back())[i].z);
        }

    }

    //Indices part
    if (primitive.indices >= 0)
    {
        const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
        mesh->mNumIndices = indAcc.count;
        const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
        const unsigned char* buffer = &(model.buffers[indView.buffer].data[indAcc.byteOffset + indView.byteOffset]);

        mesh->mIndices = reinterpret_cast<unsigned int*>(const_cast<unsigned char*>(buffer));

        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
        {
            const uint32_t* bufferInd = reinterpret_cast<const uint32_t*>(buffer);
            for (uint32_t i = 0; i < indAcc.count; ++i)
            {
                mesh->mIndices[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
        {
            const uint16_t* bufferInd = reinterpret_cast<const uint16_t*>(buffer);
            for (uint16_t i = 0; i < indAcc.count; ++i)
            {
                mesh->mIndices[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
        {
            const uint8_t* bufferInd = reinterpret_cast<const uint8_t*>(buffer);
            for (uint8_t i = 0; i < indAcc.count; ++i)
            {
                mesh->mIndices[i] = bufferInd[i];
            }
        }
    }

    if (itTang != primitive.attributes.end())
    {
        const tinygltf::Accessor& tangAcc = model.accessors[itTang->second];
        assert(tangAcc.type == TINYGLTF_TYPE_VEC4);
        assert(tangAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& tangView = model.bufferViews[tangAcc.bufferView];
        const tinygltf::Buffer& tangBuffer = model.buffers[tangView.buffer];

        const unsigned char* bufferTang = &tangBuffer.data[tangView.byteOffset + tangAcc.byteOffset];

        mesh->mAttributes.push_back(new Attribute(Attribute::TANGENT, sizeof(float) *4, attrOffset));
        attrOffset += sizeof(float) * 4;

        //Add vertices Tangent to this buffer taking into acc byteStride
        mesh->mAttributesData.back() = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferTang));

        for (auto i = 0; i < tangAcc.count; ++i)
        {
            reinterpret_cast<float4*>(mesh->mAttributesData.back())[i] = *reinterpret_cast<const float4*>(bufferTang);

            if (tangView.byteStride != 0)
            {
                bufferTang += tangView.byteStride;
            }
            else
            {
                bufferTang += sizeof(float) * 4;
            }

            LOG("%f %f %f", reinterpret_cast<float4*>(mesh->mAttributesData.back())[i].x, reinterpret_cast<float4*>(mesh->mAttributesData.back())[i].y, reinterpret_cast<float4*>(mesh->mAttributesData.back())[i].z);
        }
        mesh->mVertexSize = attrOffset;
    }
    else
    {
        //Generate Tangents
        GenerateTangents(mesh);
    }
    //TODO: es necessari???
    attrOffset += sizeof(float) * 4;
    mesh->mVertexSize = attrOffset;

    //Posar la vertex size
    mesh->mVertexSize = attrOffset;

    Mesh::Save(mesh);

    //char* fileBuffer = nullptr;
    //ResourceMesh loadedMesh;
    //Mesh::Load(fileBuffer, &loadedMesh, mesh->mMeshName);
}

void Importer::Mesh::Save(const ResourceMesh* mesh)
{
    unsigned int header[3] = { mesh->mNumIndices, mesh->mNumVertices, mesh->GetVertexSize() };

    unsigned int size = sizeof(header) + 
                        sizeof(unsigned int) * mesh->mNumIndices +          
                        sizeof(float) * mesh->mNumVertices * 3 +
                        sizeof(float) * mesh->mNumVertices * 2 +
                        sizeof(float) * mesh->mNumVertices * 3 +
                        sizeof(float) * mesh->mNumVertices * 4;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    //Save Header
    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;
    //Save Indices
    bytes = sizeof(unsigned int) * mesh->mNumIndices;
    memcpy(cursor, mesh->mIndices, bytes);
    cursor += bytes;
    //Save Positions
    bytes = sizeof(float) * mesh->mNumVertices * 3;
    memcpy(cursor, mesh->mVerticesPosition, bytes);
    cursor += bytes;
    //Save TexCoords
    assert(mesh->mVerticesTextureCoordinate != nullptr);
    bytes = sizeof(float) * mesh->mNumVertices * 2;
    memcpy(cursor, mesh->mVerticesTextureCoordinate, bytes);
    cursor += bytes;
    //Save Normals
    assert(mesh->mVerticesNormal != nullptr);
    bytes = sizeof(float) * mesh->mNumVertices * 3;
    memcpy(cursor, mesh->mVerticesNormal, bytes);
    cursor += bytes;
    //Save Tangents
    assert(mesh->mVerticesTangent != nullptr);
    bytes = sizeof(float) * mesh->mNumVertices * 3;
    memcpy(cursor, mesh->mVerticesTangent, bytes);
    cursor += bytes;

    std::string path = LIBRARY_MESH_PATH;
    path += mesh->mMeshName;
    path += ".messhi";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;
}

void Importer::Mesh::Load(char* fileBuffer, ResourceMesh* mesh, const char* fileName)
{
    std::string path = LIBRARY_MESH_PATH;
    path += fileName;
    path += ".messhi";

    App->GetFileSystem()->Load(path.c_str(), &fileBuffer);

    //Load Header
    char* cursor = fileBuffer;
    unsigned int header[2];
    unsigned int bytes = sizeof(header);
    memcpy(header, cursor, bytes);
    cursor += bytes;
    mesh->mNumIndices = header[0];
    mesh->mNumVertices = header[1];
    mesh->mVertexSize = header[2];
    //Load Indices
    bytes = sizeof(unsigned int) * mesh->mNumIndices;
    mesh->mIndices = new unsigned int[mesh->mNumIndices];
    memcpy(mesh->mIndices, cursor, bytes);
    //Load Positions
    bytes = sizeof(float) * mesh->mNumVertices * 3;
    mesh->mVerticesPosition = new float[mesh->mNumVertices * 3];
    memcpy(mesh->mVerticesPosition, cursor, bytes);
    //Save TexCoords
    bytes = sizeof(float) * mesh->mNumVertices * 2;
    mesh->mVerticesTextureCoordinate = new float[mesh->mNumVertices * 2];
    memcpy(mesh->mVerticesTextureCoordinate, cursor, bytes);
    //Save Normals
    bytes = sizeof(float) * mesh->mNumVertices * 3;
    mesh->mVerticesNormal = new float[mesh->mNumVertices * 3];
    memcpy(mesh->mVerticesNormal, cursor, bytes);
    //Save Tangents
    bytes = sizeof(float) * mesh->mNumVertices * 4;
    mesh->mVerticesTangent = new float[mesh->mNumVertices * 4];
    memcpy(mesh->mVerticesTangent, cursor, bytes);

    mesh->LoadVBO();
    mesh->LoadEBO();
    mesh->LoadVAO();

    //Create GameObject and set mesh to it;
}

bool ResourceMesh::LoadInterleavedAttribute(float* fillBuffer, const Attribute& attribute, unsigned int vertexSize) const
{
    unsigned int idx = GetAttributeIdx(attribute.type);
    if (idx < 0)
        return false;
    const Attribute& myAttribute = *mAttributes[idx];
    assert(attribute.size != myAttribute.size);
    unsigned int j = 0;
    for (int i = 0; i < mVertexSize * mNumVertices; i += vertexSize)
    {
        memcpy(&fillBuffer[(i + attribute.offset)/sizeof(float)], &((mAttributesData[idx])[j]), myAttribute.size);
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

void ResourceMesh::LoadVAO()
{
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);
    LOG("[MESH] Creting VAO %u", mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);

    //TODO: RECALCULATE OFFSET IF DONT HAVE ATTRIBUTES
    unsigned offset = 0;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(offset));

    glBindVertexArray(0);
}

void ResourceMesh::LoadVBO()
{
    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mNumVertices * 3, nullptr, GL_STATIC_DRAW);
    LOG("[MESH] Creating VBO %u, reserved %u memory", mVbo, (sizeof(float) * mNumVertices * 3));

    assert(mVerticesPosition != nullptr);
    float* ptr = reinterpret_cast<float*>(reinterpret_cast<char*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
    for (auto i = 0; i < mNumVertices; ++i)
    {
        ptr[i] = mVerticesPosition[i];
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    LOG("[MESH] Finish VBO map buffer");
}

void ResourceMesh::LoadEBO()
{
    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mNumIndices, nullptr, GL_STATIC_DRAW);
    LOG("[MESH] Creating EBO %u, reserved %u memory", mEbo, (sizeof(unsigned int) * mNumIndices));

    unsigned int* ptr = reinterpret_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
    for (auto i = 0; i < mNumIndices; ++i)
    {
        ptr[i] = mIndices[i];
    }
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    LOG("[MESH] Finish EBO map buffer");
}
