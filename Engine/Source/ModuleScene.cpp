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
	return UPDATE_CONTINUE;
}

void ModuleScene::DrawEditor()
{
}
