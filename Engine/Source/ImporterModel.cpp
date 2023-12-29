#include "Globals.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"

#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

void ImporterModel::Import(const char* filePath) const
{
    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;
    std::string error, warning;
    bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, filePath);
    if (!loadOk)
    {
        LOG("[MODEL] Error loading %s: %s", filePath, error.c_str());
    }

    std::string modelName = filePath;
    unsigned pos = modelName.find_last_of('/');
    std::string name = modelName.substr(++pos);

    for (const auto& srcMesh : model.meshes)
    {
        for (const auto& primitive : srcMesh.primitives)
        {
            ImporterMesh importerM;
            Mesh* mesh = new Mesh();

            //importerM.Import();
        }
    }
}
