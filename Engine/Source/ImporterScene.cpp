#include "ResourceScene.h"

#include "ImporterScene.h"
#include "SaveLoadScene.h"

ResourceScene* Importer::Scene::Import(const char* assetsPath, unsigned int uid, bool modifyAsset)
{
	ResourceScene* rScene = new ResourceScene(uid);

	Importer::Scene::Save(rScene, assetsPath);
	return rScene;
}
