#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleWindow.h"
#include "SDL.h"
#include "glew.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "LightSourceComponent.h"
#include "BatchManager.h"


ModuleOpenGL::ModuleOpenGL()
{
	mBatchManager = new BatchManager();
}

// Destructor
ModuleOpenGL::~ModuleOpenGL()
{
	delete mBatchManager;
}

static void __stdcall OpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	};
 	LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
}

void ModuleOpenGL::BindSceneFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
}

void ModuleOpenGL::UnbindSceneFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Called before render is available
bool ModuleOpenGL::Init()
{
	LOG("Creating Renderer context");

	context = SDL_GL_CreateContext(App->GetWindow()->window);

	GLenum err = glewInit();
	LOG("Using Glew %s", glGetString(GLEW_VERSION));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	//TODO: debug defines just for msvc compiler!!!
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
#endif // _DEBUG

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	//Initialize scene framebuffer
	glGenFramebuffers(1, &sFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
	glGenTextures(1, &depthStencil);
	glBindTexture(GL_TEXTURE_2D, depthStencil);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);
	glGenTextures(1, &colorAttachment);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Error loading the framebuffer !!!");
		return false;
	}
	unsigned int att = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &att);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//InitializePrograms
	mPbrProgramId = CreateShaderProgramFromPaths("PBR_VertexShader.glsl", "PBR_PixelShader.glsl");
	mSkyBoxProgramId = CreateShaderProgramFromPaths("skybox.vs", "skybox.fs");

	//Initialize camera uniforms
	glGenBuffers(1, &cameraUnis);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUnis);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 16 * 2, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUnis);
	//glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, sizeof(float)*16*2);
	SetOpenGlCameraUniforms();

	InitSkybox();

	//Lighting uniforms
	unsigned int program = App->GetOpenGL()->GetPBRProgramId();
	glUseProgram(program);
	glUniform3fv(1, 1, App->GetCamera()->GetPos().ptr());
	glUseProgram(0);
	glGenBuffers(1, &lightUnis);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUnis);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(mDirAmb), &mDirAmb, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightUnis);

	glGenBuffers(1, &mPointSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPointSSBO);
	const uint32_t nPointLights = mPointLights.size();
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * nPointLights, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(nPointLights), &nPointLights);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mPointSSBO);
	return true;
}

