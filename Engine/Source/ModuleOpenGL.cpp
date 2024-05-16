#define _CRT_SECURE_NO_WARNINGS

#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "SDL.h"
#include "glew.h"
#include "ModuleCamera.h"
#include "ModuleUI.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "BatchManager.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ModuleFileSystem.h"

#include "CameraComponent.h"


ModuleOpenGL::ModuleOpenGL()
{
}

// Destructor
ModuleOpenGL::~ModuleOpenGL()
{
	delete mCameraUniBuffer;
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
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
	glDrawBuffers(1, &att);;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//InitializePrograms
	const char* sourcesPaths[2] = { "PBRCT_VertexShader.glsl", "PBRCT_PixelShader.glsl" };
	int sourcesTypes[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	mPbrProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "skybox.vs";
	sourcesPaths[1] = "skybox.fs";
	mSkyBoxProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "CubeMap_VertexShader.glsl";
	sourcesPaths[1] = "Environment_FragmentShader.glsl";
	mEnvironmentProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "CubeMap_VertexShader.glsl";
	sourcesPaths[1] = "Irradiance_FragmentShader.glsl";
	mIrradianceProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "CubeMap_VertexShader.glsl";
	sourcesPaths[1] = "SpecPreFilteredEnvFragment.glsl";
	mSpecPrefilteredProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "GameVertex.glsl";
	sourcesPaths[1] = "SpecEnvBRDFFragment.glsl";
	mSpecEnvBRDFProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "basicDebugShader.vs";
	sourcesPaths[1] = "basicDebugShader.fs";
	mDebugDrawProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "ui.vs";
	sourcesPaths[1] = "ui.fs";
	mUIImageProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "HighLight_Vertex.glsl";
	sourcesPaths[1] = "HighLight_Fragment.glsl";
	mHighLightProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "particle_vertex.glsl";
	sourcesPaths[1] = "particle_fragment.glsl";
	mParticleProgramId = CreateShaderProgramFromPaths(sourcesPaths, sourcesTypes, 2);

	sourcesPaths[0] = "skinning.comp";
	int computeType = GL_COMPUTE_SHADER;
	mSkinningProgramId = CreateShaderProgramFromPaths(sourcesPaths, &computeType, 1);

	//Initialize camera uniforms
	mCameraUniBuffer = new OpenGLBuffer(GL_UNIFORM_BUFFER, GL_STATIC_DRAW, 0, sizeof(float) * 16 * 2);
	SetOpenGlCameraUniforms();

	InitSkybox();

	//Lighting uniforms
	mDLightUniBuffer = new OpenGLBuffer(GL_UNIFORM_BUFFER, GL_STATIC_DRAW, 1, sizeof(mDirLight), &mDirLight);

	const uint32_t numPointLights[4] = { mPointLights.size(), 0, 0, 0 };
	mPointsBuffer = new OpenGLBuffer(GL_SHADER_STORAGE_BUFFER, GL_STATIC_DRAW, 0, 16, &numPointLights);

	const uint32_t numSpotLights[4] = { mSpotLights.size(), 0, 0, 0 };
	mSpotsBuffer = new OpenGLBuffer(GL_SHADER_STORAGE_BUFFER, GL_STATIC_DRAW, 1, 16, &numSpotLights);

	//BakeIBL("Assets/Textures/skybox.hdr");
	BakeIBL("Assets/Textures/rural_asphalt_road_4k.hdr");
	return true;
}

update_status ModuleOpenGL::PreUpdate(float dt)
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	//Draw the skybox
	if (mEnvironmentTextureId != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentTextureId);
		glUseProgram(mSkyBoxProgramId);
		glBindVertexArray(mSkyVao);
		glDepthMask(GL_FALSE);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//BakeIBL(L"Assets/Textures/skybox.hdr");

	return UPDATE_CONTINUE;
}

