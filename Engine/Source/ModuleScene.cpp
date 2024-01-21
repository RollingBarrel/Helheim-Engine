#include "ModuleScene.h"
#include "GameObject.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "glew.h"
#include "ModuleProgram.h"
#include "MeshRendererComponent.h"
#include "Component.h"

#include "ModuleOpenGL.h"

ModuleScene::ModuleScene() {
	mRoot = new GameObject("SampleScene", nullptr);
	mSelectedGameObject = mRoot;
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
	if (mDrawQuadtree)
	{
		App->GetOpenGL()->BindSceneFramebuffer();
		mQuadtreeRoot->Draw();
	}

	//float4x4 viewProj = App->GetCamera()->GetViewProjMatrix();
	//auto program = App->GetProgram()->GetProgramID("default");
	//glUseProgram(program);
	//glUniformMatrix4fv(1, 1, GL_TRUE, &viewProj[0][0]); // first argument is 1 for the layout in vertex shader

	GameObject* root = App->GetScene()->GetRoot();
	mRenderList.clear();
	GenerateRenderList(root);
	DrawRenderList();

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
		//Pass model matrix
		float4x4 model = objectToRender->GetWorldTransform();
		auto program = App->GetProgram()->GetProgramID("default");
		glUseProgram(program);
		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]); // first argument is 0 for the layout in vertex shader
		//Render
		//
		Component* component = objectToRender->GetComponent(ComponentType::MESHRENDERER);
		MeshRendererComponent* meshRenderer = dynamic_cast<MeshRendererComponent*>(component);

		meshRenderer->Draw();
	}
}