#pragma once
#include "ModuleScene.h"
#include "GameObject.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "glew.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "Component.h"
#include "ModuleUI.h"

#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"
#include "ModuleScriptManager.h"
#include "ModuleDetourNavigation.h"
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleResource.h"
#include "Tag.h"
#include "Globals.h"

#include <algorithm>
#include <iterator>
#include "Algorithm/Random/LCG.h"

#include "GeometryBatch.h"
#include "ImporterMesh.h"

ModuleScene::ModuleScene() 
{

	mTags.push_back(new Tag(0, "Untagged", TagType::SYSTEM));
	mTags.push_back(new Tag(1, "Respawn", TagType::SYSTEM));
	mTags.push_back(new Tag(2, "Finish", TagType::SYSTEM));
	mTags.push_back(new Tag(3, "EditorOnly", TagType::SYSTEM));
	mTags.push_back(new Tag(4, "MainCamera", TagType::SYSTEM));
	mTags.push_back(new Tag(5, "Player", TagType::SYSTEM));
	mTags.push_back(new Tag(6, "Obstacle", TagType::SYSTEM));
	mTags.push_back(new Tag(7, "Enemy", TagType::SYSTEM));
	mTags.push_back(new Tag(8, "CombatArea", TagType::SYSTEM));
	mTags.push_back(new Tag(9, "Bullet", TagType::SYSTEM));
	mTags.push_back(new Tag(10, "AimGrenadeArea", TagType::SYSTEM));

}

ModuleScene::~ModuleScene()
{
	if (mRoot)
	{
		delete mRoot;
	}
	if (mBackgroundScene)
	{
		delete mBackgroundScene;
	}
	if (mQuadtreeRoot)
	{
		delete mQuadtreeRoot;
	}

	for (Tag* tag : mTags) 
	{
		delete tag;
	}
}

#pragma region Basic Functions

bool ModuleScene::Init()
{
	mRoot = new GameObject("SampleScene", nullptr);
	mQuadtreeRoot = new Quadtree(AABB(float3(-5000 , -500 , -5000), float3(5000, 500, 5000)));
	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	mShouldUpdateQuadtree = false;
	mRoot->Update();

	if (mShouldUpdateQuadtree)
	{
		mQuadtreeRoot->UpdateTree();
	}
	mQuadtreeRoot->GetRenderComponentsInFrustum(App->GetCamera()->GetCurrentCamera()->GetFrustum(), mCurrRenderComponents);
	App->GetOpenGL()->Draw(mCurrRenderComponents);
	mCurrRenderComponents.clear();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	if (!mGameObjectsToDelete.empty())
	{
		DeleteGameObjects();
	}
	if (!mGameObjectsToDuplicate.empty())
	{
		DuplicateGameObjects();
	}
	if (mClosePrefab)
	{
		int resourceId = SavePrefab(*mRoot->GetChildren()[0], mPrefabPath);
		delete mRoot;
		mRoot = mBackgroundScene;
		mBackgroundScene = nullptr;
		mRoot->SetEnabled(true);
		LoadPrefab(mPrefabPath, true);
		mPrefabPath = "";
		mClosePrefab = false;
	}
	if (mPrefabPath != "" && mBackgroundScene == nullptr)
	{
		mBackgroundScene = mRoot;
		mBackgroundScene->SetEnabled(false);
		mRoot = new GameObject(mPrefabPath, nullptr);
		LoadPrefab(mPrefabPath);
	}
	return UPDATE_CONTINUE;
}

#pragma endregion

#pragma region Tags

GameObject* ModuleScene::FindGameObjectWithTag(unsigned tagID)
{
	for (GameObject* go : mSceneGO)
	{
		if (go->GetTag()->GetID() == tagID && go != mRoot)
		{
			return go;
		}
	}

	return nullptr;
}

GameObject* ModuleScene::FindGameObjectWithTag(const char* tagName)
{
	Tag* tag = App->GetScene()->GetTagByName(tagName);
	return FindGameObjectWithTag(tag->GetID());
}

void ModuleScene::FindGameObjectsWithTag(unsigned tagID, std::vector<GameObject*>& foundGameObjects)
{
	for (GameObject* go : mSceneGO)
	{
		if (go->GetTag()->GetID() == tagID && go != mRoot)
		{
			foundGameObjects.push_back(go);
		}
	}
}

