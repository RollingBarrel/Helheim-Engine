#include "ModuleScene.h"
#include "GameObject.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleOpenGL.h"
#include "TestSceneGameObjects.cpp"
#include "Archive.h"
#include "Globals.h"

ModuleScene::ModuleScene() {
	mRoot = new GameObject("SampleScene", 1, nullptr, float3::zero, float3::one, Quat::identity);
	mSelectedGameObject = mRoot;
	mQuadtreeRoot = new Quadtree(AABB(float3(-10), float3(10)));
}

ModuleScene::~ModuleScene()
{
	delete mRoot;
}

bool ModuleScene::Init()
{
	TestSceneGameObjects test = TestSceneGameObjects();
	test.TestSceneWithGameObjects();

	Archive* sceneArchive = new Archive();
	Archive* archive = new Archive();
	SaveGame(mRoot->GetChildren(), *archive);
	sceneArchive->AddObject("Scene", *archive);

	std::string out = sceneArchive->Serialize();


	//INIT For testing purposes of Scene Load
	//const char* json = test.TestLoadSceneWithGameObjectsWithGameObjectsAsChildrenAndComponents();
	rapidjson::Document d;
	rapidjson::ParseResult ok = d.Parse(out.c_str());
	if (!ok) {
		// TODO, what we do if we fail on loading a scene?
		//LOG("Error when loading a scene: %s (%u)", rapidjson::GetParseError(ok.Code()), ok.Offset);
	}

	// Delete all GameObjects before loading the Scene
	const std::vector<GameObject*>& children = mRoot->GetChildren();
	if (!children.empty()) {
		for (GameObject* child : children) {
			mRoot->DeleteChild(child);
		}
	}

	if (d.HasMember("Scene") && d["Scene"].IsObject()) {
		const rapidjson::Value& s = d["Scene"];
		mRoot->Load(s);
	}
	//END For testing purposes of Scene Load

	

	return true;
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
	mQuadtreeRoot->UpdateDrawableGameObjects(App->GetCamera()->GetFrustum());
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update()
{
	mRoot->Update();
	if (mDrawQuadtree)
	{
		App->GetOpenGL()->BindSceneFramebuffer();
		mQuadtreeRoot->Draw();
	}
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

	return UPDATE_CONTINUE;
}

void ModuleScene::DeleteGameObjects(){

	for (auto gameObject : mGameObjectsToDelete) {
		gameObject->GetParent()->DeleteChild(gameObject);
	}

	mGameObjectsToDelete.clear();

}

void ModuleScene::DuplicateGameObjects() {

	for (auto gameObject : mGameObjectsToDuplicate) {
		gameObject->GetParent()->AddChild(gameObject);
	}

	mGameObjectsToDuplicate.clear();

}
