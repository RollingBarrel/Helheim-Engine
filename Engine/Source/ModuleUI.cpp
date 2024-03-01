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
<<<<<<< Updated upstream
	LOG("Hello");
=======

>>>>>>> Stashed changes
};

ModuleUI::~ModuleUI() 
{
};

bool ModuleUI::Init() {
	mCanvas = new GameObject(App->GetScene()->GetCanvas());
	return true;
};

<<<<<<< Updated upstream
update_status ModuleUI::PreUpdate(float dt) {
	return UPDATE_CONTINUE;
}

=======
>>>>>>> Stashed changes
update_status ModuleUI::Update(float dt) {

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
	DrawWidget(mCanvas);

	// Restore original frustum state
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	App->GetCamera()->SetFrustum(originalFrustum);

	return UPDATE_CONTINUE;
};

update_status ModuleUI::PostUpdate(float dt) {
	return UPDATE_CONTINUE;
};

bool ModuleUI::CleanUp() {
	return true;
}

void ModuleUI::DrawWidget(const GameObject* gameObject)
{
	//if (gameObject->IsEnabled())
	//{
	//	for (const Component* component : gameObject->GetComponents(ComponentType::IMAGE))
	//	{
	//		const ImageComponent* image = (const ImageComponent*) component;
	//		if (image->IsEnabled())
	//		{
	//			image->Draw();
	//		}
	//	}

	//	for (const GameObject* child : gameObject->GetChildren())
	//	{
	//		DrawWidget(child);
	//	}
	//}
	//mCanvas->Draw();
}

void ModuleUI::LoadVBO()
{
	float vertices[] = {
		// texture coordinates
		-0.5f,  0.5f,  0.0f,  1.0f,   // top-left vertex
		-0.5f, -0.5f,  0.0f,  0.0f,   // bottom-left vertex
		 0.5f, -0.5f,  1.0f,  0.0f,   // bottom-right vertex
		 0.5f,  0.5f,  1.0f,  1.0f    // top-right vertex
	};

	glGenBuffers(1, &mQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
}

void ModuleUI::CreateVAO()
{
	glGenVertexArrays(1, &mQuadVAO);
	glBindVertexArray(mQuadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}
