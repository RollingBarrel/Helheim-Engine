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
#include "Globals.h"

#include "ResourceScene.h"

#include <algorithm>
#include <iterator>
#include "Algorithm/Random/LCG.h"

#include "GeometryBatch.h"
#include "ImporterMesh.h"

ModuleScene::ModuleScene() 
{
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

	//TODO: temporally removed the quadtree
	//if (mShouldUpdateQuadtree)
	//{
	//	mQuadtreeRoot->UpdateTree();
	//}
	//mQuadtreeRoot->GetRenderComponentsInFrustum(App->GetCamera()->GetCurrentCamera()->GetFrustum(), mCurrRenderComponents);
	mRoot->GetMeshesInChildren(mCurrRenderComponents);
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

GameObject* ModuleScene::FindGameObjectWithTag(const std::string& tag)
{
	if (mGameObjectsByTags.find(tag) != mGameObjectsByTags.end())
	{
		if (!mGameObjectsByTags[tag].empty())
		{
			return *mGameObjectsByTags[tag].begin();
		}
	}

	return nullptr;
}

const std::vector<GameObject*>& ModuleScene::FindGameObjectsWithTag(const std::string& tag)
{
	if (mGameObjectsByTags.find(tag) != mGameObjectsByTags.end())
	{
		if (!mGameObjectsByTags[tag].empty())
		{
			return mGameObjectsByTags[tag];
		}
	}

	return std::vector<GameObject*>();
}

void ModuleScene::AddToTagMap(const std::string& tag, GameObject* gameObject)
{
	
	if (mGameObjectsByTags.find(tag) == mGameObjectsByTags.end())
	{
		mGameObjectsByTags[tag] = std::vector<GameObject*>();	
	}
	mGameObjectsByTags[tag].push_back(gameObject);

}

void ModuleScene::DeleteFromTagMap(const std::string& tag, GameObject* gameObject)
{
	if (mGameObjectsByTags.find(tag) != mGameObjectsByTags.end())
	{
		std::vector<GameObject*>& tagVec = mGameObjectsByTags[tag];
		for (std::vector<GameObject*>::const_iterator it = tagVec.cbegin(); it != tagVec.cend(); ++it)
		{
			if ((*it)->GetID() == gameObject->GetID())
			{
				tagVec.erase(it);
				break;
			}
		}
	}
}

#pragma endregion

#pragma region Save / Load Scene
void ModuleScene::Save(const char* sceneName) const
{
	//Don't save an open Prefab over the scene
	if (mBackgroundScene != nullptr)
	{
		LOG("Can't save scene while the Prefab editor is open");
		return;
	}

	std::string saveFilePath = sceneName;
	if (saveFilePath.find(".scn") == std::string::npos)
	{
		saveFilePath += ".scn";
	}

	Archive doc;
	JsonObject root = doc.GetRootObject();

	JsonObject scene = root.AddNewJsonObject("Scene");
	scene.AddString("Name", mRoot->GetName().c_str());

	JsonArray objArray = scene.AddNewJsonArray("GameObjects");
	for (const GameObject* go : mSceneGO) 
	{
		JsonObject gameObjectData = objArray.PushBackNewObject();
		go->Save(gameObjectData);
	}

	std::string out = doc.Serialize();
	App->GetFileSystem()->Save(saveFilePath.c_str(), out.c_str(), static_cast<unsigned int>(out.length()));
}

void ModuleScene::Load(const char* sceneName)
{
	//Close Prefab editor before loading a new scene
	if (mBackgroundScene != nullptr)
	{
		mSceneGO.clear();
		delete mRoot;
		mRoot = mBackgroundScene;
		mBackgroundScene = nullptr;
		mRoot->SetEnabled(true);
	}
	std::string filePath = ASSETS_SCENES_PATH + std::string(sceneName);

	if (filePath.find(".scn") == std::string::npos)
	{
		filePath += ".scn";
	}
	Resource* rScene = App->GetResource()->RequestResource(filePath.c_str());

	char* fileBuffer = nullptr;

	if (App->GetFileSystem()->Load(filePath.c_str(), &fileBuffer) > 0)
	{
		mQuadtreeRoot->CleanUp();
		App->GetUI()->CleanUp();
		mSceneGO.clear();
		delete mRoot;
		mRoot = new GameObject("SampleScene", nullptr);

		Archive doc(fileBuffer);
		delete[] fileBuffer;
		JsonObject root = doc.GetRootObject();

		JsonObject scene = root.GetJsonObject("Scene");
		mRoot->SetName(scene.GetString("Name").c_str());

		JsonArray gameObjects = scene.GetJsonArray("GameObjects");
		std::unordered_map<unsigned int, GameObject*> loadMap;
		//Load GameObjects
		for (unsigned int i = 0; i < gameObjects.Size(); ++i)
		{
			JsonObject gameObjectData = gameObjects.GetJsonObject(i);
			GameObject* gO = new GameObject(gameObjectData.GetInt("UID"), gameObjectData.GetString("Name").c_str(), Find(gameObjectData.GetInt("ParentUID")));
			gO->LoadGameObject(gameObjectData, loadMap);
		}
		//Load Components
		for (unsigned int i = 0; i < gameObjects.Size(); ++i)
		{
			JsonObject gameObjectData = gameObjects.GetJsonObject(i);
			mSceneGO[i]->LoadComponents(gameObjectData, loadMap);
		}

		mRoot->RecalculateMatrices();
		mQuadtreeRoot->UpdateTree();

		App->GetScriptManager()->AwakeScripts();
		App->GetScriptManager()->StartScripts();
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
	//SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector);
	//archive->AddObjectArray("GameObjects", gameObjectsArchiveVector);
	//prefabArchive->AddObject("Prefab", *archive);

	std::string out = prefabArchive->Serialize();
	App->GetFileSystem()->Save(saveFilePath, out.c_str(), static_cast<unsigned int>(out.length()));
	PathNode* root = App->GetFileSystem()->GetRootNode();
	root->mChildren.clear();
	App->GetFileSystem()->DiscoverFiles("Assets", root);
	delete prefabArchive;
	delete archive;
	//gameObject->GetParent()->DeleteChild(gameObject);		//TODO: Why delete yourself?
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
			//LoadGameObject(sceneValue, parent, &uuids);
			if (mSceneGO.size() > offset)
			{
				mSceneGO[offset]->SetPrefabId(resource->GetUID());
				ret = mSceneGO[offset];
			}
		}

		//LoadGameObjectsIntoScripts(); DOES NOT EXIST NOW
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
	else
	{
		mPrefabPath = saveFilePath;
	}
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
		if (go->GetName() == name)
		{
			return go;
		}
	}

	return nullptr;
}

