#include "ModuleUI.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
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
	mCanvas = new GameObject("Canvas", App->GetScene()->GetRoot());
	mCanvas->CreateComponent(ComponentType::CANVAS);

	LoadVBO();
	CreateVAO();

	mUIProgramId = CreateShaderProgramFromPaths("ui.vs", "ui.fs");

	return true;
};

update_status ModuleUI::PreUpdate(float dt) {
	return UPDATE_CONTINUE;
}

update_status ModuleUI::Update(float dt) {

	// Save current frustum state
	Frustum* originalFrustum = new Frustum();
	*originalFrustum = *(App->GetCamera()->GetFrustum());

	// Set Orthografic configuration
	/*int width, height;
	SDL_GetWindowSize(App->GetWindow()->window, &width, &height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);

	Frustum* UIfrustum = new Frustum();
	UIfrustum->type = FrustumType::OrthographicFrustum;
	App->GetCamera()->SetFrustum(UIfrustum);*/

	// Draw the UI
	App->GetOpenGL()->BindSceneFramebuffer();
	DrawWidget(mCanvas);
	App->GetOpenGL()->UnbindSceneFramebuffer();

	// Restore original frustum state
	/*glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	App->GetCamera()->SetFrustum(originalFrustum);*/

	return UPDATE_CONTINUE;
};

update_status ModuleUI::PostUpdate(float dt) {
	return UPDATE_CONTINUE;
};

bool ModuleUI::CleanUp() {
	glDeleteProgram(mUIProgramId);
	glDeleteVertexArrays(1, &mQuadVAO);
	glDeleteBuffers(1, &mQuadVBO);
	return true;
}

void ModuleUI::DrawWidget(const GameObject* gameObject)
{
	if (gameObject->IsEnabled())
	{
		for (const Component* component : gameObject->GetComponents(ComponentType::IMAGE))
			{
				const ImageComponent* image = (const ImageComponent*) component;
				if (image->IsEnabled())
				{
					image->Draw(false);
				}
			}

		for (const GameObject* child : gameObject->GetChildren())
		{
			DrawWidget(child);
		}
	}
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
<<<<<<< HEAD
=======
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
>>>>>>> 75b78f4 (Add UI shaders)
}