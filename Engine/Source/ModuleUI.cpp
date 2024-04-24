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
}

ModuleUI::~ModuleUI() 
{
}

bool ModuleUI::Init() 
{
	FindCanvas(App->GetScene()->GetRoot());
	if (mCanvas == nullptr) 
	{
		mCanvas = new GameObject("Canvas", App->GetScene()->GetRoot());
		mCanvas->CreateComponent(ComponentType::CANVAS);
	}

	return true;
}

update_status ModuleUI::PreUpdate(float dt) 
{
	// Draw the UI
	App->GetOpenGL()->BindSceneFramebuffer();
	if (mCanvas) 
	{
		DrawWidget(mCanvas);
	}
	App->GetOpenGL()->UnbindSceneFramebuffer();

	return UPDATE_CONTINUE;
}

update_status ModuleUI::Update(float dt) 
{
	if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		CheckRaycast();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt) 
{
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp() 
{
	mCanvas = nullptr;
	return true;
}

void ModuleUI::DrawWidget(GameObject* gameObject)
{
	if (!gameObject) return;

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

void ModuleUI::FindCanvas(GameObject* gameObject)
{
	mCanvas = nullptr;
	if (gameObject->GetComponent(ComponentType::CANVAS) != nullptr) 
	{
		mCanvas = gameObject;
		return;
	}
	else 
	{
		for (GameObject* go : gameObject->GetChildren()) 
		{
			FindCanvas(go);
			if (mCanvas != nullptr) 
			{
				return;
			}
		}
	}

	mCanvas = nullptr;
}

void ModuleUI::CheckRaycastRecursive(GameObject* gameObject, bool& eventTriggered) 
{
	if (gameObject == nullptr || eventTriggered)
		return;

	for (auto child : gameObject->GetChildren()) 
	{
		ImageComponent* image = static_cast<ImageComponent*>(child->GetComponent(ComponentType::IMAGE));
		Transform2DComponent* transform2D = static_cast<Transform2DComponent*>(child->GetComponent(ComponentType::TRANSFORM2D));
		ButtonComponent* button = static_cast<ButtonComponent*>(child->GetComponent(ComponentType::BUTTON));

		if (image != nullptr && transform2D != nullptr && button != nullptr && button->IsEnabled()) 
		{
			int mouseAbsoluteX = App->GetInput()->GetGameMousePosition().x;
			int mouseAbsoluteY = App->GetInput()->GetGameMousePosition().y;

			float normalizedX = -1.0f + 2.0f * static_cast<float>(mouseAbsoluteX - App->GetWindow()->GetGameWindowsPosition().x) / static_cast<float>(App->GetWindow()->GetGameWindowsSize().x);
			float normalizedY = 1.0f - 2.0f * static_cast<float>(mouseAbsoluteY - App->GetWindow()->GetGameWindowsPosition().y) / static_cast<float>(App->GetWindow()->GetGameWindowsSize().y);

			float mouseX = normalizedX;
			float mouseY = normalizedY;

			float2 canvasSize = static_cast<CanvasComponent*>(image->FindCanvasOnParents(image->GetOwner())->GetComponent(ComponentType::CANVAS))->GetSize();
			float2 minImagePoint = transform2D->GetGlobalMatrix().TranslatePart().xy().Mul(float2(2.0f, 2.0f)).Div(canvasSize) - transform2D->GetSize().Div(canvasSize);
			float2 maxImagePoint = transform2D->GetGlobalMatrix().TranslatePart().xy().Mul(float2(2.0f, 2.0f)).Div(canvasSize) + transform2D->GetSize().Div(canvasSize);

			// Check if the mouse position is inside the bounds of the image
			if (mouseX >= minImagePoint.x && mouseY >= minImagePoint.y && mouseX <= maxImagePoint.x && mouseY <= maxImagePoint.y) 
			{
				// Click event (button released after press)
				if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_UP) 
				{
					button->TriggerEvent(EventType::Click);
					eventTriggered = true;
					return; // Terminate function
				}
				// Button pressed
				else if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN) 
				{
					button->TriggerEvent(EventType::Press);
					eventTriggered = true;
					return; // Terminate function
				}
				// Mouse hover
				else 
				{
					button->TriggerEvent(EventType::Hover);
					eventTriggered = true;
					return; // Terminate function
				}
			}
		}

		// Recursively check children
		CheckRaycastRecursive(child, eventTriggered);
	}
}

void ModuleUI::CheckRaycast() 
{
	bool eventTriggered = false;
	CheckRaycastRecursive(mCanvas, eventTriggered);
}

