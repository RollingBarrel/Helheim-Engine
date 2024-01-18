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

static int GetNumFaces(const SMikkTSpaceContext* pContext)
{
    ResourceMesh* ptr = (ResourceMesh*)pContext->m_pUserData;
    return ptr->mNumVertices / 3;
}
static int GetNumVerticesOfFace(const SMikkTSpaceContext* pContext, const int iFace) {
    return 3;
}
static void GetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
{
    ResourceMesh* ptr = (ResourceMesh*)pContext->m_pUserData;
    float* posOut = &ptr->mVerticesPosition[(iFace * 3 + iVert)*3];
    fvPosOut[0] = posOut[0];
    fvPosOut[1] = posOut[1];
    fvPosOut[2] = posOut[2];
}
static void GetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
{
    ResourceMesh* ptr = (ResourceMesh*)pContext->m_pUserData;
    float* texCOut = &ptr->mVerticesTextureCoordinate[(iFace * 3 + iVert) * 2];
    fvTexcOut[0] = texCOut[0];
    fvTexcOut[1] = texCOut[1];
}
static void GetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
{
    ResourceMesh* ptr = (ResourceMesh*)pContext->m_pUserData;
    float* normalOut = &ptr->mVerticesTextureCoordinate[(iFace * 3 + iVert) * 3];
    fvNormOut[0] = normalOut[0];
    fvNormOut[1] = normalOut[1];
    fvNormOut[2] = normalOut[2];
}

static void SetTSpaceBasic(const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert)
{
    ResourceMesh* ptr = (ResourceMesh*)pContext->m_pUserData;
    float* currTangPtr = &ptr->mVerticesTangent[(iFace * 3 + iVert) * 4];
    memcpy(currTangPtr, fvTangent, 3 * sizeof(float));
    memcpy(&currTangPtr[3*sizeof(float)], &fSign, sizeof(float));
}

static void GenerateTangents(ResourceMesh* rMesh)
{
    assert(rMesh->mVerticesPosition == nullptr && "No positions to generate tangents");
    assert(rMesh->mVerticesTextureCoordinate == nullptr && "No texture coords to generate tangents");
    assert(rMesh->mVerticesNormal == nullptr && "No normals to generate tangents");
    ResourceMesh* unweldedRMesh = new ResourceMesh();
    unweldedRMesh->mNumIndices = 0;
    unweldedRMesh->mNumVertices = rMesh->mNumIndices;
    unweldedRMesh->mVerticesPosition = (float*)malloc(rMesh->mNumIndices * sizeof(float) * 3);
    unweldedRMesh->mVerticesTextureCoordinate = (float*)malloc(rMesh->mNumIndices * sizeof(float) * 2);
    unweldedRMesh->mVerticesNormal = (float*)malloc(rMesh->mNumIndices * sizeof(float) * 3);
    unweldedRMesh->mVerticesTangent = (float*)malloc(rMesh->mNumIndices * sizeof(float) * 4);
    for (unsigned int i = 0; i < rMesh->mNumIndices; ++i)
    {
        const unsigned int idx = rMesh->mIndices[i];
        memcpy(&unweldedRMesh->mVerticesPosition[i*3], &rMesh->mVerticesPosition[idx*3], 3 * sizeof(float));
        memcpy(&unweldedRMesh->mVerticesTextureCoordinate[i*2], &rMesh->mVerticesTextureCoordinate[idx*2], 2 * sizeof(float));
        memcpy(&unweldedRMesh->mVerticesNormal[i*3], &rMesh->mVerticesNormal[idx*3], 3 * sizeof(float));
    }

    SMikkTSpaceInterface interfaceInput = {};
    interfaceInput.m_getNumFaces = GetNumFaces;
    interfaceInput.m_getNumVerticesOfFace = GetNumVerticesOfFace;
    interfaceInput.m_getNormal = GetNormal;
    interfaceInput.m_getPosition = GetPosition;
    interfaceInput.m_getTexCoord = GetTexCoord;
    interfaceInput.m_setTSpaceBasic = SetTSpaceBasic;
    SMikkTSpaceContext tangContext = {};
    tangContext.m_pInterface = &interfaceInput;
    tangContext.m_pUserData = unweldedRMesh;
    if (!genTangSpaceDefault(&tangContext))
    {
        LOG("ERROR: Could not generate the tangent space");
    }
    float* unweldedInterleavedData = unweldedRMesh->GetInterleavedData();
    int* piRemapTable = (int*)malloc(unweldedRMesh->mNumVertices * sizeof(int));
    float* pfVertexDataOut = (float*)malloc(unweldedRMesh->mNumVertices * (rMesh->GetVertexSize() + 4 * sizeof(float)));
    unsigned int uniqueVertices = WeldMesh(piRemapTable, pfVertexDataOut,
        unweldedInterleavedData, unweldedRMesh->mNumVertices, rMesh->GetVertexSize() + 4 * sizeof(float));
    
    //TODO: pass the resourcemesh from interleaved to separated again
    //free(unweldedTVertices);
    //free(unweldedVertices);
}

void Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* mesh)
{
    mesh->mVertexSize = 0;
    
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

        posBuffer.uri;

        //App->GetFileSystem()->Copy();

        mesh->mNumVertices = posAcc.count;

        const unsigned char* bufferPos = &posBuffer.data[posView.byteOffset + posAcc.byteOffset];


        //Add vertices Pos to this buffer taking into acc byteStride
        mesh->mVerticesPosition = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferPos));
        for (auto i = 0; i < posAcc.count; ++i)
        {
            reinterpret_cast<float3*>(mesh->mVerticesPosition)[i] = *reinterpret_cast<const float3*>(bufferPos);

            if (posView.byteStride != 0) {
                bufferPos += posView.byteStride;
            }
            else {
                bufferPos += sizeof(float) * 3;
            }

            LOG("%f %f %f", reinterpret_cast<float3*>(mesh->mVerticesPosition)[i].x, reinterpret_cast<float3*>(mesh->mVerticesPosition)[i].y, reinterpret_cast<float3*>(mesh->mVerticesPosition)[i].z);
        }

        mesh->mVertexSize += 3*sizeof(float);
    }

    if (itTexCoord != primitive.attributes.end())
    {
        const tinygltf::Accessor& texCoordAcc = model.accessors[itTexCoord->second];
        assert(texCoordAcc.type == TINYGLTF_TYPE_VEC2);
        assert(texCoordAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAcc.bufferView];
        const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];

        const unsigned char* bufferTexCoord = &texCoordBuffer.data[texCoordView.byteOffset + texCoordAcc.byteOffset];

        //Add vertices TexCoord to this buffer taking into acc byteStride
        mesh->mVerticesTextureCoordinate = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferTexCoord));
        for (auto i = 0; i < texCoordAcc.count; ++i)
        {
            reinterpret_cast<float2*>(mesh->mVerticesPosition)[i] = *reinterpret_cast<const float2*>(bufferTexCoord);

            if (texCoordView.byteStride != 0)
            {
                bufferTexCoord += texCoordView.byteStride;
            }
            else
            {
                bufferTexCoord += sizeof(float) * 2;
            }

            LOG("%f %f", reinterpret_cast<float2*>(mesh->mVerticesTextureCoordinate)[i].x, reinterpret_cast<float2*>(mesh->mVerticesTextureCoordinate)[i].y);
        }

        mesh->mVertexSize += 2 * sizeof(float);
    }

    if (itNorm != primitive.attributes.end())
    {
        const tinygltf::Accessor& normAcc = model.accessors[itNorm->second];
        assert(normAcc.type == TINYGLTF_TYPE_VEC3);
        assert(normAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& normView = model.bufferViews[normAcc.bufferView];
        const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];

        const unsigned char* bufferNorm = &normBuffer.data[normView.byteOffset + normAcc.byteOffset];

        //Add vertices Normal to this buffer taking into acc byteStride
        mesh->mVerticesNormal = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferNorm));

        for (auto i = 0; i < normAcc.count; ++i)
        {
            reinterpret_cast<float3*>(mesh->mVerticesNormal)[i] = *reinterpret_cast<const float3*>(bufferNorm);

            if (normView.byteStride != 0)
            {
                bufferNorm += normView.byteStride;
            }
            else
            {
                bufferNorm += sizeof(float) * 3;
            }

            LOG("%f %f %f", reinterpret_cast<float3*>(mesh->mVerticesNormal)[i].x, reinterpret_cast<float3*>(mesh->mVerticesNormal)[i].y, reinterpret_cast<float3*>(mesh->mVerticesNormal)[i].z);
        }

        mesh->mVertexSize += 3 * sizeof(float);
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

        //Add vertices Tangent to this buffer taking into acc byteStride
        mesh->mVerticesTangent = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferTang));

        for (auto i = 0; i < tangAcc.count; ++i)
        {
            reinterpret_cast<float4*>(mesh->mVerticesTangent)[i] = *reinterpret_cast<const float4*>(bufferTang);

            if (tangView.byteStride != 0)
            {
                bufferTang += tangView.byteStride;
            }
            else
            {
                bufferTang += sizeof(float) * 4;
            }

            LOG("%f %f %f", reinterpret_cast<float4*>(mesh->mVerticesTangent)[i].x, reinterpret_cast<float4*>(mesh->mVerticesTangent)[i].y, reinterpret_cast<float4*>(mesh->mVerticesTangent)[i].z);
        }
    }
    else
    {
        //Generate Tangents
        GenerateTangents(mesh);
    }
    mesh->mVertexSize += 4* sizeof(float);

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

void ResourceMesh::LoadInterleavedAttribute(float* fillBuffer, const float* attribData, unsigned int& attribFloatsOffset, unsigned int attribElements, unsigned int elementFloats) const
{
    if (attribData != nullptr)
    {
        unsigned int attributeFloats = sizeof(float) * elementFloats * attribElements;
        for (int vertexIdx = 0; vertexIdx < mNumVertices; ++vertexIdx)
        {
            memcpy(&fillBuffer[vertexIdx * attributeFloats + attribFloatsOffset], &mVerticesPosition[vertexIdx * attributeFloats], attributeFloats * sizeof(float));
        }
        attribFloatsOffset += attributeFloats;
    }
}

float* ResourceMesh::GetInterleavedData() const
{
    float* ret = new float[mNumVertices*mVertexSize];
    unsigned int attributeFloats = 0;
    unsigned int attribFloatsOffset = 0;
    LoadInterleavedAttribute(ret, mVerticesPosition, attribFloatsOffset, 3, 3);
    LoadInterleavedAttribute(ret, mVerticesTextureCoordinate, attribFloatsOffset, 2, 2);
    LoadInterleavedAttribute(ret, mVerticesNormal, attribFloatsOffset, 3, 3);
    LoadInterleavedAttribute(ret, mVerticesTangent, attribFloatsOffset, 4, 4);
    LoadInterleavedAttribute(ret, mVerticesColor, attribFloatsOffset, 3, 3);
    return ret;
}

void ResourceMesh::FromInterleavedData(float* vData, unsigned int numvertices, unsigned int* iData, unsigned int numIndices, char attrBitmask)
{
    if (mIndices != nullptr)
    {
        //delete mIndices;
        //delete[] mIndices;
        free(mIndices);
    }
    if(attrBitmask & POS)
        
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
