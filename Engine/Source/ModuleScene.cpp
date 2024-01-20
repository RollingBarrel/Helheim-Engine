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
	mRoot = new GameObject("SampleScene", nullptr);
	mQuadtreeRoot = new Quadtree(AABB(float3(-10), float3(10)));
}

ModuleScene::~ModuleScene()
{
	delete mRoot;
}

bool ModuleScene::Init()
{
	TestSceneGameObjects test = TestSceneGameObjects();
	test.TestSceneWithGameObject();

	Archive* archive = new Archive();
	mRoot->Save(*archive);

	std::string out = archive->Serialize();


	//INIT For testing purposes of Scene Load
	const char* json = test.TestLoadSceneWithGameObjectsWithGameObjectsAsChildrenAndComponents();
	rapidjson::Document d;
	rapidjson::ParseResult ok = d.Parse(json);
	if (!ok) {
		// TODO, what we do if we fail on loading a scene?
		//LOG("Error when loading a scene: %s (%u)", rapidjson::GetParseError(ok.Code()), ok.Offset);
	}
	if (d.HasMember("Scene") && d["Scene"].IsObject()) {
		const rapidjson::Value& s = d["Scene"];
		mRoot->Load(s);
	}
	//END For testing purposes of Scene Load

	

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
