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

	LoadVBO();
	CreateVAO();

	mUIProgramId = CreateShaderProgramFromPaths("ui.vs", "ui.fs");

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
	glDeleteProgram(mUIProgramId);
	glDeleteVertexArrays(1, &mQuadVAO);
	glDeleteBuffers(1, &mQuadVBO);

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

void ModuleUI::LoadVBO()
{
	float vertices[] = {
		// texture coordinates
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		-0.5f, -0.5f,  0.0f,  1.0f,   // bottom-left vertex
		0.5f, -0.5f,  1.0f,  1.0f,   // bottom-right vertex
		0.5f,  0.5f,  1.0f,  0.0f,   // top-right vertex
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		0.5f, -0.5f,  1.0f,  1.0f    // bottom-right vertex
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

	glBindVertexArray(0);
}

unsigned int ModuleUI::CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const
{
	unsigned int programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
	int resolution;
	glGetProgramiv(programID, GL_LINK_STATUS, &resolution);
	if (resolution == GL_FALSE)
	{
		int length = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0)
		{
			int written = 0;
			char* info = (char*)malloc(length);
			glGetProgramInfoLog(programID, length, &written, info);
			LOG("Program Log Info: %s", info);
			free(info);
		}
	}
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	return programID;
}

unsigned int ModuleUI::CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const
{
	std::string fullVertexShaderPath = "Assets/Shaders/" + std::string(vertexShaderPath);
	std::string fullFragmentShaderPath = "Assets/Shaders/" + std::string(fragmentShaderPath);
	char* vertexSource = LoadShaderSource(fullVertexShaderPath.c_str());
	char* fragmentSource = LoadShaderSource(fullFragmentShaderPath.c_str());
	unsigned vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	free(vertexSource);
	free(fragmentSource);
	return CreateShaderProgramFromIDs(vertexShaderID, fragmentShaderID);
}

char* ModuleUI::LoadShaderSource(const char* shaderFileName) const
{
	char* data = nullptr;
	FILE* file = nullptr;
	auto info = fopen_s(&file, shaderFileName, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}
	return data;
}

unsigned int ModuleUI::CompileShader(unsigned type, const char* source) const
{

	unsigned int shaderID = glCreateShader(type);

	glShaderSource(shaderID, 1, &source, 0);
	glCompileShader(shaderID);
	int resolution = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &resolution);
	if (resolution == GL_FALSE)
	{
		int length = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0)
		{
			int written = 0;
			char* info = (char*)malloc(length);
			glGetShaderInfoLog(shaderID, length, &written, info);
			LOG("Log Info: %s", info);
			free(info);
		}
	}
	return shaderID;
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

	float normalizedX = -1.0 + 2.0 * (float)(mouseAbsoluteX - scenePanel->GetWindowsPos().x) / (float)scenePanel->GetWindowsSize().x;
	float normalizedY = 1.0 - 2.0 * (float)(mouseAbsoluteY - scenePanel->GetWindowsPos().y) / (float)scenePanel->GetWindowsSize().y;
	if (!mCanvas->GetChildren().empty()) {
		for (GameObject* gameObject : mCanvas->GetChildren())
		{
			ImageComponent* image = (ImageComponent*)gameObject->GetComponent(ComponentType::IMAGE);
			if (image != nullptr)
			{
				// Check if the mouse position is inside the bounds of the image
				if (normalizedX >= gameObject->GetWorldPosition().x && normalizedX <= gameObject->GetWorldPosition().x + image->GetImage()->GetWidth() &&
					normalizedY >= gameObject->GetWorldPosition().y && normalizedY <= gameObject->GetWorldPosition().y + image->GetImage()->GetHeight())
				{
					ButtonComponent* button = (ButtonComponent*)gameObject->GetComponent(ComponentType::BUTTON);
					if (button != nullptr && button->IsEnabled())
					{
						button->OnClicked();
					}
				}
			}
		}
	}
}