void ModuleScene::FindGameObjectsWithTag(const char* tagName, std::vector<GameObject*>& foundGameObjects)
{
	Tag* tag = App->GetScene()->GetTagByName(tagName);
	FindGameObjectsWithTag(tag->GetID(), foundGameObjects);
}

void ModuleScene::AddTag(std::string tagname)
{
	if (GetTagByName(tagname) == nullptr)
	{
		Tag* newTag = new Tag(mLastTagIndex, tagname, TagType::CUSTOM);
		mTags.push_back(newTag);

		++mLastTagIndex;
	}
}

int ModuleScene::GetCustomTagsSize()
{
	std::vector<Tag*> customs = GetCustomTag();
	return customs.size();
}

std::vector<Tag*> ModuleScene::GetSystemTag()
{
	std::vector<Tag*> systemTags;
	std::copy_if(mTags.begin(), mTags.end(), std::back_inserter(systemTags),
		[](Tag* tag) { return tag->GetType() == TagType::SYSTEM; });

	return systemTags;
}

std::vector<Tag*> ModuleScene::GetCustomTag()
{
	std::vector<Tag*> systemTags;
	std::copy_if(mTags.begin(), mTags.end(), std::back_inserter(systemTags),
		[](Tag* tag) { return tag->GetType() == TagType::CUSTOM; });

	return systemTags;
}

Tag* ModuleScene::GetTagByName(std::string tagname)
{
	for (Tag* tag : mTags)
	{
		if (std::strcmp(tag->GetName().c_str(), tagname.c_str()) == 0)
		{
			return tag;
		}
	}
	return nullptr;
}

Tag* ModuleScene::GetTagByID(unsigned id)
{
	for (Tag* tag : mTags)
	{
		if (tag->GetID() == id)
		{
			return tag;
		}
	}
	return nullptr;
}

void ModuleScene::DeleteTag(Tag* tag)
{
	auto it = std::find(mTags.begin(), mTags.end(), tag);

	if (it != mTags.end())
	{

		// 1. Set tags to untagged
		std::vector<GameObject*> objects = GameObject::FindGameObjectsWithTag(tag->GetName());
		for (auto object : objects)
		{
			object->SetTag(GetTagByName("Untagged"));
		}

		// 2. Delete it
		mTags.erase(it);
		delete tag;
	}
}

#pragma endregion

#pragma region Save / Load Scene

void ModuleScene::SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive) const
{
	// Save the current GameObject to its archive
	Archive gameObjectArchive;
	gameObject->Save(gameObjectArchive);
	gameObjectsArchive.push_back(gameObjectArchive);

	const std::vector<GameObject*>& children = gameObject->GetChildren();
	if (!children.empty())
	{
		for (GameObject* child : children)
		{
			SaveGameObjectRecursive(child, gameObjectsArchive);
		}
	}
}

void ModuleScene::SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive) const
{

	std::vector<Archive> gameObjectsArchiveVector;

	for (GameObject* gameObject : gameObjects)
	{
		SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector);
	}

	rootArchive.AddObjectArray("GameObjects", gameObjectsArchiveVector);
}

void ModuleScene::Save(const char* sceneName) const
{
	//Don't save an open Prefab over the scene
	if (mBackgroundScene != nullptr)
	{
		LOG("Can't save scene while the Prefab editor is open");
		return;
	}
	std::string saveFilePath = "Assets/Scenes/" + std::string(sceneName);
	if (saveFilePath.find(".json") == std::string::npos)
	{
		saveFilePath += ".json";
	}

	Archive* archive = new Archive();
	archive->AddString("Name", mRoot->GetName().c_str());
	
	// Not using recursive to save, using the sceneGO vector
	//SaveGame(mRoot->GetChildren(), *archive);
	std::vector<Archive> gameObjectsArchiveVector;
	for (GameObject* go : mSceneGO) 
	{
		Archive gameObjectArchive;
		go->Save(gameObjectArchive);
		gameObjectsArchiveVector.push_back(gameObjectArchive);
	}
	archive->AddObjectArray("GameObjects", gameObjectsArchiveVector);

	Archive* sceneArchive = new Archive();
	sceneArchive->AddObject("Scene", *archive);
	std::string out = sceneArchive->Serialize();
	App->GetFileSystem()->Save(saveFilePath.c_str(), out.c_str(), static_cast<unsigned int>(out.length()));
	
	delete sceneArchive;
	delete archive;
}

