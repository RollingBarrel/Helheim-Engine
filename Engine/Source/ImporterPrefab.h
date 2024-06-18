#pragma once

class ResourcePrefab;

namespace Importer
{
    namespace Prefab
    {
        ResourcePrefab* Import(const char* assetsPath, unsigned int uid, bool modifyAsset = false);
    }
}
