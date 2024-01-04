#include "ModuleScene.h"
#include "GameObject.h"

ModuleScene::ModuleScene() {
	mRoot = new GameObject("SampleScene", nullptr);
	mSelectedGameObject = mRoot;
}

ModuleScene::~ModuleScene()
{
	delete mRoot;
}

bool ModuleScene::Init()
{
	return true;
}

update_status ModuleScene::Update()
{
	mRoot->Update();
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

void ModuleScene::DrawInspector()
{
	if (mSelectedGameObject != mRoot) {
		mSelectedGameObject->DrawInspector();
	}
	
}

void ModuleScene::DrawHierarchy()
{
	mRoot->DrawHierarchy(mSelectedGameObject->GetID());
}

void ModuleScene::SetSelectedObject(GameObject* gameObject)
{
	mSelectedGameObject = gameObject;
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
