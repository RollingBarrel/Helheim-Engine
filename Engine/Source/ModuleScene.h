#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include "Math/float3.h"
#include "Archive.h"
#include <vector>
#include <unordered_map>
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
	bool CleanUp() override;

	// Getters
	GameObject* GetRoot() const { return mRoot; }
	const std::string& GetName() const;

	// GameObjects
	GameObject* Find(const char* name) const;
	GameObject* Find(unsigned int UID) const;
	GameObject* FindGameObjectWithTag(const std::string& tag);
	const std::vector<GameObject*>& FindGameObjectsWithTag(const std::string& tag);

	void AddGameObjectToScene(GameObject* gameObject);
	void RemoveGameObjectFromScene(GameObject* gameObject);
	void RemoveGameObjectFromScene(int id); 
	void RemoveGameObjectFromScene(const std::string& name);
	void AddGameObjectToDelete(GameObject* gameObject) { mGameObjectsToDelete.push_back(gameObject); }
	GameObject* DuplicateGameObject(GameObject* gameObject);
	void SwitchGameObjectsFromScene(GameObject* first, GameObject* second);

	// Save / Load Scene
	void NewScene();
	void Save(const char* saveFilePath) const;
	void Load(const char* saveFilePath);

	// Tags
	void AddToTagMap(const std::string &tag, GameObject* gameObject);
	void DeleteFromTagMap(const std::string& tag, GameObject* gameObject);

	// Prefabs
	GameObject* InstantiatePrefab(const char* name, GameObject* parent = nullptr);
	void SavePrefab(const GameObject& gameObject, const char* saveFilePath);
	void SavePrefabRecursive(const GameObject& objectToSave, JsonArray& gameObjects) const;
	GameObject* LoadPrefab(const char* saveFilePath, GameObject* parent = nullptr, bool update = false);
	void OverridePrefab(const JsonObject& gameObject, unsigned int id);

	void OpenPrefabScreen(const char* saveFilePath);
	void ClosePrefabScreen();
	bool IsPrefabScene() const { return mBackgroundScene != nullptr; }

	const std::unordered_map<unsigned int, unsigned int>& GetPrefabUIDMap() { return mPrefabOldNewUid; }

private:
	void AddGameObjectToDuplicate(GameObject* gameObject) { mGameObjectsToDuplicate.push_back(gameObject); }
	void DuplicateGameObjects();
	void DeleteGameObjects();

	GameObject* mRoot = nullptr;
	GameObject* mBackgroundScene = nullptr;

	// GameObjects
	std::vector<GameObject*> mSceneGO;
	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	std::unordered_map<std::string, std::vector<GameObject*>> mGameObjectsByTags;

	// Prefabs
	const char* mPrefabPath = "";
	bool mClosePrefab = false;

	std::unordered_map<unsigned int, unsigned int> mPrefabOldNewUid;

	static const std::vector<GameObject*> mEmptyVector; //This is used to avoid creating objects in methods and returning them as reference.
};

#endif //_MODULE_SCENE_H_