GameObject* ModuleScene::Find(unsigned int UID) const
{
	if (UID == 1)
	{
		return mRoot;
	}

	for (GameObject* go : mSceneGO)
	{
		if (go->GetID() == UID)
		{
			return go;
		}
	}

	return nullptr;
}
//TOOD: Redo Change this we are doing only a push back in the worst place possible
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

void ModuleScene::RemoveGameObjectFromScene(int id) 
{
	for (std::vector<GameObject*>::iterator it = mSceneGO.begin(); it != mSceneGO.end(); ++it)
	{
		if ((*it)->GetID() == id)
		{
			mSceneGO.erase(it);
			break;
		}
	}
}

void ModuleScene::RemoveGameObjectFromScene(const std::string& name) 
{
	for (std::vector<GameObject*>::iterator it = mSceneGO.begin(); it != mSceneGO.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			mSceneGO.erase(it);
			break;
		}
	}
}

void ModuleScene::DeleteGameObjects()
{
	for (GameObject* gameObject : mGameObjectsToDelete)
	{
		gameObject->GetParent()->RemoveChild(gameObject->GetID());
		RemoveGameObjectFromScene(gameObject->GetName());
		delete gameObject;
	}

	mGameObjectsToDelete.clear();
}

void ModuleScene::DuplicateGameObjects()
{
	for (int i = 0; i < mGameObjectsToDuplicate.size(); ++i)
	{
		mGameObjectsToDuplicate[i]->GetParent()->AddChild(mGameObjectsToDuplicate[i]);
	}

	mGameObjectsToDuplicate.clear();
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



