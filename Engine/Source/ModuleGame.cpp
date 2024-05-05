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
	const char* shaderSource[2] = { "GameVertex.glsl", "GameFragment.glsl" };
	int shaderType[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	mGameProgramId = App->GetOpenGL()->CreateShaderProgramFromPaths(shaderSource, shaderType, 2);
	glGenVertexArrays(1, &mVAO);

	//TODO: read the name of the fitrst scene to load from somewhere
	App->GetScene()->Load("MainMenu");
	//App->GetScene()->Load("Level1");
	//TODO:: This is bad for decoupling
	App->GetScriptManager()->StartScripts();
	//TODO: Do better!!!!
	App->GetScene()->GetNavController()->HandleBuild();

	//TODO: Add this code to is game playing in editor and game
	GameObject* cameraGameObject = App->GetScene()->FindGameObjectWithTag("MainCamera");
	if (cameraGameObject)
	{
		App->GetCamera()->SetCurrentCamera(cameraGameObject);
	}
	//App->GetEngineScriptManager()->Start();
	//App->GetScene()->GetNavController()->HandleBuild();
	
	App->GetWindow()->SetGameWindowsPosition({0,0});

	//Camera
	//Find camera on the scene and set !!!);
	return true;
}

update_status ModuleGame::PreUpdate(float dt)
{
	int width = App->GetWindow()->GetWidth();
	int height = App->GetWindow()->GetHeight();

	App->GetWindow()->GameWindowsResized({ (float)width, (float)height });

	if (width != mScreenPrevSizeX || height != mScreenPrevSizeY)
	{
		App->GetOpenGL()->SceneFramebufferResized(width, height);
		mScreenPrevSizeX = width;
		mScreenPrevSizeY = height;
	}
	return UPDATE_CONTINUE;
}

update_status ModuleGame::Update(float dt)
{


	glUseProgram(mGameProgramId);
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