update_status ModuleOpenGL::PostUpdate(float dt)
{

	SDL_GL_SwapWindow(App->GetWindow()->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleOpenGL::CleanUp()
{
	LOG("Destroying renderer");

	delete mPointsBuffer;
	delete mSpotsBuffer;
	delete mDLightUniBuffer;

	glDeleteProgram(mPbrProgramId);
	glDeleteProgram(mSkyBoxProgramId);
	glDeleteProgram(mUIImageProgramId);
	glDeleteVertexArrays(1, &mSkyVao);
	glDeleteBuffers(1, &mSkyVbo);
	glDeleteFramebuffers(1, &sFbo);
	glDeleteTextures(1, &colorAttachment);
	glDeleteTextures(1, &depthStencil);

	//Destroy window
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleOpenGL::SetWireframe(bool wireframe)
{
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



void ModuleOpenGL::AddHighLight(GameObject* gameObject)
{
	if (!gameObject->IsRoot())
	{
		std::vector<Component*> meshComponents = gameObject->GetComponentsInChildren(ComponentType::MESHRENDERER);
		if (!meshComponents.empty())
		{
			mBatchManager.AddHighLight(meshComponents);
		}
	}
}

void ModuleOpenGL::RemoveHighLight(GameObject* gameObject)
{
	if (!gameObject->IsRoot())
	{
		std::vector<Component*> meshComponents = gameObject->GetComponentsInChildren(ComponentType::MESHRENDERER);
		if (!meshComponents.empty())
		{
			mBatchManager.RemoveHighLight(meshComponents);
		}
	}
}

void ModuleOpenGL::WindowResized(unsigned width, unsigned height)
{
	glViewport(0, 0, width, height);
	//SetOpenGlCameraUniforms();
}

void ModuleOpenGL::SceneFramebufferResized(unsigned width = 0, unsigned height = 0)
{
	static unsigned sWidth = 1;
	static unsigned sHeight = 1;
	if (width == 0 && height == 0)
	{
		width = sWidth;
		height = sHeight;
	}
	sWidth = width;
	sHeight = height;

	glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
	glViewport(0, 0, width, height);
	App->GetCamera()->SetAspectRatio((float)width / (float)height);
	SetOpenGlCameraUniforms();
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, depthStencil);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleOpenGL::SetOpenGlCameraUniforms() const
{
	if (mCameraUniBuffer != nullptr)
	{
		const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();

		if (camera)
		{
			mCameraUniBuffer->UpdateData(camera->GetViewMatrix().Transposed().ptr(), sizeof(float) * 16, 0);
			mCameraUniBuffer->UpdateData(camera->GetProjectionMatrix().Transposed().ptr(), sizeof(float) * 16, sizeof(float) * 16);

			glUseProgram(mPbrProgramId);
			glUniform3fv(1, 1, camera->GetFrustum().pos.ptr());
			glUseProgram(0);
		}
		else
		{
			mCameraUniBuffer->UpdateData(float4x4::identity.Transposed().ptr(), sizeof(float) * 16, 0);
			mCameraUniBuffer->UpdateData(float4x4::identity.Transposed().ptr(), sizeof(float) * 16, sizeof(float) * 16);

			glUseProgram(mPbrProgramId);
			glUniform3fv(1, 1, float3::zero.ptr());
			glUseProgram(0);
		}
		
	}
}

//TODO: This should not be here, we need like a resource or importer
#include "DirectXTex.h"
#include <MathConstants.h>
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
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	return ret;
}

void ModuleOpenGL::InitSkybox()
{

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

unsigned int ModuleOpenGL::CreateShaderProgramFromIDs(unsigned int* shaderIds, unsigned int numShaders) const
{
	unsigned int programID = glCreateProgram();
	for (unsigned int i = 0; i < numShaders; ++i)
	{
		glAttachShader(programID, shaderIds[i]);
	}
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
	for (unsigned int i = 0; i < numShaders; ++i)
	{
		glDeleteShader(shaderIds[i]);
	}
	return programID;
}

unsigned int ModuleOpenGL::CreateShaderProgramFromPaths(const char** shaderNames, int* type, unsigned int numShaderSources) const
{
	unsigned int* shaderIds = (unsigned int*)malloc(sizeof(unsigned int) * numShaderSources);
	for (unsigned int i = 0; i < numShaderSources; ++i)
	{
		char* fullShaderPath = (char*)malloc(strlen(shaderNames[i]) + 16);
		fullShaderPath[0] = '\0';
		strcat(fullShaderPath, "Assets/Shaders/");
		strcat(fullShaderPath, shaderNames[i]);
		char* shaderSource = LoadShaderSource(fullShaderPath);
		free(fullShaderPath);
		//assert(App->GetFileSystem()->Exists(fullShaderPath));
		shaderIds[i] = CompileShader(type[i], shaderSource);
		free(shaderSource);
	}
	unsigned int ret = CreateShaderProgramFromIDs(shaderIds, numShaderSources);
	free(shaderIds);
	return ret;
}

void ModuleOpenGL::BakeEnvironmentBRDF(unsigned int width, unsigned int height)
{
	if (mEnvBRDFTexId != 0)
		glDeleteTextures(1, &mEnvBRDFTexId);
	glGenTextures(1, &mEnvBRDFTexId);
	glBindTexture(GL_TEXTURE_2D, mEnvBRDFTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glUseProgram(mSpecEnvBRDFProgramId);
	glViewport(0, 0, width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mEnvBRDFTexId, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void ModuleOpenGL::BakeIBL(const char* hdrTexPath, unsigned int irradianceSize, unsigned int specEnvBRDFSize, unsigned int specPrefilteredSize)
{
	DirectX::ScratchImage image;

	size_t size = strlen(hdrTexPath) + 1;
	wchar_t* pathTex = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, pathTex, size, hdrTexPath, size - 1);

	HRESULT res = DirectX::LoadFromHDRFile(pathTex, nullptr, image);
	delete[] pathTex;

	if (res == S_OK)
	{
		if (mIrradianceTextureId != 0)
		{
			//TODO: Not all must be deleted if separated from here
			glDeleteTextures(1, &mIrradianceTextureId);
			glDeleteTextures(1, &mEnvironmentTextureId);
			glDeleteTextures(1, &mSpecPrefilteredTexId);
			mIrradianceTextureId = 0;
		}

		const float3 front[6] = { float3::unitX, -float3::unitX, float3::unitY,
							 -float3::unitY, float3::unitZ, -float3::unitZ };

		const float3 up[6] = { -float3::unitY, -float3::unitY, float3::unitZ,
						 -float3::unitZ, -float3::unitY, -float3::unitY };

		Frustum frustum;
		frustum.type = FrustumType::PerspectiveFrustum;
		frustum.pos = float3::zero;
		frustum.nearPlaneDistance = 0.1f;
		frustum.farPlaneDistance = 100.0f;
		frustum.verticalFov = pi / 2.0f;
		frustum.horizontalFov = pi / 2.0f;

		const unsigned int irradianceWidth = irradianceSize;
		const unsigned int irradianceHeight = irradianceSize;
		glGenTextures(1, &mEnvironmentTextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentTextureId);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceWidth, irradianceHeight, 0,
				GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



		const DirectX::TexMetadata& metadata = image.GetMetadata();

		glGenTextures(1, &mHDRTextureId);
		glBindTexture(GL_TEXTURE_2D, mHDRTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, metadata.width, metadata.height, 0, GL_RGBA, GL_FLOAT, image.GetPixels());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glUseProgram(mEnvironmentProgramId);
		//glUniform1i(glGetUniformLocation(mEnvironmentProgramId, "HDRImage"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mHDRTextureId);

		//glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
		unsigned int frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		//int viewPortSize[4];
		//glGetIntegerv(GL_VIEWPORT, viewPortSize);
		glViewport(0, 0, irradianceWidth, irradianceHeight);
		glDepthMask(GL_FALSE);
		glBindVertexArray(mSkyVao);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mEnvironmentTextureId, 0);

			frustum.front = front[i];
			frustum.up = up[i];

			glUniformMatrix4fv(0, 1, GL_TRUE, frustum.ViewMatrix().ptr());
			glUniformMatrix4fv(1, 1, GL_TRUE, frustum.ProjectionMatrix().ptr());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentTextureId);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glUseProgram(mIrradianceProgramId);
		glUniform1ui(5, irradianceSize);
		glGenTextures(1, &mIrradianceTextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceTextureId);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceWidth, irradianceHeight, 0,
				GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentTextureId);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mIrradianceTextureId, 0);

			frustum.front = front[i];
			frustum.up = up[i];

			glUniformMatrix4fv(0, 1, GL_TRUE, frustum.ViewMatrix().ptr());
			glUniformMatrix4fv(1, 1, GL_TRUE, frustum.ProjectionMatrix().ptr());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Specular IBL
		const unsigned int specWidth = specPrefilteredSize;
		const unsigned int specHeight = specPrefilteredSize;
		glGenTextures(1, &mSpecPrefilteredTexId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mSpecPrefilteredTexId);

		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, specWidth, specHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		int numMipMaps = int(log(float(specWidth)) / log(2.0f));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMipMaps);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glUseProgram(mSpecPrefilteredProgramId);
		glUniform1ui(5, specWidth);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentTextureId);
		for (int currMipMap = 0; currMipMap <= numMipMaps; ++currMipMap)
		{
			float roughness = (static_cast<float>(currMipMap) / static_cast<float>(numMipMaps - 1));
			glUniform1f(3, roughness);
			float coolMath = specWidth * pow(0.5f, currMipMap);
			glViewport(0, 0, coolMath, coolMath);
			// Render each cube plane
			for (unsigned int i = 0; i < 6; ++i)
			{
				// TODO: Draw UnitCube using prefiltered environment map shader and roughness
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mSpecPrefilteredTexId, currMipMap);

				frustum.front = front[i];
				frustum.up = up[i];

				glUniformMatrix4fv(0, 1, GL_TRUE, frustum.ViewMatrix().ptr());
				glUniformMatrix4fv(1, 1, GL_TRUE, frustum.ProjectionMatrix().ptr());
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		
		static unsigned int sizeX = 0;
		static unsigned int sizeY = 0;
		if (mEnvBRDFTexId == 0 || sizeX != specEnvBRDFSize || sizeY != specEnvBRDFSize)
		{
			sizeX = specEnvBRDFSize;
			sizeY = specEnvBRDFSize;
			BakeEnvironmentBRDF(specEnvBRDFSize, specEnvBRDFSize);
		}

		glUseProgram(0);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
		//glViewport(viewPortSize[0], viewPortSize[1], viewPortSize[2], viewPortSize[3]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &frameBuffer);
		glDeleteTextures(1, &mHDRTextureId);

		glUseProgram(mPbrProgramId);
		glUniform1ui(glGetUniformLocation(mPbrProgramId, "numLevels"), numMipMaps);
		glUniform1i(glGetUniformLocation(mPbrProgramId, "prefilteredIBL"), 5);
		glUniform1i(glGetUniformLocation(mPbrProgramId, "diffuseIBL"), 6);
		glUniform1i(glGetUniformLocation(mPbrProgramId, "environmentBRDF"), 7);
		glUseProgram(0);
		SceneFramebufferResized();
	}

}


//Es pot optimitzar el emplace back pasantli els parameters de PointLight ??
void ModuleOpenGL::AddPointLight(const PointLightComponent& component)
{
	mPointLights.push_back(&component);
	mPointsBuffer->PushBackData(&component.GetData(), sizeof(PointLight));
	uint32_t size = mPointLights.size();
	mPointsBuffer->UpdateData(&size, sizeof(size), 0);
}

void ModuleOpenGL::UpdatePointLightInfo(const PointLightComponent& cPointLight)
{
	for (int i = 0; i < mPointLights.size(); ++i)
	{
		if (mPointLights[i] == &cPointLight)
		{
			mPointsBuffer->UpdateData(&mPointLights[i]->GetData(), sizeof(mPointLights[i]->GetData()), 16 + sizeof(mPointLights[i]->GetData()) * i);
			return;
		}
	}
}

void ModuleOpenGL::RemovePointLight(const PointLightComponent& cPointLight)
{
	for (int i = 0; i < mPointLights.size(); ++i)
	{
		if (mPointLights[i]->GetID() == cPointLight.GetID())
		{
			mPointLights.erase(mPointLights.begin() + i);
			mPointsBuffer->RemoveData(sizeof(mPointLights[i]->GetData()), 16 + sizeof(mPointLights[i]->GetData()) * i);
			uint32_t size = mPointLights.size();
			mPointsBuffer->UpdateData(&size, sizeof(size), 0);
			return;
		}
	}
}

void ModuleOpenGL::BatchAddMesh(MeshRendererComponent* mesh)
{
	mBatchManager.AddMeshComponent(mesh);
}

void ModuleOpenGL::BatchRemoveMesh(MeshRendererComponent* mesh)
{
	mBatchManager.RemoveMeshComponent(mesh);
}

void ModuleOpenGL::BatchEditMaterial(const MeshRendererComponent* mesh)
{
	mBatchManager.EditMaterial(mesh);
}

void ModuleOpenGL::Draw()
{
	BindSceneFramebuffer();
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mSpecPrefilteredTexId);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceTextureId);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, mEnvBRDFTexId);
	mBatchManager.Draw();
	glActiveTexture(GL_TEXTURE0);
	for (auto partSys : mParticleSystems)
	{
		partSys->Draw();
	}
	UnbindSceneFramebuffer();
}
//Es pot optimitzar el emplace back pasantli els parameters de SpotLight ??
void ModuleOpenGL::AddSpotLight(const SpotLightComponent& component)
{
	mSpotLights.push_back(&component);
	mSpotsBuffer->PushBackData(&component.GetData(), sizeof(SpotLight));
	uint32_t size = mSpotLights.size();
	mSpotsBuffer->UpdateData(&size, sizeof(size), 0);
}

