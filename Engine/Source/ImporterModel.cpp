#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"

#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

void Importer::Model::Import(const char* filePath)
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
    App->GetFileSystem()->Copy(filePath, gltfPath.c_str());

    std::string binPath = (ASSETS_MODEL_PATH + name + ".bin");
    App->GetFileSystem()->Copy(extension.c_str(), binPath.c_str());

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;

    std::string error, warning;
    bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, gltfPath);
    if (!loadOk)
    {
        LOG("[MODEL] Error loading %s: %s", gltfPath, error.c_str());
    }

    for (const auto& srcMesh : model.meshes)
    {
        for (const auto& primitive : srcMesh.primitives)
        {
            ResourceMesh* mesh = new ResourceMesh;
            mesh->mMeshName = name.c_str();
            Importer::Mesh::Import(model, primitive, mesh);
           
            delete mesh;
            mesh = nullptr;
        }
    }

    
}
