#pragma once

class ResourceScene;

namespace Importer
{
    namespace Scene
    {
        ResourceScene* Import(const char* assetsPath, unsigned int uid, bool modifyAsset = false);
    }
}