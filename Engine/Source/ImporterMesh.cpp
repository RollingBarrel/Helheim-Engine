#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterMesh.h"
#include "ResourceMesh.h"

#include "glew.h"
#include "float2.h"
#include "float4.h"


#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"



static float* GetInterleavedData(const std::vector<Attribute>& attributes, const std::vector<float*>& attributesData, unsigned int numVertices)
{
    unsigned int vertexSize = 0;
    for (int i = 0; i < attributes.size(); ++i)
    {
        vertexSize += attributes[i].size;
    }
    float* ret = new float[numVertices * vertexSize / sizeof(float)];
    unsigned int vertexFloats = vertexSize / sizeof(float);
    for (unsigned int vertexIdx = 0; vertexIdx < numVertices; ++vertexIdx)
    {
        for (unsigned int attributeIdx = 0; attributeIdx < attributes.size(); ++attributeIdx)
        {
            unsigned int attributeSize = attributes[attributeIdx].size / sizeof(float);
            unsigned int attributeOffset = attributes[attributeIdx].offset / sizeof(float);
            const float* attributeData = attributesData[attributeIdx];
            memcpy(&ret[vertexIdx * vertexFloats + attributeOffset], &attributeData[vertexIdx * attributeSize], attributeSize * sizeof(float));
        }
    }
    return ret;
}


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

static float* GetAttributeDataFromInterleavedBuffer(Attribute attr, float* interleavedBuffer, unsigned int bufferSize, unsigned int vertexSize)
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

static void GenerateTangents(std::vector<Attribute>& attributes, std::vector<float*>& attributeData, unsigned int& numIndices, unsigned int*& indexData, unsigned int& vertexSize, unsigned int& numVertices, float* vertexData)
{
#ifdef _DEBUG
    bool foundPos = false;
    bool foundUv = false;
    bool foundNormal = false;
    for (const Attribute attribute : attributes)
    {
        switch (attribute.type)
        {
        case Attribute::POS:
            foundPos = true;
            break;
        case Attribute::UV:
            foundUv = true;
            break;
        case Attribute::NORMAL:
            foundNormal = true;
            break;
        }
        if (foundPos && foundUv && foundNormal)
            break;
    }
    if (!foundPos || !foundUv || !foundNormal)
        return;
#endif // _DEBUG

    const char* vertices = reinterpret_cast<const char*>(vertexData);
    char* unweldedVertices = new char[numIndices * vertexSize];

    for (int i = 0; i < numIndices; ++i)
    {
        memcpy(&unweldedVertices[i * vertexSize], &vertices[indexData[i] * vertexSize], vertexSize);
    }

    SMikkTSpaceInterface interfaceInput = {};
    interfaceInput.m_getNumFaces = GetNumFaces;
    interfaceInput.m_getNumVerticesOfFace = GetNumVerticesOfFace;
    interfaceInput.m_getNormal = GetNormal;
    interfaceInput.m_getPosition = GetPosition;
    interfaceInput.m_getTexCoord = GetTexCoord;
    interfaceInput.m_setTSpaceBasic = SetTSpaceBasic;
    MikkTSpaceStruct mikkInput = {};
    mikkInput.numVertices = numIndices;
    mikkInput.posOffset = 0;
    mikkInput.texCoordOffset = 3 * sizeof(float);
    mikkInput.normOffset = 5 * sizeof(float);
    mikkInput.vertexSize = 8 * sizeof(float);
    mikkInput.vertices = unweldedVertices;
    //Les mikktangents son vec4
    char* unweldedTVertices = new char[numIndices * (vertexSize + 4 * sizeof(float))];
    mikkInput.tVertices = unweldedTVertices;
    SMikkTSpaceContext tangContext = {};
    tangContext.m_pInterface = &interfaceInput;
    tangContext.m_pUserData = &mikkInput;
    if (!genTangSpaceDefault(&tangContext))
    {
        LOG("ERROR: Could not generate the tangent space");
        delete[] unweldedTVertices;
        delete[] unweldedVertices;
        return;
    }

    int* piRemapTable = new int[mikkInput.numVertices];
    float* pfVertexDataOut = new float[mikkInput.numVertices * 12];
    unsigned int uniqueVertices = WeldMesh(piRemapTable, pfVertexDataOut, reinterpret_cast<float*>(mikkInput.tVertices), mikkInput.numVertices, 12);
    delete[] unweldedTVertices;
    delete[] unweldedVertices;

    delete[] indexData;
    delete[] vertexData;
    attributes.clear();
    for (float* data : attributeData)
        delete[] data;
    attributeData.clear();

    numVertices = uniqueVertices;
    numIndices = mikkInput.numVertices;
    attributes.emplace_back(Attribute::POS, sizeof(float) * 3, 0);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    attributes.emplace_back(Attribute::UV, sizeof(float) * 2, sizeof(float) * 3);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    attributes.emplace_back(Attribute::NORMAL, sizeof(float) * 3, sizeof(float) * 5);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    attributes.emplace_back(Attribute::TANGENT, sizeof(float) * 4, sizeof(float) * 8);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));

    numIndices = mikkInput.numVertices;
    indexData = reinterpret_cast<unsigned int*>(piRemapTable);
    delete[] pfVertexDataOut;
}

