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
		//glEnable(GL_DEPTH_TEST);
	}
	else {
		mCurrentFrustum = (Frustum*)(App->GetCamera()->GetFrustum());
		//glDisable(GL_DEPTH_TEST);
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

	mUIfrustum->orthographicWidth = widthFrustum; //Change with canvas width
	mUIfrustum->orthographicHeight = heightFrustum; //Change with canvas height
}

void ModuleUI::CheckRaycast()
{
	ScenePanel* scenePanel = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL));

	int mouseAbsoluteX = scenePanel->GetMousePosition().x;
	int mouseAbsoluteY = scenePanel->GetMousePosition().y;

	float normalizedX = mouseAbsoluteX - scenePanel->GetWindowsSize().x / 2;
	float normalizedY = -(mouseAbsoluteY - scenePanel->GetWindowsSize().y / 2);
	if (!mCanvas->GetChildren().empty()) {
		for (GameObject* gameObject : mCanvas->GetChildren())
		{
			ImageComponent* image = (ImageComponent*)gameObject->GetComponent(ComponentType::IMAGE);
			Transform2DComponent* transform2D = (Transform2DComponent*)gameObject->GetComponent(ComponentType::TRANSFORM2D);
			if (image != nullptr && transform2D != nullptr)
			{
				//float2 offset = float2(image->GetImage()->GetWidth(), image->GetImage()->GetHeight()) / 2.0f;
				//float2 screenOffset = scenePanel->GetWindowsSize() / 2.0f;
				float2 minImagePoint = transform2D->GetGlobalPosition().xy() - transform2D->GetSize();
				float2 maxImagePoint = transform2D->GetGlobalPosition().xy() + transform2D->GetSize();
				// Check if the mouse position is inside the bounds of the image
				if (normalizedX >= minImagePoint.x && normalizedY >= minImagePoint.y &&
					normalizedX <= maxImagePoint.x && normalizedY <= maxImagePoint.y)
				{
					LOG("Button Clicked");
					//ButtonComponent* button = (ButtonComponent*)gameObject->GetComponent(ComponentType::BUTTON);
					//if (button != nullptr && button->IsEnabled())
					//{
					//	button->OnClicked();
					//}
				}
			}
		}
	}
}