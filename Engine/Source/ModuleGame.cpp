#include "ModuleGame.h"
#include "Application.h"
#include "Timer.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "ModuleScriptManager.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "NavMeshController.h"
#include "glew.h"

bool ModuleGame::Init()
{
	glGenVertexArrays(1, &mVAO);

	//TODO: read the name of the fitrst scene to load from somewhere
	App->GetScene()->Load("Assets/Scenes/MainMenu");
	//App->GetScene()->Load("Level1");
	//TODO:: This is bad for decoupling
	//App->GetScriptManager()->AwakeScripts();
	//App->GetScriptManager()->StartScripts();

	//TODO: Add this code to is game playing in editor and game
	//App->GetEngineScriptManager()->Start();
	//App->GetScene()->GetNavController()->HandleBuild();

	App->GetWindow()->SetGameWindowsPosition({ 0,0 });

	//Camera
	//Find camera on the scene and set !!!);
	if(App->GetCamera()->GetCurrentCamera() == nullptr)
		App->GetCamera()->ActivateFirstCamera();
	return true;
}

update_status ModuleGame::PreUpdate(float dt)
{
	int width = App->GetWindow()->GetWidth();
	int height = App->GetWindow()->GetHeight();

	if (width != mScreenPrevSizeX || height != mScreenPrevSizeY)
	{
		App->GetWindow()->GameWindowsResized({ (float)width, (float)height });
		App->GetOpenGL()->SceneFramebufferResized(width, height);
		mScreenPrevSizeX = width;
		mScreenPrevSizeY = height;
	}
	return UPDATE_CONTINUE;
}

update_status ModuleGame::Update(float dt)
{
	glUseProgram(App->GetOpenGL()->GetScreenTexProgramId());
	glBindVertexArray(mVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, App->GetOpenGL()->GetFramebufferTexture());
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glUseProgram(0);
	return UPDATE_CONTINUE;
}

bool ModuleGame::CleanUp()
{
	//glDeleteVertexArrays(1, &mVAO);
	return true;
}