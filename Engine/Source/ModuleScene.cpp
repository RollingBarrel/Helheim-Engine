#include "ModuleScene.h"
#include "GameObject.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"

ModuleScene::ModuleScene() {
	mRoot = new GameObject("SampleScene", nullptr);
	mQuadtreeRoot = new Quadtree(AABB(float3(-10), float3(10)));
}

ModuleScene::~ModuleScene()
{
	delete mRoot;
}

bool ModuleScene::Init()
{
	return true;
}

update_status ModuleScene::PreUpdate()
{
	mQuadtreeRoot->UpdateDrawableGameObjects(App->GetCamera()->GetFrustum());
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update()
{
	mRoot->Update();
	if (*mDrawQuadtree)
	{
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
