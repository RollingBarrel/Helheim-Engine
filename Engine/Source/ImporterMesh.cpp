#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterMesh.h"
#include "ResourceMesh.h"

#include "glew.h"

#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "Algorithm/Random/LCG.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

ResourceMesh* Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
    //TODO: Use the function Add attribute to import to the resource mesh
    // Try not to use friend func import on resourceMesh
    // Put the indices private

    ResourceMesh* rMesh = new ResourceMesh(math::LCG().Int());

    const auto& itPos = primitive.attributes.find("POSITION");
    const auto& itTexCoord = primitive.attributes.find("TEXCOORD_0");
    const auto& itNorm = primitive.attributes.find("NORMAL");
    const auto& itTang = primitive.attributes.find("TANGENT");

    Attribute attr = Attribute(Attribute::POS,sizeof(float)*3, 0);
    if (itPos != primitive.attributes.end())
    {
        const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
        assert(posAcc.type == TINYGLTF_TYPE_VEC3);
        assert(posAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

        rMesh->SetNumberVertices(posAcc.count);

        const unsigned char* bufferPos = &posBuffer.data[posView.byteOffset + posAcc.byteOffset];

        attr = Attribute(Attribute::POS, sizeof(float) * 3, 0);
        float* data = new float[posAcc.count * 3];
        rMesh->AddAttribute(attr, data);
        //Add vertices Pos to this buffer taking into acc byteStride
        for (auto i = 0; i < posAcc.count; ++i)
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

    }

    if (itTexCoord != primitive.attributes.end())
    {
        const tinygltf::Accessor& texCoordAcc = model.accessors[itTexCoord->second];
        assert(texCoordAcc.type == TINYGLTF_TYPE_VEC2);
        assert(texCoordAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAcc.bufferView];
        const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];

        const unsigned char* bufferTexCoord = &texCoordBuffer.data[texCoordView.byteOffset + texCoordAcc.byteOffset];


        attr = Attribute(Attribute::UV, sizeof(float) * 2, 0);
        float* data = new float[texCoordAcc.count * 2];
        rMesh->AddAttribute(attr, data);
        //Add vertices TexCoord to this buffer taking into acc byteStride
        for (auto i = 0; i < texCoordAcc.count; ++i)
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
    }

    if (itNorm != primitive.attributes.end())
    {
        const tinygltf::Accessor& normAcc = model.accessors[itNorm->second];
        assert(normAcc.type == TINYGLTF_TYPE_VEC3);
        assert(normAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& normView = model.bufferViews[normAcc.bufferView];
        const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];

        const unsigned char* bufferNorm = &normBuffer.data[normView.byteOffset + normAcc.byteOffset];

        attr = Attribute(Attribute::NORMAL, sizeof(float) * 3, 0);
        float* data = new float[normAcc.count * 3];
        rMesh->AddAttribute(attr, data);

        //Add vertices Normal to this buffer taking into acc byteStride
        for (auto i = 0; i < normAcc.count; ++i)
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

    }

    //Indices part
    if (primitive.indices >= 0)
    {
        const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
        rMesh->SetNumberIndices(indAcc.count);
        const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
        const unsigned char* buffer = &(model.buffers[indView.buffer].data[indAcc.byteOffset + indView.byteOffset]);

        rMesh->AddIndices(new unsigned int[rMesh->GetNumberIndices()]);

        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
        {
            const uint32_t* bufferInd = reinterpret_cast<const uint32_t*>(buffer);
            for (uint32_t i = 0; i < indAcc.count; ++i)
            {
                rMesh->GetIndices()[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
        {
            const uint16_t* bufferInd = reinterpret_cast<const uint16_t*>(buffer);
            for (uint16_t i = 0; i < indAcc.count; ++i)
            {
                rMesh->GetIndices()[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
        {
            const uint8_t* bufferInd = reinterpret_cast<const uint8_t*>(buffer);
            for (uint8_t i = 0; i < indAcc.count; ++i)
            {
                rMesh->GetIndices()[i] = bufferInd[i];
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

        attr = Attribute(Attribute::TANGENT, sizeof(float) * 4, 0);
        float* data = new float[tangAcc.count * 4];
        rMesh->AddAttribute(attr, data);

        //Add vertices Tangent to this buffer taking into acc byteStride
        for (auto i = 0; i < tangAcc.count; ++i)
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
    }
    else
    {
        //Generate Tangents
        rMesh->GenerateTangents();
    }

    return rMesh;
}

void Importer::Mesh::Save(const ResourceMesh* mesh)
{
    unsigned int header[] = { mesh->GetNumberIndices(), mesh->GetNumberVertices(), mesh->GetAttributes().size()};

    unsigned int size = sizeof(header) + sizeof(unsigned int) * mesh->GetNumberIndices();
    for (std::vector<Attribute*>::const_iterator it = mesh->GetAttributes().cbegin(); it != mesh->GetAttributes().cend(); ++it)
    {
        size += (*it)->size * mesh->GetNumberIndices() + sizeof(Attribute);
    }

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    //Save Header
    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;
    //Save Indices
    bytes = sizeof(unsigned int) * mesh->GetNumberIndices();
    memcpy(cursor, (void*)mesh->GetNumberIndices(), bytes);
    cursor += bytes;
    //Save attributes and data
    unsigned int idx = 0;
    for (std::vector<Attribute*>::const_iterator it = mesh->GetAttributes().cbegin(); it != mesh->GetAttributes().cend(); ++it)
    {
        //save attribute metadata
        memcpy(cursor, *(it), sizeof(Attribute));
        cursor += sizeof(Attribute);
        //save attribute data
        bytes = (*it)->size * mesh->GetNumberVertices();
        memcpy(cursor, mesh->mAttributesData[idx], bytes);
        cursor += bytes;
        ++idx;
    }

    std::string path = LIBRARY_MESH_PATH;
    path += std::to_string(mesh->GetUID());
    path += ".mesh";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;
}

void Importer::Mesh::Load(ResourceMesh* mesh, const char* fileName)
{
    std::string path = LIBRARY_MESH_PATH;
    path += fileName;
    path += ".mesh";

    char* fileBuffer;
    App->GetFileSystem()->Load(path.c_str(), &fileBuffer);

    //Load Header
    char* cursor = fileBuffer;
    unsigned int header[3];
    unsigned int bytes = sizeof(header);
    memcpy(header, cursor, bytes);
    cursor += bytes;
    mesh->SetNumberIndices(header[0]);
    mesh->SetNumberVertices(header[1]);
    unsigned int numAttributes = header[2];
    //Load Indices
    bytes = sizeof(unsigned int) * mesh->GetNumberIndices();
    mesh->AddIndices(new unsigned int[mesh->GetNumberIndices()]);
    memcpy(mesh->GetIndices(), cursor, bytes);
    cursor += bytes;

    for (int i = 0; i < numAttributes; ++i)
    {
        Attribute* attr = reinterpret_cast<Attribute*>(cursor);
        cursor += sizeof(Attribute);
        mesh->AddAttribute(*attr, reinterpret_cast<float*>(cursor));
        cursor += attr->size * mesh->GetNumberVertices();
    }

    mesh->LoadToMemory();
}