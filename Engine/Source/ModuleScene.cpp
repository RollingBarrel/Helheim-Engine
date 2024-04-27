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
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleResource.h"
#include "Archive.h"
#include "Tag.h"
#include "Globals.h"
#include "NavMeshController.h"

#include <algorithm>
#include <iterator>
#include "Algorithm/Random/LCG.h"

#include "GeometryBatch.h"
#include "ImporterMesh.h"

ModuleScene::ModuleScene() {
	mNavMeshController = new NavMeshController();

	mTags.push_back(new Tag(0, "Untagged", TagType::SYSTEM));
	mTags.push_back(new Tag(1, "Respawn", TagType::SYSTEM));
	mTags.push_back(new Tag(2, "Finish", TagType::SYSTEM));
	mTags.push_back(new Tag(3, "EditorOnly", TagType::SYSTEM));
	mTags.push_back(new Tag(4, "MainCamera", TagType::SYSTEM));
	mTags.push_back(new Tag(5, "Player", TagType::SYSTEM));
	mTags.push_back(new Tag(6, "Obstacle", TagType::SYSTEM));
	mTags.push_back(new Tag(7, "Enemy", TagType::SYSTEM));
	mTags.push_back(new Tag(8, "CombatArea", TagType::SYSTEM));

}

ModuleScene::~ModuleScene()
{
	mQuadtreeRoot->CleanUp();
	delete mQuadtreeRoot;
	delete mNavMeshController;

	delete mRoot;
	delete mBackgroundScene;

	for (Tag* tag : mTags) 
	{
		delete tag;
	}
}

bool ModuleScene::Init()
{
	mRoot = new GameObject("SampleScene", nullptr);
	mQuadtreeRoot = new Quadtree(AABB(float3(-5000 , -500 , -5000), float3(5000, 500, 5000)));

	//Load("scene");
	//Load("MainMenu");
	//Load("Level1");


	return true;
}

void ModuleScene::ResetFrustumCulling(GameObject* obj)
{
	MeshRendererComponent* meshRend = (MeshRendererComponent*)obj->GetComponent(ComponentType::MESHRENDERER);
	if (meshRend != nullptr)
	{
		meshRend->SetInsideFrustum(false);
	}
	for (GameObject* child : obj->GetChildren())
	{
		ResetFrustumCulling(child);
	}

}

GameObject* ModuleScene::FindGameObjectWithTag(GameObject* root, unsigned tagID)
{
	if (root->GetTag()->GetID() == tagID && root != mRoot) 
	{
		return root;
	}

	for (GameObject* child : root->GetChildren())
	{
		GameObject* foundObject = FindGameObjectWithTag(child, tagID);
		if (foundObject != nullptr) 
		{
			return foundObject;
		}
	}

	return nullptr;
}

GameObject* ModuleScene::FindGameObjectWithTag(unsigned tagID)
{
	return FindGameObjectWithTag(mRoot, tagID);
}

GameObject* ModuleScene::FindGameObjectWithTag(const char* tagName)
{
	Tag* tag = App->GetScene()->GetTagByName(tagName);
	return FindGameObjectWithTag(mRoot, tag->GetID());
}

void ModuleScene::FindGameObjectsWithTag(GameObject* root, unsigned tagid, std::vector<GameObject*>& foundGameObjects)
{
	if (root->GetTag()->GetID() == tagid && root != mRoot) 
	{
		foundGameObjects.push_back(root);
	}

	for (GameObject* child : root->GetChildren())
	{
		FindGameObjectsWithTag(child, tagid, foundGameObjects);
	}
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

void ModuleScene::Save(const char* sceneName) const 
{
	std::string saveFilePath = "Assets/Scenes/" + std::string(sceneName);
	if (saveFilePath.find(".json") == std::string::npos) 
	{
		saveFilePath += ".json";
	}

	Archive* sceneArchive = new Archive();
	Archive* archive = new Archive();

	SaveGame(mRoot->GetChildren(), *archive);
	sceneArchive->AddObject("Scene", *archive);

	std::string out = sceneArchive->Serialize();
	App->GetFileSystem()->Save(saveFilePath.c_str(), out.c_str(), static_cast<unsigned int>(out.length()));
	delete sceneArchive;
	delete archive;
}

int ModuleScene::SavePrefab(const GameObject* gameObject, const char* saveFilePath) const 
{
	//TODO: separate game engine
	//unsigned int resourceId = LCG().Int();
	//Resource* resource = App->GetResource()->RequestResource(mPrefabPath);
	//if (resource != nullptr) { resourceId = resource->GetUID(); }
	//Archive* prefabArchive = new Archive();
	//Archive* archive = new Archive();
	//std::vector<Archive> gameObjectsArchiveVector;
	//SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector, gameObject->GetParent()->GetID());
	////SaveGame(gameObject->GetChildren(), *archive);
	//archive->AddObjectArray("GameObjects", gameObjectsArchiveVector);
	//prefabArchive->AddObject("Prefab", *archive);
	//
	//std::string out = prefabArchive->Serialize();
	//App->GetFileSystem()->Save(saveFilePath, out.c_str(), static_cast<unsigned int>(out.length()));
	////App->GetResource()->ImportFile(saveFilePath, resourceId);
	//PathNode* root = App->GetFileSystem()->GetRootNode();
	//root->mChildren.clear();
	//App->GetFileSystem()->DiscoverFiles("Assets", root);
	//delete prefabArchive;
	//delete archive;
	//return resourceId;
	return 0;
}


void ModuleScene::Load(const char* sceneName) 
{
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
		mRoot = new GameObject(sceneName, nullptr);


		if (document.HasMember("Scene") && document["Scene"].IsObject()) 
		{
			const rapidjson::Value& sceneValue = document["Scene"];
			mRoot->Load(sceneValue);
		}

		mQuadtreeRoot->UpdateTree();
		delete[] loadedBuffer;

		LoadGameObjectsIntoScripts();
	}

	GameObject* cameraGameObject = App->GetScene()->FindGameObjectWithTag("MainCamera");
	if (cameraGameObject)
	{
		App->GetCamera()->SetCurrentCamera(cameraGameObject);
	}

	if (App->IsPlayMode())
	{
		App->GetScriptManager()->Start();
	}
	

}