void ModuleScene::Load(const char* sceneName)
{
	//Close Prefab editor before loading a new scene
	if (mBackgroundScene != nullptr)
	{
		delete mRoot;
		mRoot = mBackgroundScene;
		mBackgroundScene = nullptr;
		mRoot->SetEnabled(true);
	}
	std::string loadFilePath = "Assets/Scenes/" + std::string(sceneName);
	if (loadFilePath.find(".json") == std::string::npos)
	{
		loadFilePath += ".json";
	}

	char* loadedBuffer = nullptr;

	if (App->GetFileSystem()->Load(loadFilePath.c_str(), &loadedBuffer) > 0)
	{
		rapidjson::Document document;
		rapidjson::ParseResult ok = document.Parse(loadedBuffer);
		if (!ok)
		{
			LOG("Scene was not loaded.");
			return;
		}

		mQuadtreeRoot->CleanUp();
		App->GetUI()->CleanUp();
		delete mRoot;
		mSceneGO.clear();
		mRoot = new GameObject("SampleScene", nullptr);


		if (document.HasMember("Scene") && document["Scene"].IsObject())
		{
			const rapidjson::Value& sceneValue = document["Scene"];
			if (sceneValue.HasMember("Name"))
			{
				mRoot->SetName(sceneValue["Name"].GetString());
				App->GetNavigation()->LoadResourceData();
			}
			LoadGameObject(sceneValue, mRoot);
		}

		mQuadtreeRoot->UpdateTree();
		delete[] loadedBuffer;

		LoadGameObjectsIntoScripts();
		App->GetScriptManager()->AwakeScripts();
		App->GetScriptManager()->StartScripts();
	}
}

void ModuleScene::LoadGameObject(const rapidjson::Value& gameObjectsJson, GameObject* parent, std::unordered_map<int, int>* uuids)
{
	// Manage GameObjects inside the Scene
	if (gameObjectsJson.HasMember("GameObjects") && gameObjectsJson["GameObjects"].IsArray())
	{
		const rapidjson::Value& gameObjects = gameObjectsJson["GameObjects"];
		int offset = mSceneGO.size();
		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
		{
			if (gameObjects[i].IsObject())
			{
				GameObject::LoadGameObjectFromJSON(gameObjects[i], parent, uuids);
			}
		}

		parent->RecalculateMatrices();

		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
		{
			// Manage Components
			if (gameObjects[i].HasMember("Components") && gameObjects[i]["Components"].IsArray())
			{
				mSceneGO[offset + i]->LoadComponentsFromJSON(gameObjects[i]["Components"]);
			}
		}

	}
}

#pragma endregion

#pragma region Prefabs

GameObject* ModuleScene::InstantiatePrefab(const char* name, GameObject* parent)
{
	std::string strName = name;
	if (strName.find(ASSETS_PREFABS_PATH) == std::string::npos)
	{
		strName = ASSETS_PREFABS_PATH + strName;
	}
	GameObject* gameObject = App->GetScene()->LoadPrefab(strName.c_str(), parent);
	return gameObject;
}

int ModuleScene::SavePrefab(const GameObject& objectToSave, const char* saveFilePath) const
{
	GameObject* gameObject = new GameObject(objectToSave, mRoot); //Make a copy to change IDs
	gameObject->SetRotation(float3::zero);
	gameObject->SetPosition(float3::zero);
	gameObject->RecalculateMatrices();
	unsigned int resourceId = LCG().Int();
	Resource* resource = App->GetResource()->RequestResource(mPrefabPath);
	if (resource != nullptr) { resourceId = resource->GetUID(); }
	Archive* prefabArchive = new Archive();
	Archive* archive = new Archive();
	std::vector<Archive> gameObjectsArchiveVector;
	SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector);
	archive->AddObjectArray("GameObjects", gameObjectsArchiveVector);
	prefabArchive->AddObject("Prefab", *archive);

	std::string out = prefabArchive->Serialize();
	App->GetFileSystem()->Save(saveFilePath, out.c_str(), static_cast<unsigned int>(out.length()));
	PathNode* root = App->GetFileSystem()->GetRootNode();
	root->mChildren.clear();
	App->GetFileSystem()->DiscoverFiles("Assets", root);
	delete prefabArchive;
	delete archive;
	gameObject->GetParent()->DeleteChild(gameObject);
	return resourceId;
}

