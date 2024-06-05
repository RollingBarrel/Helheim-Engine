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

	mTags.clear();
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

GameObject* ModuleScene::FindGameObjectWithTag(const std::string& tagID)
{
	for (GameObject* go : mSceneGO)
	{
		if (go->GetTag() == tagID)
		{
			return go;
		}
	}

	return nullptr;
}

void ModuleScene::FindGameObjectsWithTag(const std::string& tagID, std::vector<GameObject*>& foundGameObjects)
{
	for (GameObject* go : mSceneGO)
	{
		if (go->GetTag() == tagID)
		{
			foundGameObjects.push_back(go);
		}
	}
}

void ModuleScene::AddTag(std::string tagName)
{
	bool exists = false;

	for (auto it = mTags.cbegin(); it < mTags.cend(); it++)
	{

	}
	
	if (!exists)
	{
		mTags.push_back(tagName);
	}
}

void ModuleScene::DeleteTag(std::string tag)
{

}

//
//int ModuleScene::GetCustomTagsSize()
//{
//	std::vector<Tag*> customs = GetCustomTag();
//	return customs.size();
//}
//
//std::vector<Tag*> ModuleScene::GetSystemTag()
//{
//	std::vector<Tag*> systemTags;
//	std::copy_if(mTags.begin(), mTags.end(), std::back_inserter(systemTags),
//		[](Tag* tag) { return tag->GetType() == TagType::SYSTEM; });
//
//	return systemTags;
//}
//
//std::vector<Tag*> ModuleScene::GetCustomTag()
//{
//	std::vector<Tag*> systemTags;
//	std::copy_if(mTags.begin(), mTags.end(), std::back_inserter(systemTags),
//		[](Tag* tag) { return tag->GetType() == TagType::CUSTOM; });
//
//	return systemTags;
//}
//
//Tag* ModuleScene::GetTagByName(std::string tagname)
//{
//	for (Tag* tag : mTags)
//	{
//		if (std::strcmp(tag->GetName().c_str(), tagname.c_str()) == 0)
//		{
//			return tag;
//		}
//	}
//	return nullptr;
//}																																																																																																																																																																																	
//
//Tag* ModuleScene::GetTagByID(unsigned id)
//{ 
//	for (Tag* tag : mTags)
//	{
//		if (tag->GetID() == id)
//		{
//			return tag;
//		}
//	}
//	return nullptr;
//}
//
//void ModuleScene::DeleteTag(Tag* tag)
//{
//	auto it = std::find(mTags.begin(), mTags.end(), tag);
//
//	if (it != mTags.end())
//	{
//		// 1. Set tags to untagged
//		//std::vector<GameObject*> objects = GameObject::FindGameObjectsWithTag(tag->GetName());
//		//for (auto object : objects)
//		//{
//		//	object->SetTag(GetTagByName("Untagged"));
//		//}
//		//
//		// 2. Delete it
//		mTags.erase(it);
//		delete tag;
//	}
//}

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

	std::string saveFilePath = ASSETS_SCENES_PATH + std::string(sceneName);
	if (saveFilePath.find(".json") == std::string::npos)
	{
		saveFilePath += ".json";
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
		delete mRoot;
		mRoot = mBackgroundScene;
		mBackgroundScene = nullptr;
		mRoot->SetEnabled(true);
	}
	std::string filePath = ASSETS_SCENES_PATH + std::string(sceneName);
	
	if (filePath.find(".json") == std::string::npos)
	{
		filePath += ".json";
	}

	char* fileBuffer = nullptr;

	if (App->GetFileSystem()->Load(filePath.c_str(), &fileBuffer) > 0)
	{
		mQuadtreeRoot->CleanUp();
		App->GetUI()->CleanUp();
		mSceneGO.clear();
		delete mRoot;
		mRoot = new GameObject("SampleScene", nullptr);

		Archive doc(fileBuffer);
		JsonObject root = doc.GetRootObject();

		JsonObject scene = root.GetJsonObject("Scene");
		mRoot->SetName(scene.GetString("Name").c_str());

		JsonArray gameObjects = scene.GetJsonArray("GameObjects");
		for (unsigned int i = 0; i < gameObjects.Size(); ++i)
		{
			JsonObject gameObjectData = gameObjects.GetJsonObject(i);
			GameObject* gO = new GameObject(gameObjectData.GetInt("UID"), gameObjectData.GetString("Name").c_str(), Find(gameObjectData.GetInt("ParentUID")));
			gO->Load(gameObjectData);
			mSceneGO.push_back(gO);
		}

		mRoot->RecalculateMatrices();		
		mQuadtreeRoot->UpdateTree();

		delete[] fileBuffer;

		LoadGameObjectsIntoScripts();
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
		AddGameObjectToScene(mGameObjectsToDuplicate[i]);
	}

	mGameObjectsToDuplicate.clear();
}

void ModuleScene::LoadGameObjectsIntoScripts()
{
	for (std::pair<unsigned int, GameObject**> pair : mGameObjectsToLoadIntoScripts)
	{
		*(pair.second) = Find(pair.first);
	}

	mGameObjectsToLoadIntoScripts.clear();
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



