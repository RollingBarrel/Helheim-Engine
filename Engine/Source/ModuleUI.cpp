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
#include "VideoComponent.h"
#include "Transform2DComponent.h"

#include "glew.h"
#include "SDL.h"
#include "ButtonComponent.h"
#include "ResourceTexture.h"
#include "ScenePanel.h"
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
	App->GetOpenGL()->BindSceneFramebuffer();
	for (GameObject* gameObject : mCanvasList) 
	{
		DrawWidget(gameObject);
	}
	glEnable(GL_DEPTH_TEST);
	App->GetOpenGL()->UnbindFramebuffer();

	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt) 
{
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
		//TODO: Check this...
		ImageComponent* image = static_cast<ImageComponent*>(gameObject->GetComponent(ComponentType::IMAGE));
		if (image && image->IsEnabled())
		{
			image->Draw();
		}
		
		TextComponent* text = static_cast<TextComponent*>(gameObject->GetComponent(ComponentType::TEXT));
		if (text && text->IsEnabled())
		{
			text->Draw();
		}

		VideoComponent* video = static_cast<VideoComponent*>(gameObject->GetComponent(ComponentType::VIDEO));
		if (video && video->IsEnabled())
		{
			video->Draw();
		}

		for (GameObject* child : gameObject->GetChildren())
		{
			DrawWidget(child);
		}
	}
}

void ModuleUI::RemoveCanvas(GameObject* gameObject)
{
	if (mCanvasList.empty()) return;

	for (std::vector<GameObject*>::iterator it = mCanvasList.begin(); it != mCanvasList.end(); ++it)
	{
		if ((*it)->GetID() == gameObject->GetID())
		{
			mCanvasList.erase(it);
			return;
		}
	}
}

void ModuleUI::AddCanvas(GameObject* gameObject)
{
	mCanvasList.push_back(gameObject);
}