GameObject* ModuleScene::LoadPrefab(const char* saveFilePath, bool update, GameObject* parent)
{
	GameObject* ret = nullptr;
	if (parent == nullptr) parent = mRoot;
	char* loadedBuffer = nullptr;
	App->GetFileSystem()->Load(saveFilePath, &loadedBuffer);
	Resource* resource = App->GetResource()->RequestResource(saveFilePath);
	rapidjson::Document d;
	rapidjson::ParseResult ok = d.Parse(loadedBuffer);
	if (!ok)
	{
		LOG("Object was not loaded.");
		return nullptr;
	}
	std::unordered_map<int, int> uuids;

	if (d.HasMember("Prefab") && d["Prefab"].IsObject())
	{
		const rapidjson::Value& sceneValue = d["Prefab"];

		if (update)
		{
			mRoot->LoadChangesPrefab(sceneValue, resource->GetUID());
		}
		else
		{
			int offset = mSceneGO.size();
			LoadGameObject(sceneValue, parent, &uuids);
			if (mSceneGO.size() > offset)
			{
				mSceneGO[offset]->SetPrefabId(resource->GetUID());
				ret = mSceneGO[offset];
			}
		}

		LoadGameObjectsIntoScripts();
		App->GetScriptManager()->StartScripts();
		
	}
	delete[] loadedBuffer;
	
	return ret;
}

void ModuleScene::OpenPrefabScreen(const char* saveFilePath)
{
	if (mBackgroundScene != nullptr) 
	{ 
		mClosePrefab = true; 
	}
	mPrefabPath = saveFilePath;
}

void ModuleScene::ClosePrefabScreen()
{
	if (mBackgroundScene != nullptr)
	{
		mClosePrefab = true;
	}
}

#pragma endregion

#pragma region GameObject Functions

GameObject* ModuleScene::Find(const char* name) const
{
	for (GameObject* go : mSceneGO)
	{
		if (go->GetName() == name) return go;
	}

	return nullptr;
}

GameObject* ModuleScene::Find(unsigned int UID) const
{
	for (GameObject* go : mSceneGO)
	{
		if (go->GetID() == UID) return go;
	}

	return nullptr;
}

void ModuleScene::AddGameObjectToScene(GameObject* gameObject) 
{
	mSceneGO.push_back(gameObject);
}

void ModuleScene::RemoveGameObjectFromScene(GameObject* gameObject) 
{
	for (std::vector<GameObject*>::iterator it = mSceneGO.begin(); it != mSceneGO.end(); ++it)
	{
		if ((*it)->GetID() == gameObject->GetID())
		{
			mSceneGO.erase(it);
			return;
		}
	}
}

void ModuleScene::RemoveGameObjectFromScene(int id) {
	for (std::vector<GameObject*>::iterator it = mSceneGO.begin(); it != mSceneGO.end(); ++it)
	{
		if ((*it)->GetID() == id)
		{
			mSceneGO.erase(it);
			return;
		}
	}
}

void ModuleScene::RemoveGameObjectFromScene(const std::string& name) {
	for (std::vector<GameObject*>::iterator it = mSceneGO.begin(); it != mSceneGO.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			mSceneGO.erase(it);
			return;
		}
	}
}

void ModuleScene::DeleteGameObjects()
{

	for (GameObject* gameObject : mGameObjectsToDelete)
	{
		gameObject->GetParent()->DeleteChild(gameObject);
	}

	mGameObjectsToDelete.clear();
}

void ModuleScene::DuplicateGameObjects()
{

	for (GameObject* gameObject : mGameObjectsToDuplicate)
	{
		gameObject->GetParent()->AddChild(gameObject);
	}

	mGameObjectsToDuplicate.clear();

}

void ModuleScene::LoadGameObjectsIntoScripts()
{
	for (auto& pair : mGameObjectsToLoadIntoScripts)
	{
		*(pair.second) = Find(pair.first);
	}
}

#pragma endregion

#pragma region Others

void ModuleScene::AddMeshToRender(const MeshRendererComponent& meshRendererComponent)
{
	mCurrRenderComponents.push_back(&meshRendererComponent);
}

void ModuleScene::NewScene()
{
	mQuadtreeRoot->CleanUp();
	App->GetUI()->CleanUp();
	delete mRoot;
	mRoot = new GameObject("Untlitled", nullptr);
}

const std::string& ModuleScene::GetName() const
{
	return mRoot->GetName();
}

#pragma endregion



