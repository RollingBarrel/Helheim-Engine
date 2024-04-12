#include "ModuleScene.h"
#include "GameObject.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "glew.h"
#include "MeshRendererComponent.h"
#include "Component.h"

#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"
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
}

ModuleScene::~ModuleScene()
{
	mQuadtreeRoot->CleanUp();
	delete mQuadtreeRoot;
	delete mNavMeshController;

	delete mRoot;

	for (Tag* tag : mTags) {
		delete tag;
	}
}

bool ModuleScene::Init()
{
	mRoot = new GameObject("SampleScene", nullptr);
	mQuadtreeRoot = new Quadtree(AABB(float3(-50), float3(50)));
	//TestSceneGameObjects test = TestSceneGameObjects();
	//test.TestSceneWithGameObjects();

	//Save("Scene");
	Load("scene");

	return true;
}

GameObject* ModuleScene::FindGameObjectWithTag(GameObject* root, unsigned tagid)
{
	if (root->GetTag()->GetID() == tagid && root != mRoot) {
		return root;
	}

	for (GameObject* child : root->GetChildren())
	{
		GameObject* foundObject = FindGameObjectWithTag(child, tagid);
		if (foundObject != nullptr) {
			return foundObject;
		}
	}

	return nullptr;
}

void ModuleScene::FindGameObjectsWithTag(GameObject* root, unsigned tagid, std::vector<GameObject*>& foundGameObjects)
{
	if (root->GetTag()->GetID() == tagid && root != mRoot) {
		foundGameObjects.push_back(root);
	}

	for (GameObject* child : root->GetChildren())
	{
		FindGameObjectsWithTag(child, tagid, foundGameObjects);
	}
}