void ModuleOpenGL::UpdateSpotLightInfo(const SpotLightComponent& cSpotLight)
{
	for (int i = 0; i < mSpotLights.size(); ++i)
	{
		if (mSpotLights[i]->GetID() == cSpotLight.GetID())
		{
			mSpotsBuffer->UpdateData(&mSpotLights[i]->GetData(), sizeof(mSpotLights[i]->GetData()), 16 + sizeof(mSpotLights[i]->GetData()) * i);
			return;
		}
	}
}

void ModuleOpenGL::RemoveSpotLight(const SpotLightComponent& cSpotLight)
{
	for (int i = 0; i < mSpotLights.size(); ++i)
	{
		if (mSpotLights[i]->GetID() == cSpotLight.GetID())
		{
			mSpotLights.erase(mSpotLights.begin() + i);
			mSpotsBuffer->RemoveData(sizeof(mSpotLights[i]->GetData()), 16 + sizeof(mSpotLights[i]->GetData()) * i);
			uint32_t size = mSpotLights.size();
			mSpotsBuffer->UpdateData(&size, sizeof(size), 0);
			return;
		}
	}
}

OpenGLBuffer::OpenGLBuffer(GLenum type, GLenum usage, unsigned int binding, unsigned int size, const void* data) : mType(type), mUsage(usage), mBinding(binding), mIdx(0), mDataSize(size), mDataCapacity(size) {
	glGenBuffers(1, &mIdx);
	glBindBuffer(mType, mIdx);
	if (mDataSize != 0)
		glBufferData(mType, mDataSize, data, mUsage);
	if (mBinding != -1)
		glBindBufferBase(mType, mBinding, mIdx);
	//glBindBufferRange(mType, mBinding, mIdx, offset, size);
	glBindBuffer(mType, 0);
}