update_status ModuleOpenGL::PreUpdate()
{

	glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw the skybox
	glBindFramebuffer(GL_FRAMEBUFFER, sFbo);

	glUseProgram(App->GetOpenGL()->GetSkyboxProgramId());

	glBindVertexArray(mSkyVao);

	glDepthMask(GL_FALSE);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

update_status ModuleOpenGL::PostUpdate()
{

	SDL_GL_SwapWindow(App->GetWindow()->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleOpenGL::CleanUp()
{
	LOG("Destroying renderer");

	glDeleteProgram(mPbrProgramId);
	glDeleteProgram(mSkyBoxProgramId);
	glDeleteVertexArrays(1, &mSkyVao);
	glDeleteBuffers(1, &mSkyVbo);
	glDeleteBuffers(1, &cameraUnis);
	glDeleteBuffers(1, &lightUnis);
	glDeleteBuffers(1, &mPointSSBO);
	glDeleteFramebuffers(1, &sFbo);
	glDeleteTextures(1, &colorAttachment);
	glDeleteTextures(1, &depthStencil);

	//Destroy window
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleOpenGL::WindowResized(unsigned width, unsigned height)
{
	glViewport(0, 0, width, height);
	App->GetCamera()->WindowResized(width, height);
}

void ModuleOpenGL::SceneFramebufferResized(unsigned width, unsigned height)
{
	App->GetCamera()->WindowResized(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
	glViewport(0, 0, width, height);
	App->GetCamera()->WindowResized(width, height);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, depthStencil);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleOpenGL::SetOpenGlCameraUniforms() const
{
	if (context != nullptr)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, App->GetCamera()->GetViewMatrix().Transposed().ptr());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16, App->GetCamera()->GetProjectionMatrix().Transposed().ptr());

		glUseProgram(App->GetOpenGL()->GetPBRProgramId());
		glUniform3fv(1, 1, App->GetCamera()->GetPos().ptr());
		glUseProgram(0);
	}
}

//TODO: This should not be here, we need like a resource or importer
#include "DirectXTex.h"
static unsigned int LoadCubeMap()
{
	unsigned int ret = 0;
	DirectX::ScratchImage image;

	HRESULT res = DirectX::LoadFromDDSFile(L"Assets/Textures/cubemap.dds", DirectX::DDS_FLAGS_NONE, nullptr, image);

	if (res == S_OK)
	{
		const DirectX::TexMetadata& metadata = image.GetMetadata();

		glGenTextures(1, &ret);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ret);

		for (uint32_t i = 0; i < metadata.arraySize; ++i)
		{
			const DirectX::Image* face = image.GetImage(0, i, 0);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, face->width, face->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, face->pixels);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	return ret;
}

void ModuleOpenGL::InitSkybox()
{
	mSkyBoxTexture = LoadCubeMap();

	float skyboxVertices[] = {
	   -1.0f,  1.0f, -1.0f,
	   -1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,

	   -1.0f, -1.0f,  1.0f,
	   -1.0f, -1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

	   -1.0f, -1.0f,  1.0f,
	   -1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,

	   -1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f, -1.0f,

	   -1.0f, -1.0f, -1.0f,
	   -1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
	   -1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &mSkyVao);
	glGenBuffers(1, &mSkyVbo);
	glBindVertexArray(mSkyVao);
	glBindBuffer(GL_ARRAY_BUFFER, mSkyVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyBoxTexture);

	glBindVertexArray(0);
}

char* ModuleOpenGL::LoadShaderSource(const char* shaderFileName) const
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

unsigned int ModuleOpenGL::CompileShader(unsigned type, const char* source) const
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

unsigned int ModuleOpenGL::CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const
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
unsigned int ModuleOpenGL::CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const
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

//Es pot optimitzar el emplace back pasantli els parameters de PointLight ??
LightSourceComponent* ModuleOpenGL::AddPointLight(const PointLight& pLight, GameObject* owner)
{
	LightSourceComponent* newComponent = new LightSourceComponent(owner, pLight);
	mPointLights.push_back(newComponent);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPointSSBO);
	const uint32_t numPointLights = mPointLights.size();
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(numPointLights), &numPointLights);

	const uint32_t dataSize = 16 + sizeof(PointLight) * numPointLights;
	glBindBuffer(GL_COPY_READ_BUFFER, mPointSSBO);
	unsigned int tmp;
	glGenBuffers(1, &tmp);
	glBindBuffer(GL_COPY_WRITE_BUFFER, tmp);
	glBufferData(GL_COPY_WRITE_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, dataSize - sizeof(PointLight));
	glDeleteBuffers(1, &mPointSSBO);
	mPointSSBO = tmp;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPointSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, dataSize - sizeof(PointLight), sizeof(PointLight), &mPointLights.back()->mData); 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mPointSSBO);

	return newComponent;
}

void ModuleOpenGL::UpdatePoinLightInfo(const LightSourceComponent& cPointLight)
{
	for (int i = 0; i < mPointLights.size(); ++i)
	{
		if (mPointLights[i] == &cPointLight)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPointSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * i, sizeof(PointLight), &mPointLights[i]->mData);
			return;
		}
	}
}

void ModuleOpenGL::RemovePointLight(const LightSourceComponent& cPointLight)
{
	for(int i = 0; i < mPointLights.size(); ++i)
	{
		if (mPointLights[i] == &cPointLight)
		{
			mPointLights.erase(mPointLights.begin() + i);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPointSSBO);
			glBindBuffer(GL_COPY_READ_BUFFER, mPointSSBO);
			glBindBuffer(GL_COPY_WRITE_BUFFER, mPointSSBO);
			unsigned int offset = 16 + sizeof(PointLight) * i;
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, offset, offset + sizeof(PointLight), (mPointLights.size()-i +1)*sizeof(PointLight));
			uint32_t numPlights = mPointLights.size();
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &numPlights);
			return;
		}
	}
}

void ModuleOpenGL::AddMeshRendererComponent(MeshRendererComponent* mesh)
{

	mBatchManager->AddMeshRendererComponent(mesh);

}

void ModuleOpenGL::Draw()
{
	mBatchManager->Draw();
}
