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
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Ui");
	UiBlurPass();
	App->GetOpenGL()->BindSceneFramebuffer();
	for (GameObject* gameObject : mCanvasList) 
	{
		DrawWidget(gameObject);
	}
	glEnable(GL_DEPTH_TEST);
	App->GetOpenGL()->UnbindFramebuffer();
	glPopDebugGroup();
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
	assert(gameObject);
		
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

void ModuleUI::DrawBlurWidget(GameObject* gameObject)
{
	assert(gameObject);

	ImageComponent* image = static_cast<ImageComponent*>(gameObject->GetComponent(ComponentType::IMAGE));
	if (image && image->IsEnabled())
	{
		image->BlurDraw();
	}

	for (GameObject* child : gameObject->GetChildren())
	{
		DrawBlurWidget(child);
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

void ModuleUI::UiBlurPass()
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "UiBlur");
	//STENCIL BLUR PREPASS
	unsigned int blurTex = App->GetOpenGL()->BlurTexture(App->GetOpenGL()->GetFramebufferTexture(), false, mBlurIntensity);
	App->GetOpenGL()->BindSceneFramebuffer();
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurTex);
	glActiveTexture(GL_TEXTURE0);
	for (GameObject* gameObject : mCanvasList)
	{
		assert(gameObject);
		DrawBlurWidget(gameObject);
	}
	glPopDebugGroup();
}