void ModuleScene::LoadPrefab(const char* saveFilePath, unsigned int resourceId, bool update)
{
	char* loadedBuffer = nullptr;
	App->GetFileSystem()->Load(saveFilePath, &loadedBuffer);

	rapidjson::Document d;
	rapidjson::ParseResult ok = d.Parse(loadedBuffer);
	if (!ok) 
	{
		LOG("Object was not loaded.");
		return;
	}

	if (d.HasMember("Prefab") && d["Prefab"].IsObject()) 
	{
		const rapidjson::Value& s = d["Prefab"];
		if (update) { mRoot->LoadChangesPrefab(s, resourceId); }
		else 
		{
			GameObject* temp = new GameObject("Temp", mRoot);
			temp->Load(s);
			for (GameObject* child : temp->GetChildren()) 
			{
				child->ResetTransform();
				child->SetPrefabId(resourceId);
				mRoot->AddChild(child);
				temp->RemoveChild(child->GetID());
			}
			mRoot->DeleteChild(temp);
		}
	}

	delete[] loadedBuffer;
}

void ModuleScene::OpenPrefabScreen(const char* saveFilePath)
{
	if (mBackgroundScene != nullptr) { mClosePrefab = true; }
	mPrefabPath = saveFilePath;
}

void ModuleScene::ClosePrefabScreen()
{
	if (mBackgroundScene != nullptr) 
	{
		mClosePrefab = true;
	}
}

GameObject* ModuleScene::Find(const char* name) const
{
	return mRoot->Find(name);
}

GameObject* ModuleScene::Find(unsigned int UID) const
{
	if (UID != mRoot->GetID()) 
	{
		return mRoot->Find(UID);
	}
	else 
	{
		return mRoot;
	}

}

void ModuleScene::SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive, int parentUuid) const  
{
	// Save the current GameObject to its archive
	Archive gameObjectArchive;
	gameObject->Save(gameObjectArchive, parentUuid);
	gameObjectsArchive.push_back(gameObjectArchive);

	const std::vector<GameObject*>& children = gameObject->GetChildren();
	if (!children.empty()) 
	{
		for (GameObject* child : children) 
		{
			SaveGameObjectRecursive(child, gameObjectsArchive, parentUuid);
		}
	}
}

void ModuleScene::SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive) const 
{
	
	std::vector<Archive> gameObjectsArchiveVector;

	for (GameObject* gameObject : gameObjects) 
	{
		SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector, mRoot->GetID());
	}

	rootArchive.AddObjectArray("GameObjects", gameObjectsArchiveVector);
}

update_status ModuleScene::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}


update_status ModuleScene::Update(float dt)
{
	mShouldUpdateQuadtree = false;
	mRoot->Update();
	if (mNavMeshController)
	{
		mNavMeshController->Update();
	}
	App->GetOpenGL()->Draw();

	if (mShouldUpdateQuadtree)
	{
		mQuadtreeRoot->UpdateTree();
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
		int resourceId = SavePrefab(mRoot->GetChildren()[0], mPrefabPath);
		delete mRoot;
		mRoot = mBackgroundScene;
		mBackgroundScene = nullptr;
		mRoot->SetEnabled(true);
		LoadPrefab(mPrefabPath, resourceId, true);
		mQuadtreeRoot->UpdateTree();
		mPrefabPath = "";
		mClosePrefab = false;
	}
	if (mPrefabPath != "" && mBackgroundScene == nullptr) 
	{
		Resource* resource = App->GetResource()->RequestResource(mPrefabPath);
		if (resource->GetType() == Resource::Type::Object) 
		{
			mBackgroundScene = mRoot;
			mBackgroundScene->SetEnabled(false);
			mRoot = new GameObject(mPrefabPath, nullptr);
			LoadPrefab(mPrefabPath, resource->GetUID());
			mQuadtreeRoot->UpdateTree();
		}
	}

	const Frustum frustum = ((CameraComponent*)App->GetCamera()->GetCurrentCamera())->GetFrustum();

	return UPDATE_CONTINUE;
}

void ModuleScene::DeleteGameObjects() 
{

	for (auto gameObject : mGameObjectsToDelete) 
	{
		gameObject->GetParent()->DeleteChild(gameObject);
	}

	mGameObjectsToDelete.clear();
	mQuadtreeRoot->UpdateTree();
}

void ModuleScene::DuplicateGameObjects() 
{

	for (auto gameObject : mGameObjectsToDuplicate) 
	{
		gameObject->GetParent()->AddChild(gameObject);
	}

	mGameObjectsToDuplicate.clear();
	mQuadtreeRoot->UpdateTree();

}

void ModuleScene::LoadGameObjectsIntoScripts()
{
	for (auto& pair : mGameObjectsToLoadIntoScripts) 
	{
		*(pair.second) = Find(pair.first);
	}
}
