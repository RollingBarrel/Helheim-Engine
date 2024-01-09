#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterMesh.h"

#include "glew.h"

#include "float2.h"
#include "float3.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

void Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* mesh)
{
    //Create Duplicate .bin
    App->GetFileSystem();

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
    }

    if (itTang != primitive.attributes.end())
    {
        const tinygltf::Accessor& tangAcc = model.accessors[itTang->second];
        assert(tangAcc.type == TINYGLTF_TYPE_VEC3);
        assert(tangAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& tangView = model.bufferViews[tangAcc.bufferView];
        const tinygltf::Buffer& tangBuffer = model.buffers[tangView.buffer];

        const unsigned char* bufferTang = &tangBuffer.data[tangView.byteOffset + tangAcc.byteOffset];

        //Add vertices Tangent to this buffer taking into acc byteStride
        mesh->mVerticesTangent = reinterpret_cast<float*>(const_cast<unsigned char*>(bufferTang));

        for (auto i = 0; i < tangAcc.count; ++i)
        {
            reinterpret_cast<float3*>(mesh->mVerticesTangent)[i] = *reinterpret_cast<const float3*>(bufferTang);

            if (tangView.byteStride != 0)
            {
                bufferTang += tangView.byteStride;
            }
            else
            {
                bufferTang += sizeof(float) * 3;
            }

            LOG("%f %f %f", reinterpret_cast<float3*>(mesh->mVerticesTangent)[i].x, reinterpret_cast<float3*>(mesh->mVerticesTangent)[i].y, reinterpret_cast<float3*>(mesh->mVerticesTangent)[i].z);
        }
    }

    //TODO: Add Indices part
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

    Mesh::Save(mesh);

    char* fileBuffer = nullptr;
    ResourceMesh loadedMesh;
    Mesh::Load(fileBuffer, &loadedMesh, mesh->mMeshName);
}

void Importer::Mesh::Save(const ResourceMesh* mesh)
{
    unsigned int header[2] = { mesh->mNumIndices, mesh->mNumVertices };

    unsigned int size = sizeof(header) + sizeof(unsigned int) * mesh->mNumIndices + sizeof(float) * mesh->mNumVertices * 3;

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
    //Load Indices
    bytes = sizeof(unsigned int) * mesh->mNumIndices;
    mesh->mIndices = new unsigned int[mesh->mNumIndices];
    memcpy(mesh->mIndices, cursor, bytes);
    //Load Positions
    bytes = sizeof(float) * mesh->mNumVertices * 3;
    mesh->mVerticesPosition = new float[mesh->mNumVertices * 3];
    memcpy(mesh->mVerticesPosition, cursor, bytes);

    mesh->LoadVBO();
    mesh->LoadEBO();
    mesh->LoadVAO();
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
