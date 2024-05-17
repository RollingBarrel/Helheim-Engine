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
#include "TextComponent.h"
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
	return true;
}

update_status ModuleUI::PreUpdate(float dt) 
{
	

	return UPDATE_CONTINUE;
}

update_status ModuleUI::Update(float dt) 
{
	// Draw the UI
	//App->GetOpenGL()->BindSceneFramebuffer();
	//DrawWidget();
	//App->GetOpenGL()->UnbindSceneFramebuffer();
	

	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt) 
{
	// TODO: Check if app is on Menu or Pause
	CheckRaycast();

	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp() 
{
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

		for (Component* component : gameObject->GetComponents(ComponentType::TEXT))
		{
			TextComponent* text = (TextComponent*)component;
			if (text->IsEnabled())
			{
				text->Draw();
			}
		}

		for (GameObject* child : gameObject->GetChildren())
		{
			DrawWidget(child);
		}
	}
}

void ModuleUI::CheckRaycastRecursive(GameObject* gameObject, bool& eventTriggered) 
{
	if (gameObject == nullptr || eventTriggered) return;
	if (gameObject->GetChildren().empty() || gameObject->GetChildren().size() == 0) return;
	
	
	for (auto child : gameObject->GetChildren()) 
	{
		if (eventTriggered) return;
		if (child == nullptr) continue;
		if (!child->IsEnabled()) continue;

		ImageComponent* image = static_cast<ImageComponent*>(child->GetComponent(ComponentType::IMAGE));
		Transform2DComponent* transform2D = static_cast<Transform2DComponent*>(child->GetComponent(ComponentType::TRANSFORM2D));
		ButtonComponent* button = static_cast<ButtonComponent*>(child->GetComponent(ComponentType::BUTTON));

		if (image != nullptr && transform2D != nullptr && button != nullptr && button->IsEnabled()) 
		{

			float2 mouseAbsolute(App->GetInput()->GetGlobalMousePosition());

			
			float normalizedX = -1.0f + 2.0f * static_cast<float>(mouseAbsolute.x - App->GetWindow()->GetGameWindowsPosition().x) / static_cast<float>(App->GetWindow()->GetGameWindowsSize().x);
			float normalizedY = 1.0f - 2.0f * static_cast<float>(mouseAbsolute.y - App->GetWindow()->GetGameWindowsPosition().y) / static_cast<float>(App->GetWindow()->GetGameWindowsSize().y);

			float mouseX = normalizedX;
			float mouseY = normalizedY;

			float2 canvasSize = static_cast<CanvasComponent*>(image->FindCanvasOnParents(image->GetOwner())->GetComponent(ComponentType::CANVAS))->GetSize();
			float2 minImagePoint = transform2D->GetGlobalMatrix().TranslatePart().xy().Mul(float2(2.0f, 2.0f)).Div(canvasSize) - transform2D->GetSize().Div(canvasSize);
			float2 maxImagePoint = transform2D->GetGlobalMatrix().TranslatePart().xy().Mul(float2(2.0f, 2.0f)).Div(canvasSize) + transform2D->GetSize().Div(canvasSize);

			if (mouseX >= minImagePoint.x && mouseY >= minImagePoint.y && mouseX <= maxImagePoint.x && mouseY <= maxImagePoint.y) 
			{
				KeyState mouseButtonLeftState = App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT);
				
				if (mouseButtonLeftState == KeyState::KEY_UP)
				{
					button->TriggerEvent(EventType::CLICK);
					eventTriggered = true;
					return; 
				}
				// Button pressed
				else if (mouseButtonLeftState == KeyState::KEY_DOWN || mouseButtonLeftState == KeyState::KEY_REPEAT)
				{
					button->TriggerEvent(EventType::PRESS);
					eventTriggered = true;
					return; 
				}
				// Mouse hover
				else 
				{
					button->TriggerEvent(EventType::HOVER);
					button->SetHovered(true);
					eventTriggered = true;
					return;
				}
			}

			if (button->GetHovered())
			{
				button->TriggerEvent(EventType::HOVEROFF);
				button->SetHovered(false);
			}
		}

		// Recursively check children
		CheckRaycastRecursive(child, eventTriggered);
	}
}

void ModuleUI::CheckRaycast() 
{
	bool eventTriggered = false;
	CheckRaycastRecursive(App->GetScene()->GetRoot(), eventTriggered);
}

