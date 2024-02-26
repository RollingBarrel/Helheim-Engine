#include "ModuleUI.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "Application.h"

#include "GameObject.h"
#include "CanvasComponent.h"
#include "ImageComponent.h"

#include "glew.h"
#include "SDL.h"


ModuleUI::ModuleUI() 
{
};

ModuleUI::~ModuleUI() 
{
};

bool ModuleUI::Init() {
	return true;
};

update_status ModuleUI::Update() {

	// Save current frustum state
	Frustum* originalFrustum = new Frustum();
	*originalFrustum = *(App->GetCamera()->GetFrustum());

	// Set Orthografic configuration
	int width, height;
	SDL_GetWindowSize(App->GetWindow()->window, &width, &height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);

	Frustum* UIfrustum = new Frustum();
	UIfrustum->type = FrustumType::OrthographicFrustum;
	App->GetCamera()->SetFrustum(UIfrustum);

	// Draw the UI
	DrawWidget(App->GetScene()->GetCanvas());

	// Restore original frustum state
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	App->GetCamera()->SetFrustum(originalFrustum);

	return UPDATE_CONTINUE;
};

update_status ModuleUI::PostUpdate() {
	return UPDATE_CONTINUE;
};

void ModuleUI::DrawWidget(const GameObject* gameObject)
{
	if (gameObject->IsEnabled())
	{
		for (const Component* component : gameObject->GetComponents(ComponentType::IMAGE))
		{
			const ImageComponent* image = (const ImageComponent*) component;
			if (image->IsEnabled())
			{
				image->Draw();
			}
		}

		for (const GameObject* child : gameObject->GetChildren())
		{
			DrawWidget(child);
		}
	}
}