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
#include "Globals.h"

#include "GeometryBatch.h"
#include "ImporterMesh.h"

ModuleScene::ModuleScene() {
	mRoot = new GameObject("SampleScene", 1, nullptr, float3::zero, float3::one, Quat::identity);
	mQuadtreeRoot = new Quadtree(AABB(float3(-50), float3(50)));
}

ModuleScene::~ModuleScene()
{
	mQuadtreeRoot->CleanUp();
	delete mQuadtreeRoot;

	delete mRoot;
}

bool ModuleScene::Init()
{
	//TestSceneGameObjects test = TestSceneGameObjects();
	//test.TestSceneWithGameObjects();

	//Save("Scene");
	//Load("scene");

	return true;
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

update_status ModuleScene::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update()
{
	mRoot->Update();
	if (mDrawQuadtree)
	{
		App->GetOpenGL()->BindSceneFramebuffer();
		mQuadtreeRoot->Draw();
		App->GetOpenGL()->UnbindSceneFramebuffer();
	}

	GenerateRenderList(mRoot);
	DrawRenderList();
	mRenderList.clear();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
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

		unsigned int instanceCounter = 0;
		// Enable/disable mesh renderer component
		if(meshRenderer->IsEnabled())
		{
			
			if (mApplyculling) {
				if (meshRenderer->IsInsideFrustum()) {
					
					meshRenderer->AddCommand(instanceCounter);
					instanceCounter++;
				}
			}
			else {

				meshRenderer->AddCommand(instanceCounter);
				instanceCounter++;
			}
			
		}
	}

	App->GetOpenGL()->Draw();
}