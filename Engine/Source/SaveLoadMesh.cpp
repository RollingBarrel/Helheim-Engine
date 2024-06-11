#include "SaveLoadMesh.h"
#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include <vector>
#include <assert.h>

void Importer::Mesh::Save(const ResourceMesh* rMesh)
{
    std::vector<Attribute> attributes;
    rMesh->GetAttributes(attributes);
    unsigned int header[] = { rMesh->GetNumberIndices(), rMesh->GetNumberVertices(), attributes.size() };

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
        for (unsigned int i = 0; i < numAttributes; ++i)
        {
            Attribute* attr = reinterpret_cast<Attribute*>(cursor);
            attributes.push_back(*attr);
            cursor += sizeof(Attribute);
            float* newAttributeData = new float[numVertices * attr->size / sizeof(float)];
            memcpy(newAttributeData, cursor, numVertices * attr->size);
            attributesData.push_back(newAttributeData);
            cursor += attr->size * numVertices;
        }

        rMesh = new ResourceMesh(uid, numIndices, std::move(indices), numVertices, std::move(attributes), std::move(attributesData));
        delete[] fileBuffer;
    }

    return rMesh;
}