ResourceMesh* Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, unsigned int uid)
{

    unsigned int numVertices = 0;
    unsigned int numIndices = 0;
    unsigned int vertexSize = 0;
    unsigned int* indices = nullptr;
    std::vector<Attribute>attributes;
    std::vector<float*>attributesData;

    const auto& itPos = primitive.attributes.find("POSITION");
    const auto& itTexCoord = primitive.attributes.find("TEXCOORD_0");
    const auto& itNorm = primitive.attributes.find("NORMAL");
    const auto& itTang = primitive.attributes.find("TANGENT");

    if (itPos != primitive.attributes.end())
    {
        const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
        assert(posAcc.type == TINYGLTF_TYPE_VEC3);
        assert(posAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

        numVertices = posAcc.count;

        const unsigned char* bufferPos = &posBuffer.data[posView.byteOffset + posAcc.byteOffset];

        attributes.emplace_back(Attribute::POS, sizeof(float) * 3, vertexSize);
        vertexSize += sizeof(float) * 3;
        float* data = new float[posAcc.count * 3];
        //Add vertices Pos to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < posAcc.count; ++i)
        {
            reinterpret_cast<float3*>(data)[i] = *reinterpret_cast<const float3*>(bufferPos);

            if (posView.byteStride != 0) {
                bufferPos += posView.byteStride;
            }
            else {
                bufferPos += sizeof(float) * 3;
            }

            //LOG("%f %f %f", reinterpret_cast<float3*>(data)[i].x, reinterpret_cast<float3*>(data)[i].y, reinterpret_cast<float3*>(data)[i].z);
        }
        attributesData.push_back(data);
    }

    if (itTexCoord != primitive.attributes.end())
    {
        const tinygltf::Accessor& texCoordAcc = model.accessors[itTexCoord->second];
        assert(texCoordAcc.type == TINYGLTF_TYPE_VEC2);
        assert(texCoordAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAcc.bufferView];
        const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];

        const unsigned char* bufferTexCoord = &texCoordBuffer.data[texCoordView.byteOffset + texCoordAcc.byteOffset];

        attributes.emplace_back(Attribute::UV, sizeof(float) * 2, vertexSize);
        vertexSize += sizeof(float) * 2;
        float* data = new float[texCoordAcc.count * 2];
        //Add vertices TexCoord to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < texCoordAcc.count; ++i)
        {
            reinterpret_cast<float2*>(data)[i] = *reinterpret_cast<const float2*>(bufferTexCoord);

            if (texCoordView.byteStride != 0)
            {
                bufferTexCoord += texCoordView.byteStride;
            }
            else
            {
                bufferTexCoord += sizeof(float) * 2;
            }

            //LOG("%f %f", reinterpret_cast<float2*>(data)[i].x, reinterpret_cast<float2*>(data)[i].y);
        }
        attributesData.push_back(data);
    }

    if (itNorm != primitive.attributes.end())
    {
        const tinygltf::Accessor& normAcc = model.accessors[itNorm->second];
        assert(normAcc.type == TINYGLTF_TYPE_VEC3);
        assert(normAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& normView = model.bufferViews[normAcc.bufferView];
        const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];

        const unsigned char* bufferNorm = &normBuffer.data[normView.byteOffset + normAcc.byteOffset];

        attributes.emplace_back(Attribute::NORMAL, sizeof(float) * 3, vertexSize);
        vertexSize += sizeof(float) * 3;
        float* data = new float[normAcc.count * 3];

        //Add vertices Normal to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < normAcc.count; ++i)
        {
            reinterpret_cast<float3*>(data)[i] = *reinterpret_cast<const float3*>(bufferNorm);

            if (normView.byteStride != 0)
            {
                bufferNorm += normView.byteStride;
            }
            else
            {
                bufferNorm += sizeof(float) * 3;
            }

            //LOG("%f %f %f", reinterpret_cast<float3*>(data)[i].x, reinterpret_cast<float3*>(data)[i].y, reinterpret_cast<float3*>(data)[i].z);
        }
        attributesData.push_back(data);
    }

    //Indices part
    if (primitive.indices >= 0)
    {
        const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
        numIndices = indAcc.count;
        LOG("Tri: %u", numIndices/3);
        const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
        const unsigned char* buffer = &(model.buffers[indView.buffer].data[indAcc.byteOffset + indView.byteOffset]);

        indices = new unsigned int[numIndices];

        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
        {
            const uint32_t* bufferInd = reinterpret_cast<const uint32_t*>(buffer);
            for (unsigned int i = 0; i < indAcc.count; ++i)
            {
                //TODO: Change this
                indices[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
        {
            const uint16_t* bufferInd = reinterpret_cast<const uint16_t*>(buffer);
            for (unsigned int i = 0; i < indAcc.count; ++i)
            {
                indices[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
        {
            const uint8_t* bufferInd = reinterpret_cast<const uint8_t*>(buffer);
            for (unsigned int i = 0; i < indAcc.count; ++i)
            {
                indices[i] = bufferInd[i];
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

        attributes.emplace_back(Attribute::TANGENT, sizeof(float) * 4, vertexSize);
        vertexSize += sizeof(float) * 4;
        float* data = new float[tangAcc.count * 4];

        //Add vertices Tangent to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < tangAcc.count; ++i)
        {
            reinterpret_cast<float4*>(data)[i] = *reinterpret_cast<const float4*>(bufferTang);

            if (tangView.byteStride != 0)
            {
                bufferTang += tangView.byteStride;
            }
            else
            {
                bufferTang += sizeof(float) * 4;
            }

            //LOG("%f %f %f %f", reinterpret_cast<float4*>(data)[i].x, reinterpret_cast<float4*>(data)[i].y, reinterpret_cast<float4*>(data)[i].z, reinterpret_cast<float4*>(data)[i].w);
        }
        attributesData.push_back(data);
    }
    else
    {
        //Generate Tangents
        float* interleavedVertices = GetInterleavedData(attributes, attributesData, numVertices);
        GenerateTangents(attributes, attributesData, numIndices, indices, vertexSize, numVertices, interleavedVertices);
    }

    ResourceMesh* rMesh = new ResourceMesh(uid, numIndices, std::move(indices), numVertices, std::move(attributes), std::move(attributesData));
    Importer::Mesh::Save(rMesh);
    return rMesh;
}

void Importer::Mesh::Save(const ResourceMesh* rMesh)
{
    std::vector<Attribute> attributes;
    rMesh->GetAttributes(attributes);
    unsigned int header[] = { rMesh->GetNumberIndices(), rMesh->GetNumberVertices(), attributes.size()};

    unsigned int size = sizeof(header) + sizeof(unsigned int) * rMesh->GetNumberIndices();
    for (std::vector<Attribute>::const_iterator it = attributes.cbegin(); it != attributes.cend(); ++it)
    {
        size += it->size * rMesh->GetNumberVertices() + sizeof(Attribute);
    }

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    //Save Header
    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    //Save Indices
    bytes = sizeof(unsigned int) * rMesh->GetNumberIndices();
    memcpy(cursor, rMesh->GetIndices(), bytes);
    cursor += bytes;

    //Save attributes and data
    unsigned int idx = 0;
    for (std::vector<Attribute>::const_iterator it = attributes.cbegin(); it != attributes.cend(); ++it)
    {
        //save attribute metadata
        memcpy(cursor, &(*it), sizeof(Attribute));
        cursor += sizeof(Attribute);
        //save attribute data
        bytes = it->size * rMesh->GetNumberVertices();
        assert(rMesh->GetAttributeData(it->type) && "Mesh does not have this attribute");
        memcpy(cursor, rMesh->GetAttributeData(it->type), bytes);
        cursor += bytes;
        ++idx;
    }

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rMesh->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceMesh* Importer::Mesh::Load(const char* filePath, unsigned int uid)
{
    char* fileBuffer = nullptr;
    ResourceMesh* rMesh = nullptr;

    if (App->GetFileSystem()->Load(filePath, &fileBuffer))
    {
        //Load Header
        char* cursor = fileBuffer;
        unsigned int header[3];
        unsigned int bytes = sizeof(header);
        memcpy(header, cursor, bytes);
        cursor += bytes;
        unsigned int numIndices = header[0];
        unsigned int numVertices = header[1];

        unsigned int numAttributes = header[2];

        //Load Indices
        bytes = sizeof(unsigned int) * numIndices;
        unsigned int* indices = new unsigned int[numIndices];
        memcpy(indices, cursor, bytes);
        cursor += bytes;


        std::vector<Attribute> attributes;
        std::vector<float*> attributesData;
        for (int i = 0; i < numAttributes; ++i)
        {
            Attribute* attr = reinterpret_cast<Attribute*>(cursor);
            attributes.push_back(*attr);
            cursor += sizeof(Attribute);
            float* newAttributeData = new float[numVertices * attr->size / sizeof(float)];
            memcpy(newAttributeData, cursor, numVertices * attr->size);
            attributesData.push_back(newAttributeData);
            cursor += attr->size * numVertices;
        }

        rMesh = new ResourceMesh(uid, numIndices,  std::move(indices), numVertices, std::move(attributes), std::move(attributesData));
        delete[] fileBuffer;
    }

   return rMesh;
}