void ModuleScene::AddTag(std::string tagname)
{
	if (GetTagByName(tagname) == nullptr) {
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
	for (Tag* tag : mTags) {
		if (std::strcmp(tag->GetName().c_str(), tagname.c_str()) == 0) {
			return tag;
		}
	}
	return nullptr;
}

Tag* ModuleScene::GetTagByID(unsigned id)
{
	for (Tag* tag : mTags) {
		if (tag->GetID() == id) {
			return tag;
		}
	}
	return nullptr;
}

void ModuleScene::DeleteTag(Tag* tag)
{
	auto it = std::find(mTags.begin(), mTags.end(), tag);

	if (it != mTags.end()) {

		// 1. Set tags to untagged
		std::vector<GameObject*> objects = GameObject::FindGameObjectsWithTag(tag->GetName());
		for (auto object : objects) {
			object->SetTag(GetTagByName("Untagged"));
		}

		// 2. Delete it
		mTags.erase(it);
		delete tag;
	}
}

void ModuleScene::Save(const char* sceneName) const {
	std::string saveFilePath = "Assets/Scenes/" + std::string(sceneName);
	if (saveFilePath.find(".json") == std::string::npos) {
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

void ModuleScene::SavePrefab(const GameObject* gameObject, const char* saveFilePath) const {
	Archive* prefabArchive = new Archive();
	Archive* archive = new Archive();
	std::vector<Archive> gameObjectsArchiveVector;
	SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector, gameObject->GetParent()->GetID());
	//SaveGame(gameObject->GetChildren(), *archive);
	archive->AddObjectArray("GameObjects", gameObjectsArchiveVector);
	prefabArchive->AddObject("Prefab", *archive);

	std::string out = prefabArchive->Serialize();
	App->GetFileSystem()->Save(saveFilePath, out.c_str(), static_cast<unsigned int>(out.length()));
	App->GetResource()->ImportFile(saveFilePath, LCG().Int()); //TODO Generate UUID
	PathNode* root = App->GetFileSystem()->GetRootNode();
	root->mChildren.clear();
	App->GetFileSystem()->DiscoverFiles("Assets", root);
	delete archive;
}

void ModuleScene::Load(const char* sceneName) {

	std::string loadFilePath = "Assets/Scenes/" + std::string(sceneName);
	if (loadFilePath.find(".json") == std::string::npos) {
		loadFilePath += ".json";
	}

	char* loadedBuffer = nullptr;

	if (App->GetFileSystem()->Load(loadFilePath.c_str(), &loadedBuffer) > 0)
	{
		rapidjson::Document document;
		rapidjson::ParseResult ok = document.Parse(loadedBuffer);
		if (!ok) {
			// Handle parsing error
			LOG("Scene was not loaded.");
			return;
		}

		/*const std::vector<GameObject*>& children = mRoot->GetChildren();
		if (!children.empty()) {
			for (GameObject* child : children) {
				mRoot->DeleteChild(child);
			}
		}*/
		mQuadtreeRoot->CleanUp();
		delete mRoot;
		mRoot = new GameObject("SampleScene", nullptr);


		if (document.HasMember("Scene") && document["Scene"].IsObject()) {
			const rapidjson::Value& sceneValue = document["Scene"];
			mRoot->Load(sceneValue);
		}

		HierarchyPanel* hierarchyPanel = (HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL);
		hierarchyPanel->SetFocus(mRoot);
		mQuadtreeRoot->UpdateTree();

		// Free the loaded buffer
		delete[] loadedBuffer;

		LoadGameObjectsIntoScripts();
	}
}

void ModuleScene::LoadPrefab(const char* saveFilePath)
{
	char* loadedBuffer = nullptr;
	App->GetFileSystem()->Load(saveFilePath, &loadedBuffer);

	rapidjson::Document d;
	rapidjson::ParseResult ok = d.Parse(loadedBuffer);
	if (!ok) {
		// Handle parsing error
		LOG("Object was not loaded.");
		return;
	}

	if (d.HasMember("Prefab") && d["Prefab"].IsObject()) {
		const rapidjson::Value& s = d["Prefab"];
		mRoot->Load(s);
	}

	// Free the loaded buffer
	delete[] loadedBuffer;
}

GameObject* ModuleScene::Find(const char* name)
{
	return mRoot->Find(name);

}

GameObject* ModuleScene::Find(unsigned int UID)
{
	if (UID != 1) {
		return mRoot->Find(UID);
	}
	else {
		return mRoot;
	}

}

void ModuleScene::SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive, int parentUuid) const {
	// Save the current GameObject to its archive
	Archive gameObjectArchive;
	gameObject->Save(gameObjectArchive, parentUuid);
	gameObjectsArchive.push_back(gameObjectArchive);

	// Save children game objects
	const std::vector<GameObject*>& children = gameObject->GetChildren();
	if (!children.empty()) {
		for (GameObject* child : children) {
			SaveGameObjectRecursive(child, gameObjectsArchive, parentUuid);
		}
	}
}

void ModuleScene::SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive) const {
	// Create a vector to store individual game object archives
	std::vector<Archive> gameObjectsArchiveVector;

	// Save each GameObject to the gameObjectsArchiveVector
	for (GameObject* gameObject : gameObjects) {
		SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector, mRoot->GetID());
	}

	// Add the gameObjectsArchiveVector to the root archive under the key "gameobjects"
	rootArchive.AddObjectArray("GameObjects", gameObjectsArchiveVector);
}

update_status ModuleScene::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	mRoot->Update();
	if (mDrawQuadtree)
	{
		App->GetOpenGL()->BindSceneFramebuffer();
		mQuadtreeRoot->Draw();
		App->GetOpenGL()->UnbindSceneFramebuffer();
	}
	if (mNavMeshController)
	{
		mNavMeshController->Update();
	}
	App->GetOpenGL()->Draw();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	if (!mGameObjectsToDelete.empty()) {
		DeleteGameObjects();
	}
	if (!mGameObjectsToDuplicate.empty()) {
		DuplicateGameObjects();
	}

	mQuadtreeRoot->UpdateDrawableGameObjects(App->GetCamera()->GetFrustum());

	return UPDATE_CONTINUE;
}

void ModuleScene::DeleteGameObjects() {

	for (auto gameObject : mGameObjectsToDelete) {
		gameObject->GetParent()->DeleteChild(gameObject);
	}

	mGameObjectsToDelete.clear();
	mQuadtreeRoot->UpdateTree();
}

void ModuleScene::DuplicateGameObjects() {

	for (auto gameObject : mGameObjectsToDuplicate) {
		gameObject->GetParent()->AddChild(gameObject);
	}

	mGameObjectsToDuplicate.clear();
	mQuadtreeRoot->UpdateTree();

}

void ModuleScene::LoadGameObjectsIntoScripts()
{

	for (auto& pair : mGameObjectsToLoadIntoScripts) {
		*pair.second = Find(pair.first);
	}
}