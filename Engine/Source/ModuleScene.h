#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include "Math/float3.h"
#include "Archive.h"
#include <vector>
#include <string>

class Quadtree;
class GameObject;
class MeshRendererComponent;
class Archive;
class Tag;

class ENGINE_API ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;

	// Getters
	GameObject* GetRoot() const { return mRoot; }
	const std::string& GetName() const;

	// GameObjects
	GameObject* Find(const char* name) const;
	GameObject* Find(unsigned int UID) const;
	void AddGameObjectToScene(GameObject* gameObject);
	void RemoveGameObjectFromScene(GameObject* gameObjet);
	void RemoveGameObjectFromScene(int id); 
	void RemoveGameObjectFromScene(const std::string& name);
	void AddGameObjectToDelete(GameObject* gameObject) {
		mGameObjectsToDelete.push_back(gameObject);
	}
	void AddGameObjectToDuplicate(GameObject* gameObject) {
		mGameObjectsToDuplicate.push_back(gameObject);
	}
	void AddGameObjectToLoadIntoScripts(std::pair<unsigned int, GameObject**> pair) {
		mGameObjectsToLoadIntoScripts.push_back(pair);
	}

	void AddMeshToRender(const MeshRendererComponent& meshRendererComponent);

	// Quadtree
	Quadtree* GetQuadtreeRoot() const { return mQuadtreeRoot; }
	bool GetShouldUpdateQuadtree() const { return mShouldUpdateQuadtree; }
	void SetShouldUpdateQuadtree(bool updateQuadtree) { mShouldUpdateQuadtree = updateQuadtree; }

	// Frustum Culling
	bool GetApplyFrustumCulling() const { return mApplyculling; }
	void SetApplyFrustumCulling(bool applyFrustumCulling) { mApplyculling = applyFrustumCulling; }

	// Save / Load Scene
	void NewScene();
	void Save(const char* saveFilePath) const;
	void Load(const char* saveFilePath);

	// Tags
	GameObject* FindGameObjectWithTag(unsigned tagID);
	GameObject* FindGameObjectWithTag(const char* tagName);
	void FindGameObjectsWithTag(unsigned tagID, std::vector<GameObject*>& foundGameObjects);
	void FindGameObjectsWithTag(const char* tagName, std::vector<GameObject*>& foundGameObjects);

	void AddTag(std::string tag);
	unsigned int GetSize() { return static_cast<unsigned int>(mTags.size()); };
	int GetCustomTagsSize();
	std::vector<Tag*> GetAllTags() { return mTags; };
	std::vector<Tag*> GetSystemTag();
	std::vector<Tag*> GetCustomTag();
	Tag* GetTagByName(std::string tagname);
	Tag* GetTagByID(unsigned id);
	void DeleteTag(Tag* tag);

	// Prefabs
	static GameObject* InstantiatePrefab(const char* name, GameObject* parent = nullptr);
	int SavePrefab(const GameObject& gameObject, const char* saveFilePath) const;
	GameObject* LoadPrefab(const char* saveFilePath, unsigned int resourceId, bool update = false, GameObject* parent = nullptr);
	GameObject* LoadPrefab(const char* saveFilePath, unsigned int resourceId, GameObject* parent) { return LoadPrefab(saveFilePath, resourceId, false, parent); }
	void OpenPrefabScreen(const char* saveFilePath);
	void ClosePrefabScreen();
	bool IsPrefabScene() const { return mBackgroundScene != nullptr; }

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();
	void LoadGameObjectsIntoScripts();
	
	void SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive) const;
	void SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive) const;
	void LoadGameObject(const rapidjson::Value& gameObjectsJson, GameObject* parent);

	GameObject* mRoot = nullptr;
	GameObject* mBackgroundScene = nullptr;

	// GameObjects
	std::vector<GameObject*> mSceneGO;
	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	std::vector<std::pair<unsigned int, GameObject**>> mGameObjectsToLoadIntoScripts;

	// Quadtree
	Quadtree* mQuadtreeRoot = nullptr;
	bool mShouldUpdateQuadtree = false;
	bool mApplyculling = false;

	// Prefabs
	const char* mPrefabPath = "";
	bool mClosePrefab = false;

	// Tags
	std::vector<Tag*> mTags;
	unsigned mLastTagIndex = 10;

	// Others
	std::vector<const MeshRendererComponent*>mCurrRenderComponents;
};

#endif //_MODULE_SCENE_H_