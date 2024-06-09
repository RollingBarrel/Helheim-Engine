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
	void AddGameObjectToDuplicate(GameObject* gameObject) {	mGameObjectsToDuplicate.push_back(gameObject); }
	void SwitchGameObjectsFromScene(GameObject* first, GameObject* second);

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
	void AddToTagMap(const std::string &tag, GameObject* gameObject);
	void DeleteFromTagMap(const std::string& tag, GameObject* gameObject);

	// Prefabs
	GameObject* InstantiatePrefab(const char* name, GameObject* parent = nullptr);
	int SavePrefab(const GameObject& gameObject, const char* saveFilePath) const;
	GameObject* LoadPrefab(const char* saveFilePath, bool update = false, GameObject* parent = nullptr);
	GameObject* LoadPrefab(const char* saveFilePath, GameObject* parent) { return LoadPrefab(saveFilePath, false, parent); }
	void OpenPrefabScreen(const char* saveFilePath);
	void ClosePrefabScreen();
	bool IsPrefabScene() const { return mBackgroundScene != nullptr; }

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();

	GameObject* mRoot = nullptr;
	GameObject* mBackgroundScene = nullptr;

	// GameObjects
	std::vector<GameObject*> mSceneGO;
	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	std::unordered_map<std::string, std::vector<GameObject*>> mGameObjectsByTags;

	// Quadtree
	Quadtree* mQuadtreeRoot = nullptr;
	bool mShouldUpdateQuadtree = false;
	bool mApplyculling = false;

	// Prefabs
	const char* mPrefabPath = "";
	bool mClosePrefab = false;

	// Others
	std::vector<const MeshRendererComponent*> mCurrRenderComponents;
};

#endif //_MODULE_SCENE_H_