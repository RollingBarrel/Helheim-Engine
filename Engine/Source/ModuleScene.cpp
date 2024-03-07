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
#include "Archive.h"
#include "Tag.h"
#include "Globals.h"
#include "NavMeshController.h"

#include <algorithm>
#include <iterator>

ModuleScene::ModuleScene() {
	mTags.push_back(new Tag(0, "Untagged", TagType::SYSTEM));
	mTags.push_back(new Tag(1, "Respawn", TagType::SYSTEM));
	mTags.push_back(new Tag(2, "Finish", TagType::SYSTEM));
	mTags.push_back(new Tag(3, "EditorOnly", TagType::SYSTEM));
	mTags.push_back(new Tag(4, "MainCamera", TagType::SYSTEM));
	mTags.push_back(new Tag(5, "Player", TagType::SYSTEM));
	mRoot = new GameObject("SampleScene", 1, nullptr, float3::zero, float3::one, Quat::identity);
	mQuadtreeRoot = new Quadtree(AABB(float3(-50), float3(50)));
	mNavMeshController = new NavMeshController();
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
	mRoot = new GameObject("SampleScene", 1, nullptr, float3::zero, float3::one, Quat::identity);
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

void ModuleScene::Save(const char* sceneName) {
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
}

void ModuleScene::Load(const char* sceneName) {
	std::string loadFilePath = "Assets/Scenes/" + std::string(sceneName);
	if (loadFilePath.find(".json") == std::string::npos) {
		loadFilePath += ".json";
	}

	char* loadedBuffer = nullptr;
	App->GetFileSystem()->Load(loadFilePath.c_str(), &loadedBuffer);

	rapidjson::Document d;
	rapidjson::ParseResult ok = d.Parse(loadedBuffer);
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
	mRoot = new GameObject("SampleScene", 1, nullptr, float3::zero, float3::one, Quat::identity);
	

	if (d.HasMember("Scene") && d["Scene"].IsObject()) {
		const rapidjson::Value& s = d["Scene"];
		mRoot->Load(s);
	}

	HierarchyPanel* hierarchyPanel = (HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL);
	hierarchyPanel->SetFocus(mRoot);
	mQuadtreeRoot->UpdateTree();

	// Free the loaded buffer
	delete[] loadedBuffer;
}

void ModuleScene::SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive) {
	// Save the current GameObject to its archive
	Archive gameObjectArchive;
	gameObject->Save(gameObjectArchive);
	gameObjectsArchive.push_back(gameObjectArchive);

	// Save children game objects
	const std::vector<GameObject*>& children = gameObject->GetChildren();
	if (!children.empty()) {
		for (GameObject* child : children) {
			SaveGameObjectRecursive(child, gameObjectsArchive);
		}
	}
}

void ModuleScene::SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive) {
	// Create a vector to store individual game object archives
	std::vector<Archive> gameObjectsArchiveVector;

	// Save each GameObject to the gameObjectsArchiveVector
	for (GameObject* gameObject : gameObjects) {
		SaveGameObjectRecursive(gameObject, gameObjectsArchiveVector);
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
	mNavMeshController->Update();
	GenerateRenderList(mRoot);
	DrawRenderList();
	mRenderList.clear();

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

void ModuleScene::DeleteGameObjects(){

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

void ModuleScene::AddToRenderList(GameObject* root)
{
	mRenderList.push_back(root);
}

void ModuleScene::GenerateRenderList(GameObject* root)
{
	// if engine slows down there is an optimization 
	// HERE on getMeshRenderer
	if (root->GetComponent(ComponentType::MESHRENDERER) != nullptr)
	{
		AddToRenderList(root);
	}
	for (GameObject* child : root->GetChildren())
	{
		GenerateRenderList(child);
	}
}

void ModuleScene::DrawRenderList()
{
	for (GameObject* objectToRender : mRenderList)
	{
		Component* component = objectToRender->GetComponent(ComponentType::MESHRENDERER);
		MeshRendererComponent* meshRenderer = reinterpret_cast<MeshRendererComponent*>(component);

		// Enable/disable mesh renderer component
		if (meshRenderer->IsEnabled() && meshRenderer->GetOwner()->IsActive())
		{
			meshRenderer->Draw();
		}
	}
}