OpenGLBuffer::~OpenGLBuffer()
{
	glDeleteBuffers(1, &mIdx);
}

void OpenGLBuffer::PushBackData(const void* data, unsigned int dataSize)
{
	if (mDataCapacity == 0)
	{
		glBindBuffer(mType, mIdx);
		glBufferData(mType, mDataSize, data, mUsage);
		mDataSize = dataSize;
		mDataCapacity = dataSize;
		glBindBuffer(mType, 0);
		return;
	}
	const unsigned int newSize = (mDataSize + dataSize);
	if (newSize > mDataCapacity)
	{
		ChangeBufferCapacity(newSize);
	}

	glBindBuffer(mType, mIdx);
	glBufferSubData(mType, mDataSize, dataSize, data);
	mDataSize = newSize;
	glBindBuffer(mType, 0);
}

void OpenGLBuffer::PopBackData(unsigned int dataSize)
{
	if (mDataCapacity < dataSize)
	{
		mDataSize = 0;
		return;
	}

	mDataSize -= dataSize;
}

void OpenGLBuffer::ShrinkToFit()
{
	if (mDataCapacity != mDataSize)
	{
		const unsigned int diff = mDataCapacity - mDataSize;
		ChangeBufferCapacity(mDataCapacity - diff);
	}
}

