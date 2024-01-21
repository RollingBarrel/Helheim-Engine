#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"

#include "Algorithm/Random/LCG.h"


#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

void Importer::Model::Import(const char* filePath, ResourceModel* rModel)
{
    //Create GLTF Dupliate REMEMBER .gltf && .bin must have the same name
    std::string modelName = filePath;
    unsigned pos = modelName.find_last_of('/');
    std::string name = modelName.substr(++pos);
    unsigned dotPos = name.find_last_of('.');
    name = name.substr(0, dotPos);

    std::string extension = filePath;
    unsigned extensionPos = extension.find_last_of('.');
    extension = extension.substr(0, extensionPos);
    extension.append(".bin");

    std::string gltfPath = (ASSETS_MODEL_PATH + name + ".gltf");
    App->GetFileSystem()->CopyAbsolutePath(filePath, gltfPath.c_str());

    std::string binPath = (ASSETS_MODEL_PATH + name + ".bin");
    App->GetFileSystem()->CopyAbsolutePath(extension.c_str(), binPath.c_str());

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;

    std::string error, warning;
    bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, gltfPath);
    if (!loadOk)
    {
        LOG("[MODEL] Error loading %s: %s", gltfPath, error.c_str());
    }
   
    for (int i = 0; i < model.images.size(); ++i)
    {
        std::string pngName = filePath;
        unsigned filePos = pngName.find_last_of('/');
        pngName = pngName.substr(0, filePos+1);
        pngName.append(model.images[i].uri);

        std::string images = (ASSETS_TEXTURE_PATH + model.images[i].uri);

        App->GetFileSystem()->CopyAbsolutePath(pngName.c_str(), images.c_str());
    }

    for (const auto& srcMesh : model.meshes)
    {
        for (const auto& primitive : srcMesh.primitives)
        {
            ResourceMesh* mesh = new ResourceMesh();
            mesh->mMeshName = name.c_str();
            mesh->mUID = math::LCG().Int();

            Importer::Mesh::Import(model, primitive, mesh);

            if (primitive.material != -1) {
                ResourceMaterial* material = new ResourceMaterial();  
                material->mUID = math::LCG().Int();
                Importer::Material::Import(model, model.materials[primitive.material], material);

                Model::Save(rModel);

                delete material;
                material = nullptr;
            }

            delete mesh;
            mesh = nullptr;

        }
    }
    
}

void Importer::Model::Save(const ResourceModel* ourModel)
{
    unsigned int UIDs[2] = { ourModel->meshUID, ourModel->materiaUID };

    unsigned int size = sizeof(UIDs);

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(UIDs);
    memcpy(cursor, UIDs, bytes);
    cursor += bytes;

    std::string path = LIBRARY_MODEL_PATH;
    path += std::to_string(ourModel->mUID);
    path += ".model";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

void Importer::Model::Load(ResourceModel* ourModel, const char* fileName)
{
    char* fileBuffer;

    std::string path = LIBRARY_MODEL_PATH;
    path += fileName;
    path += ".model";

    App->GetFileSystem()->Load(path.c_str(), &fileBuffer);

    char* cursor = fileBuffer;
    unsigned int UIDs[2];
    unsigned int bytes = sizeof(UIDs);

    memcpy(UIDs, cursor, bytes);
    cursor += bytes;
    ourModel->meshUID = UIDs[0];
    ourModel->materiaUID = UIDs[1];
}
