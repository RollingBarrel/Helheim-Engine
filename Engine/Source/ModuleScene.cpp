#include "ModuleScene.h"
#include "GameObject.h"

ModuleScene::ModuleScene() {
	mRoot = new GameObject("SampleScene", nullptr);
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
	DrawEditor();
	return UPDATE_CONTINUE;
}

void ModuleScene::DrawEditor()
{
	//This is a placeholder for testing.
	//Here we should call the DrawEditor of the GameObject that has Focus
	mRoot->DrawEditor();
}