void OpenGLBuffer::ChangeBufferCapacity(unsigned int newCapacity)
{
	if (newCapacity < mDataSize)
	{
		mDataSize = newCapacity;
	}
	if (newCapacity == 0)
	{
		glBindBuffer(mType, mIdx);
		glBufferData(mType, newCapacity, nullptr, mUsage);
		mDataCapacity = newCapacity;
		glBindBuffer(mType, 0);
		return;
	}
	unsigned int tmp;
	glGenBuffers(1, &tmp);
	glBindBuffer(mType, tmp);
	glBufferData(mType, newCapacity, nullptr, mUsage);
	glCopyNamedBufferSubData(mIdx, tmp, 0, 0, mDataSize);
	glDeleteBuffers(1, &mIdx);
	mIdx = tmp;
	if (mBinding != -1)
		glBindBufferBase(mType, mBinding, mIdx);
	mDataCapacity = newCapacity;
	glBindBuffer(mType, 0);
}

void OpenGLBuffer::UpdateData(const void* data, unsigned int dataSize, unsigned int offset)
{
	const unsigned int requiredSize = (dataSize + offset);
	if (requiredSize > mDataCapacity)
	{
		ChangeBufferCapacity(requiredSize);
	}
	glBindBuffer(mType, mIdx);
	glBufferSubData(mType, offset, dataSize, data);
	glBindBuffer(mType, 0);
}

