#ifndef _MODULE_OPENGL_H_
#define _MODULE_OPENGL_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include <vector>

typedef struct DirectionalAmbient {
	float mDirDir[4] = { 0.0f, -1.0f, -1.0f, 0.0f }; //w is padding
	float mDirCol[4] = { 1.f, 1.f, 1.f, 1.2f }; //w is the intensity
	float mAmbientCol[4] = { 0.3f, 0.4f, 0.6f, 0.0f }; //w is padding
}DirectionalAmbient;

class PointLightComponent;
class SpotLightComponent;
class PointLight;
class BatchManager;
class SpotLight;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
typedef unsigned int GLenum;

class OpenGLBuffer {
public:
	OpenGLBuffer(GLenum type, GLenum usage, unsigned int binding = -1, unsigned int size = 0, const void* data = nullptr);
	~OpenGLBuffer();
	void PushBackData(const void* data, unsigned int dataSize);
	void PopBackData(unsigned int dataSize);
	void ShrinkToFit();
	void UpdateData(const void* data, unsigned int dataSize, unsigned int offset);
	void RemoveData(unsigned int dataSize, unsigned int offset);

private:
	void ChangeBufferCapacity(unsigned int newCapacity);
	const GLenum mType;
	const GLenum mUsage;
	const unsigned int mBinding;
	unsigned int mIdx;
	unsigned int mDataSize;
	unsigned int mDataCapacity;
};

class ModuleOpenGL : public Module
{
public:
	ModuleOpenGL();
	~ModuleOpenGL();

	bool Init() override;
	update_status PreUpdate() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void WindowResized(unsigned width, unsigned height);
	void SceneFramebufferResized(unsigned width, unsigned height);
	unsigned int GetFramebufferTexture() const { return colorAttachment; }
	void BindSceneFramebuffer();
	void UnbindSceneFramebuffer();
	void SetOpenGlCameraUniforms() const;
	void* GetOpenGlContext() { return context; }

	unsigned int GetPBRProgramId() const { return mPbrProgramId; }
	unsigned int GetSkyboxProgramId() const { return mSkyBoxProgramId; }

	//TODO: put all this calls into one without separating for light type??
	PointLightComponent* AddPointLight(const PointLight& pLight, GameObject* owner);
	void UpdatePoinLightInfo(const PointLightComponent& ptrPointLight);
	void RemovePointLight(const PointLightComponent& cPointLight);
	SpotLightComponent* AddSpotLight(const SpotLight& pLight, GameObject* owner);
	void UpdateSpotLightInfo(const SpotLightComponent& ptrSpotLight);
	void RemoveSpotLight(const SpotLightComponent& cSpotLight);

	void AddMeshRendererComponent(MeshRendererComponent* mesh);
	void Draw();


private:
	void* context = nullptr;
	BatchManager* mBatchManager;

	//Framebuffer
	unsigned int sFbo;
	unsigned int colorAttachment;
	unsigned int depthStencil;

	//Camera
	OpenGLBuffer* mCameraUniBuffer = nullptr;

	//Skybox
	void InitSkybox();
	unsigned int mSkyBoxTexture = 0;
	unsigned int mSkyVao = 0;
	unsigned int mSkyVbo = 0;

	//Programs
	char* LoadShaderSource(const char* shaderFileName) const;
	unsigned int CompileShader(unsigned type, const char* source) const;
	unsigned int CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const;
	unsigned int CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const;
	unsigned int mPbrProgramId = 0;
	unsigned int mSkyBoxProgramId = 0;


	//Lighting uniforms
	OpenGLBuffer* mDLightUniBuffer = nullptr;
	DirectionalAmbient mDirAmb;
	std::vector<const PointLightComponent*>mPointLights;
	OpenGLBuffer* mPointsBuffer = nullptr;
	std::vector<const SpotLightComponent*>mSpotLights;
	OpenGLBuffer* mSpotsBuffer = nullptr;
	friend class LightningPanel;
};

#endif /* _MODULEOPENGL_H_ */