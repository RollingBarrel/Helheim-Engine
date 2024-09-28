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

void ModuleUI::DrawStencilWidget(GameObject* gameObject)
{
	assert(gameObject);

	ImageComponent* image = static_cast<ImageComponent*>(gameObject->GetComponent(ComponentType::IMAGE));
	if (image && image->IsEnabled())
	{
		image->StencilDraw();
	}

	for (GameObject* child : gameObject->GetChildren())
	{
		DrawStencilWidget(child);
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
	App->GetOpenGL()->BindBlurFramebuffer();
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	for (GameObject* gameObject : mCanvasList)
	{
		assert(gameObject);
		DrawStencilWidget(gameObject);
	}

	//BLUR THE SCENE USING THE STENCIL PREPASS
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	//App->GetOpenGL()->GaussianBlurTexture(App->GetOpenGL()->GetFramebufferTexture(), 16);
	unsigned int res = App->GetOpenGL()->SimpleBlurTexture(App->GetOpenGL()->GetFramebufferTexture(), 8);
	App->GetOpenGL()->BindSceneFramebuffer();
	glUseProgram(App->GetOpenGL()->GetScreenTexProgramId());
	glBindTexture(GL_TEXTURE_2D, res);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisable(GL_STENCIL_TEST);
	//glClear(GL_STENCIL_BUFFER_BIT);

	glPopDebugGroup();
}