void OpenGLBuffer::RemoveData(unsigned int dataSize, unsigned int offset)
{
	if (dataSize >= mDataCapacity)
	{
		mDataSize = 0;
		ShrinkToFit();
		return;
	}
	if ((offset + dataSize) == mDataSize)
	{
		PopBackData(dataSize);
		return;
	}
	//Careful same buffer overlaped data
	//glCopyNamedBufferSubData(mIdx, mIdx, offset + dataSize, offset, mDataSize - offset - dataSize);
	unsigned int tmp;
	glGenBuffers(1, &tmp);
	glBindBuffer(GL_COPY_WRITE_BUFFER, tmp);
	glBufferData(GL_COPY_WRITE_BUFFER, mDataSize - dataSize, nullptr, GL_STATIC_COPY);
	glBindBuffer(GL_COPY_READ_BUFFER, mIdx);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, offset);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, offset + dataSize, offset, mDataSize - offset - dataSize);
	mDataSize -= dataSize;
	glBindBuffer(GL_COPY_READ_BUFFER, tmp);
	glBindBuffer(GL_COPY_WRITE_BUFFER, mIdx);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, mDataSize);
	glDeleteBuffers(1, &tmp);
}

void ModuleOpenGL::RemoveParticleSystem(const ParticleSystemComponent* component)
{
	for (int i = 0; i < mParticleSystems.size(); ++i)
	{
		if (mParticleSystems[i] == component)
		{
			mParticleSystems.erase(mParticleSystems.begin() + i);
		}
	}
}
