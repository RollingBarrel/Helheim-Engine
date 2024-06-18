#include "ResourcePrefab.h"

#include "ImporterPrefab.h"
#include "SaveLoadPrefab.h"

ResourcePrefab* Importer::Prefab::Import(const char* assetsPath, unsigned int uid, bool modifyAsset)
{
	ResourcePrefab* rPrefab = new ResourcePrefab(uid);

	Importer::Prefab::Save(rPrefab, assetsPath);

	return rPrefab;
}
