#include "ModuleScene.h"
#include "Application.h"
#include "Globals.h"

#include "ModuleCamera.h"
#include "ModuleUI.h"
#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"
#include "ModuleScriptManager.h"
#include "ModuleDetourNavigation.h"
#include "MeshRendererComponent.h"
#include <algorithm> 
#include "glew.h"

#include "PlayerStats.h"

const std::vector<GameObject*> ModuleScene::mEmptyVector = std::vector<GameObject*>();

ModuleScene::ModuleScene() 
{
}

ModuleScene::~ModuleScene()
{
	
}

#pragma region Basic Functions

bool ModuleScene::Init()
{
	mRoot = new GameObject("EmptyScene", nullptr);
	mPlayerStats = new PlayerStats();
	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	mRoot->Update();
	if (App->GetCamera()->GetCurrentCamera())
	{
		App->GetOpenGL()->Draw();
	}

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
		SavePrefab(*mRoot->GetChildren()[0], mPrefabPath);
		AddGameObjectToDelete(mRoot->GetChildren()[0]);
		mRoot = mBackgroundScene;
		mBackgroundScene = nullptr;
		mRoot->SetEnabled(true);
		//LoadPrefab(mPrefabPath, nullptr, true);
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

bool ModuleScene::CleanUp()
{
	if (mRoot)
	{
		delete mRoot;
	}
	if (mBackgroundScene)
	{
		delete mBackgroundScene;
	}

	delete mPlayerStats;

	return true;
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
		return mGameObjectsByTags[tag];
	}

	return ModuleScene::mEmptyVector;
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
		if (go->GetName() == "Teleport1" || go->GetName() == "Teleport2")
		{
			LOG("Juan");
		}
		JsonObject gameObjectData = objArray.PushBackNewObject();
		go->Save(gameObjectData);
	}
	scene.AddInt("NavMeshResource", App->GetNavigation()->GetResourceId());
	scene.AddInt("SkyBoxResource", App->GetOpenGL()->GetSkyboxID());
	scene.AddFloat("BloomIntensity", App->GetOpenGL()->GetBloomIntensity());
	scene.AddFloats("DirectionalLight", reinterpret_cast<const float*>(&App->GetOpenGL()->GetDirectionalLight()), sizeof(DirectionalLight) / sizeof(float));
	JsonObject fogObj = scene.AddNewJsonObject("Fog");
	float fogCol[3]; App->GetOpenGL()->GetFogColor(fogCol);
	fogObj.AddFloats("Color", fogCol, 3);
	fogObj.AddFloat("Density", App->GetOpenGL()->GetFogDensity());
	fogObj.AddFloat("HeightFallof", App->GetOpenGL()->GetFogHeightFallof());
	fogObj.AddFloat("MaxFog", App->GetOpenGL()->GetMaxFog());
	JsonObject volObject = scene.AddNewJsonObject("Volumetric");
	volObject.AddFloat("BaseExtCoeff", App->GetOpenGL()->GetVolBaseExtCoeff());
	volObject.AddFloat("NoiseAmount", App->GetOpenGL()->GetVolNoiseAmount());
	volObject.AddFloat("VolIntensity", App->GetOpenGL()->GetVolIntensity());
	volObject.AddFloat("Anisotropy", App->GetOpenGL()->GetVolAnisotropy());
	volObject.AddFloat("StepSize", App->GetOpenGL()->GetVolStepSize());
	volObject.AddInt("MaxSteps", App->GetOpenGL()->GetVolMaxSteps());

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

	//TODO: Request resource only works in engine 
	//App->GetResource()->RequestResource(sceneName);

	std::string loadFilePath = sceneName;
	if (loadFilePath.find(".scn") == std::string::npos)
	{
		loadFilePath += ".scn";
	}


	char* fileBuffer = nullptr;

	if (App->GetFileSystem()->Load(loadFilePath.c_str(), &fileBuffer) > 0)
	{
		//App->GetScene()->Load(fileBuffer);
	}

	if(fileBuffer != nullptr)
	{
		mPrefabOldNewUid.clear();
		App->GetUI()->CleanUp();
		mSceneGO.clear();
		delete mRoot;
		mGameObjectsByTags.clear();
		mRoot = new GameObject("EmptyScene", nullptr);
		Archive doc(fileBuffer);
		delete[] fileBuffer;
		JsonObject root = doc.GetRootObject();

		JsonObject scene = root.GetJsonObject("Scene");
		mRoot->SetName(scene.GetString("Name").c_str());

		JsonArray gameObjects = scene.GetJsonArray("GameObjects");
		std::unordered_map<unsigned int, GameObject*> loadMap;
		//Load GameObjects		//TODO: REDOO Look if prefab has been overrided

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

		unsigned int resourceNavMesh = scene.GetInt("NavMeshResource");
		if(resourceNavMesh != 0)
			App->GetNavigation()->CreateQuery(resourceNavMesh);
		else
			App->GetNavigation()->ReleaseResource();

		if(scene.HasMember("SkyBoxResource"))
			App->GetOpenGL()->SetSkybox(scene.GetInt("SkyBoxResource"));
		else
			App->GetOpenGL()->SetSkybox(0);

		if (scene.HasMember("BloomIntensity"))
			App->GetOpenGL()->SetBloomIntensity(scene.GetFloat("BloomIntensity"));
		else
			App->GetOpenGL()->SetBloomIntensity(0.5f);

		float directionalLight[]{ 0.0f, -1.0f, -1.0f, 0.0f, 1.f, 1.f, 1.f, 0.05f };
		if (scene.HasMember("DirectionalLight"))
			scene.GetFloats("DirectionalLight", directionalLight);
		App->GetOpenGL()->SetDirectionalLight(*reinterpret_cast<DirectionalLight*>(directionalLight));

		if (scene.HasMember("Fog"))
		{
			JsonObject fogObj = scene.GetJsonObject("Fog");
			float fogCol[3]; fogObj.GetFloats("Color", fogCol);
			App->GetOpenGL()->SetFogColor(fogCol);
			App->GetOpenGL()->SetFogDensity(fogObj.GetFloat("Density"));
			App->GetOpenGL()->SetFogHeightFallof(fogObj.GetFloat("HeightFallof"));
			App->GetOpenGL()->SetMaxFog(fogObj.GetFloat("MaxFog"));
		}
		else
		{
			//Default Values
			float fogCol[3] = { 1.0f,1.0f,1.0f };
			App->GetOpenGL()->SetFogColor(fogCol);
			App->GetOpenGL()->SetFogDensity(0.009f);
			App->GetOpenGL()->SetFogHeightFallof(0.0f);
			App->GetOpenGL()->SetMaxFog(0.0f);
		}

		if (scene.HasMember("Volumetric"))
		{
			JsonObject volObj = scene.GetJsonObject("Volumetric");
			App->GetOpenGL()->SetVolBaseExtCoeff(volObj.GetFloat("BaseExtCoeff"));
			App->GetOpenGL()->SetVolNoiseAmount(volObj.GetFloat("NoiseAmount"));
			App->GetOpenGL()->SetVolIntensity(volObj.GetFloat("VolIntensity"));
			App->GetOpenGL()->SetVolAnisotropy(volObj.GetFloat("Anisotropy"));
			App->GetOpenGL()->SetVolStepSize(volObj.GetFloat("StepSize"));
			App->GetOpenGL()->SetVolMaxSteps(volObj.GetInt("MaxSteps"));
		}
		else
		{
			//Default Values
			App->GetOpenGL()->SetVolBaseExtCoeff(0.04f);
			App->GetOpenGL()->SetVolNoiseAmount(1.0f);
			App->GetOpenGL()->SetVolIntensity(1.0f);
			App->GetOpenGL()->SetVolAnisotropy(0.35f);
			App->GetOpenGL()->SetVolStepSize(1.0f);
			App->GetOpenGL()->SetVolMaxSteps(16);
		}

		App->GetScriptManager()->AwakeScripts();
		
		if (App->IsPlayMode())
		{
			App->GetScriptManager()->StartScripts();
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

void ModuleScene::SavePrefab(const GameObject& objectToSave, const char* saveFilePath)
{
	GameObject* gameObject = new GameObject(objectToSave, mRoot); //Make a copy to change IDs
	gameObject->SetWorldRotation(float3::zero);
	gameObject->SetWorldPosition(float3::zero);
	
	Archive doc;
	JsonObject root = doc.GetRootObject();
	JsonObject prefab = root.AddNewJsonObject("Prefab");
	JsonArray gameObjects = prefab.AddNewJsonArray("GameObjects");
	SavePrefabRecursive(objectToSave, gameObjects);

	std::string out = doc.Serialize();
	App->GetFileSystem()->Save(saveFilePath, out.c_str(), static_cast<unsigned int>(out.length()));
	PathNode* rootNode = App->GetFileSystem()->GetRootNode();
	rootNode->mChildren.clear();
	App->GetFileSystem()->DiscoverFiles("Assets", rootNode);

	gameObject->GetParent()->RemoveChild(gameObject->GetID());		//TODO: Why delete yourself?
	AddGameObjectToDelete(gameObject);
}

void ModuleScene::SavePrefabRecursive(const GameObject& objectToSave, JsonArray& gameObjects) const
{
	JsonObject obj = gameObjects.PushBackNewObject();
	objectToSave.Save(obj);
	for (std::vector<GameObject*>::const_iterator it = objectToSave.GetChildren().cbegin(); it < objectToSave.GetChildren().cend(); ++it)
	{
		SavePrefabRecursive(*(*it), gameObjects);
	}
}

GameObject* ModuleScene::LoadPrefab(const char* saveFilePath, GameObject* parent, bool update)
{
	mPrefabOldNewUid.clear();
	
	GameObject* ret = nullptr;
	if (parent == nullptr) 
	{
		parent = mRoot;
	}

	char* fileBuffer = nullptr;

	if (App->GetFileSystem()->Load(saveFilePath, &fileBuffer) > 0)
	{
		Archive doc(fileBuffer);
		delete[] fileBuffer;

		JsonObject root = doc.GetRootObject();

		JsonObject prefab = root.GetJsonObject("Prefab");
		
		std::unordered_map<unsigned int, GameObject*> loadMap;

		JsonArray gameObjects = prefab.GetJsonArray("GameObjects");	

		unsigned int currSize = mSceneGO.empty() ? 0 : mSceneGO.size();

		//Load GameObjects
		for (unsigned int i = 0; i < gameObjects.Size(); ++i)
		{
			JsonObject gameObjectData = gameObjects.GetJsonObject(i);
			GameObject* gO;
			if (i == 0)
			{
				gO = new GameObject(gameObjectData.GetString("Name").c_str(), parent);
			}
			else
			{
				gO = new GameObject(gameObjectData.GetString("Name").c_str(), Find(mPrefabOldNewUid[gameObjectData.GetInt("ParentUID")]));
			}
			mPrefabOldNewUid[gameObjectData.GetInt("UID")] = gO->GetID();
			gO->LoadGameObject(gameObjectData, loadMap);
		}

		//Load Components
		for (unsigned int i = 0; i < gameObjects.Size(); ++i)
		{
			JsonObject gameObjectData = gameObjects.GetJsonObject(i);
			mSceneGO[currSize + i]->LoadComponents(gameObjectData, loadMap);
		}
	
		ret = mSceneGO[currSize];

		App->GetScriptManager()->AwakeGameObjectScripts(ret);
		
		if (App->IsPlayMode())
		{
			App->GetScriptManager()->StartGameObjectScripts(ret);
		}
		
	}
	
	return ret;
}

void ModuleScene::OverridePrefab(const JsonObject& gameObject, unsigned int id)
{
	//if (mPrefabOverride && mPrefabId == id)
	//{
	//	std::vector<GameObject*> loadedObjects;
	//	for (GameObject* child : mChildren)
	//	{
	//		delete child;
	//	}
	//	mChildren.clear();
	//	std::unordered_map<int, int> uuids;
	//
	//	if (obj.HasMember("Prefab") && obj["GameObjects"].IsArray())
	//	{
	//		const rapidjson::Value& gameObjects = gameObject["GameObjects"];
	//		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
	//		{
	//			if (gameObjects[i].IsObject())
	//			{
	//				unsigned int parentUID{ 0 };
	//				unsigned int uuid{ 0 };
	//
	//				if (gameObjects[i].HasMember("ParentUID") && gameObjects[i]["ParentUID"].IsInt())
	//				{
	//					parentUID = gameObjects[i]["ParentUID"].GetInt();
	//				}
	//				if (gameObjects[i].HasMember("UID") && gameObjects[i]["UID"].IsInt())
	//				{
	//					uuid = gameObjects[i]["UID"].GetInt();
	//				}
	//				if (parentUID == 1) {
	//					if (gameObjects[i].HasMember("Components") && gameObjects[i]["Components"].IsArray())
	//					{
	//						loadedObjects.push_back(this);
	//						uuids[uuid] = mUid;
	//						//LoadComponentsFromJSON(gameObjects[i]["Components"]);
	//					}
	//				}
	//				else
	//				{
	//					//GameObject* go = LoadGameObjectFromJSON(gameObjects[i], this, &uuids);
	//					//loadedObjects.push_back(go);
	//					//go->LoadComponentsFromJSON(gameObjects[i]["Components"]);
	//				}
	//			}
	//		}
	//		mParent->RecalculateMatrices();
	//		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
	//		{
	//			if (gameObjects[i].IsObject())
	//			{
	//				//loadedObjects[i]->LoadComponentsFromJSON(gameObjects[i]["Components"]);
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	for (GameObject* child : mChildren)
	//	{
	//		child->LoadChangesPrefab(obj, id);
	//	}
	//}
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

const void ModuleScene::FilterGameObjects(std::string& filter, std::vector<GameObject*>& filteredElements) const
{
	std::transform(filter.begin(), filter.end(), filter.begin(), ::toupper);
	for (GameObject* go : mSceneGO)
	{
		std::string name = go->GetName();
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		if(name.find(filter) != std::string::npos)
		{
			filteredElements.push_back(go);
		}
	}
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

GameObject* ModuleScene::DuplicateGameObject(GameObject* gameObject)
{
	std::unordered_map<const GameObject*, GameObject*> originalToNew;
	std::vector<MeshRendererComponent*>mRenderers;
	GameObject* duplicatedGameObject = new GameObject(*gameObject, gameObject->GetParent(), &originalToNew, &mRenderers);
	for (MeshRendererComponent* mRend : mRenderers)
	{
		if (mRend->HasSkinning())
		{
			mRend->UpdateSkeletonObjects(originalToNew);
		}
	}
	AddGameObjectToDuplicate(duplicatedGameObject);

	return duplicatedGameObject;
}

void ModuleScene::SwitchGameObjectsFromScene(GameObject* first, GameObject* second)
{
	for (std::vector<GameObject*>::const_iterator it = mSceneGO.cbegin(); it != mSceneGO.cend(); ++it)
	{
		if ((*it)->GetID() == second->GetID())
		{
			mSceneGO.erase(it);
			break;
		}
	}

	if (!first->IsRoot())
	{
		for (std::vector<GameObject*>::const_iterator it = mSceneGO.cbegin(); it != mSceneGO.cend(); ++it)
		{
			if ((*it)->GetID() == first->GetID())
			{
				if (it+1 != mSceneGO.cend())
				{
					mSceneGO.insert(it+1, second);
				}
				else
				{
					mSceneGO.push_back(second);
				}

				break;
			}
		}
	}
	else 
	{
		mSceneGO.push_back(second);
	}

	for (int i = second->GetChildren().size() - 1; i >= 0; --i)
	{
		SwitchGameObjectsFromScene(second, second->GetChildren()[i]);
	}
}

void ModuleScene::DeleteGameObjects()
{
	for (GameObject* gameObject : mGameObjectsToDelete)
	{
		gameObject->GetParent()->RemoveChild(gameObject->GetID());
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

void ModuleScene::NewScene()
{
	App->GetUI()->CleanUp();
	mGameObjectsByTags.clear();
	mSceneGO.clear();

	App->GetNavigation()->ReleaseResource();
	App->GetOpenGL()->SetSkybox(0);

	delete mRoot;
	mRoot = new GameObject("EmptyScene", nullptr);

	//TODO: If last selected window is game. And new scene is clicked, editor camera is not found
}

const std::string& ModuleScene::GetName() const
{
	return mRoot->GetName();
}

#pragma endregion



