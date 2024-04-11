#include "ModuleUI.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "Application.h"

#include "GameObject.h"
#include "CanvasComponent.h"
#include "ImageComponent.h"
#include "Transform2DComponent.h"

#include "glew.h"
#include "SDL.h"
#include "ButtonComponent.h"
#include "ResourceTexture.h"
#include "ScenePanel.h"
#include "Quadtree.h"
#include <MathGeoLib.h>

ModuleUI::ModuleUI() 
{
};

ModuleUI::~ModuleUI() 
{
};

bool ModuleUI::Init() {
	
	mCanvas = FindCanvas(App->GetScene()->GetRoot());
	if (mCanvas == nullptr) 
	{
		mCanvas = new GameObject("Canvas", App->GetScene()->GetRoot());
		mCanvas->CreateComponent(ComponentType::CANVAS);
	}



	mUIfrustum = new Frustum();

	mUIfrustum->type = FrustumType::OrthographicFrustum;
	//mUIfrustum->orthographicWidth = App->GetWindow()->GetWidth();
	//mUIfrustum->orthographicHeight = App->GetWindow()->GetHeight();

	mUIfrustum->front = -float3::unitZ;
	mUIfrustum->up = float3::unitY;
	mUIfrustum->pos = float3::zero;

	mUIfrustum->nearPlaneDistance = 0.0f;
	mUIfrustum->farPlaneDistance = 100.0f;

	return true;
};

update_status ModuleUI::PreUpdate(float dt) {
	if (mScreenSpace) {
		mCurrentFrustum = mUIfrustum;
		mUIfrustum->orthographicWidth = App->GetWindow()->GetWidth();
		mUIfrustum->orthographicHeight = App->GetWindow()->GetHeight();
	}
	else {
		mCurrentFrustum = (Frustum*)(App->GetCamera()->GetFrustum());
	}

	// Draw the UI
	App->GetOpenGL()->BindSceneFramebuffer();
	DrawWidget(mCanvas);
	App->GetOpenGL()->UnbindSceneFramebuffer();
	return UPDATE_CONTINUE;
}

update_status ModuleUI::Update(float dt) {
	if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		CheckRaycast();
	}

	return UPDATE_CONTINUE;
};

update_status ModuleUI::PostUpdate(float dt) {
	return UPDATE_CONTINUE;
};

bool ModuleUI::CleanUp() {

	delete mUIfrustum;

	return true;
}

void ModuleUI::DrawWidget(GameObject* gameObject)
{
	if (gameObject == nullptr) return;

	if (gameObject->IsEnabled())
	{
		for (Component* component : gameObject->GetComponents(ComponentType::IMAGE))
		{
			ImageComponent* image = (ImageComponent*)component;
			if (image->IsEnabled())
			{
				image->Draw();
			}
		}

		for (GameObject* child : gameObject->GetChildren())
		{
			DrawWidget(child);
		}
	}
}

GameObject* ModuleUI::FindCanvas(GameObject* gameObject)
{
	if (gameObject->GetComponent(ComponentType::CANVAS) != nullptr) 
	{
		return gameObject;
	}

	for (GameObject* go : gameObject->GetChildren()) {
		if (FindCanvas(go) != nullptr) {
			return go;
		}
	}
	
	return nullptr;
}

void ModuleUI::ResizeFrustum(unsigned int width, unsigned int height) {
	float heightFrustum = height;
	float widthFrustum = width;

	ScenePanel* scenePanel = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL));

	mUIfrustum->orthographicWidth = scenePanel->GetWindowsSize().x;  //widthFrustum; //Change with canvas width
	mUIfrustum->orthographicHeight = scenePanel->GetWindowsSize().y;  //heightFrustum; //Change with canvas height
}

void ModuleUI::CheckRaycast()
{
	ScenePanel* scenePanel = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL));

	float mouseSceneX = scenePanel->GetMousePosition().x - scenePanel->GetWindowsPos().x;
	float mouseSceneY = scenePanel->GetMousePosition().y - scenePanel->GetWindowsPos().y;

	//float2 pos = scenePanel->GetWindowsPos();

	float mouseX = (mouseSceneX) - scenePanel->GetWindowsSize().x / 2;
	float mouseY = -((mouseSceneY) - scenePanel->GetWindowsSize().y / 2);

	//float normalizedX = (- 1.0 + 2.0 * (float)(mouseSceneX) / (float)scenePanel->GetWindowsSize().x) * (float)scenePanel->GetWindowsSize().x;
	//float normalizedY = (1.0 - 2.0 * (float)(mouseSceneY) / (float)scenePanel->GetWindowsSize().y) * (float)scenePanel->GetWindowsSize().y;

	//float mouseX = normalizedX - 70;
	//float mouseY = normalizedY + 70;
	
	if (!mCanvas->GetChildren().empty()) {
		for (GameObject* gameObject : mCanvas->GetChildren())
		{
			ImageComponent* image = (ImageComponent*)gameObject->GetComponent(ComponentType::IMAGE);
			Transform2DComponent* transform2D = (Transform2DComponent*)gameObject->GetComponent(ComponentType::TRANSFORM2D);
			if (image != nullptr && transform2D != nullptr)
			{
				float2 minImagePoint = transform2D->GetGlobalPosition().xy() - transform2D->GetSize()/2;
				float2 maxImagePoint = transform2D->GetGlobalPosition().xy() + transform2D->GetSize()/2;
				
				// Check if the mouse position is inside the bounds of the image
				if (mouseX >= minImagePoint.x && mouseY >= minImagePoint.y &&
					mouseX <= maxImagePoint.x && mouseY <= maxImagePoint.y)
				{
					LOG("Button Clicked");
					//ButtonComponent* button = (ButtonComponent*)gameObject->GetComponent(ComponentType::BUTTON);
					//if (button != nullptr && button->IsEnabled())
					//{
					//	button->OnClicked();
					//}
					image->SetColor((image->GetColor()->x == 1.0f) ? float3(0, 0, 0) : float3(1, 1, 1));
				}
			}
		}
	}
}