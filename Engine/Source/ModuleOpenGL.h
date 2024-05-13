#ifndef _MODULE_OPENGL_H_
#define _MODULE_OPENGL_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "BatchManager.h"
#include "ParticleSystemComponent.h"
#include <vector>

typedef struct DirectionalLight 
{
	float mDir[4] = { 0.0f, -1.0f, -1.0f, 0.0f }; //w is padding
	float mCol[4] = { 1.f, 1.f, 1.f, 1.2f }; //w is the intensity  1.2f
}DirectionalLight;

class PointLightComponent;
class SpotLightComponent;
struct PointLight;
struct SpotLight;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
typedef unsigned int GLenum;

class ENGINE_API OpenGLBuffer {
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

class ENGINE_API ModuleOpenGL : public Module
{
public:
	ModuleOpenGL();
	~ModuleOpenGL();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void WindowResized(unsigned width, unsigned height);
	void SceneFramebufferResized(unsigned width, unsigned height);
	unsigned int GetFramebufferTexture() const { return sceneTexture; }
	//unsigned int GetGBufferDepth() const { return depthStencil; }
	unsigned int GetGBufferDiffuse() const { return mGDiffuse; }
	unsigned int GetGBufferSpecularRough() const { return mGSpecularRough; }
	unsigned int GetGBufferEmissive() const { return mGEmissive; }
	unsigned int GetGBufferNormals() const { return mGNormals; }
	unsigned int GetGBufferPositions() const { return mGPositions; }
	void BindSceneFramebuffer();
	void UnbindSceneFramebuffer();
	void SetOpenGlCameraUniforms() const;
	void* GetOpenGlContext() { return context; }

	unsigned int GetDebugDrawProgramId() const { return mDebugDrawProgramId; }
	unsigned int GetParticleProgramId() const { return mParticleProgramId; }
	unsigned int GetUIImageProgram() const { return mUIImageProgramId; }
	unsigned int GetSkinningProgramId() const { return mSkinningProgramId; }
	unsigned int GetHighLightProgramId() const { return mHighLightProgramId; }
	unsigned int GetPbrGeoPassProgramId() const { return mPbrGeoPassProgramId; }
	unsigned int GetPbrDeferredPassProgramId() const { return mPbrLightingPassProgramId; }


	//TODO: put all this calls into one without separating for light type??
	void AddPointLight(const PointLightComponent& component);
	void UpdatePointLightInfo(const PointLightComponent& ptrPointLight);
	void RemovePointLight(const PointLightComponent& cPointLight);
	void AddSpotLight(const SpotLightComponent& component);
	void UpdateSpotLightInfo(const SpotLightComponent& ptrSpotLight);
	void RemoveSpotLight(const SpotLightComponent& cSpotLight);

	void BatchAddMesh(MeshRendererComponent* mesh);
	void BatchRemoveMesh(MeshRendererComponent* mesh);
	void BatchEditMaterial(const MeshRendererComponent* mesh);
	void Draw();
	void SetWireframe(bool wireframe);
	void AddHighLight(GameObject* gameObject);
	void RemoveHighLight(GameObject* gameObject);

	void AddParticleSystem(const ParticleSystemComponent* component) { mParticleSystems.push_back(component); }
	void RemoveParticleSystem(const ParticleSystemComponent* component);

	unsigned int CreateShaderProgramFromPaths(const char** shaderNames, int* type, unsigned int numShaderSources) const;

	void BakeIBL(const char* hdrTexPath, unsigned int irradianceSize = 256, unsigned int specEnvBRDFSize = 512, unsigned int specPrefilteredSize = 256);

private:
	void* context = nullptr;

	BatchManager mBatchManager;

	//scene Framebuffer
	unsigned int sFbo;
	unsigned int sceneTexture;
	unsigned int depthStencil;
	//Gbuffer Framebuffer
	unsigned int mGFbo;
	unsigned int mGdepthStencil;
	unsigned int mGDiffuse;
	unsigned int mGSpecularRough;
	unsigned int mGEmissive;
	unsigned int mGNormals;
	unsigned int mGPositions;
	void ResizeGBuffer(unsigned int width, unsigned int height);

	//Camera
	OpenGLBuffer* mCameraUniBuffer = nullptr;

	//Skybox
	void InitSkybox();
	unsigned int mSkyVao = 0;
	unsigned int mSkyVbo = 0;

	//Programs
	char* LoadShaderSource(const char* shaderFileName) const;
	unsigned int CompileShader(unsigned type, const char* source) const;
	unsigned int CreateShaderProgramFromIDs(unsigned int* shaderIds, unsigned int numShaders) const;
	unsigned int mPbrGeoPassProgramId = 0;
	unsigned int mPbrLightingPassProgramId = 0;
	unsigned int mSkyBoxProgramId = 0;
	unsigned int mDebugDrawProgramId = 0;
	unsigned int mUIImageProgramId = 0;
	unsigned int mSkinningProgramId = 0;
	unsigned int mEnvironmentProgramId = 0;
	unsigned int mIrradianceProgramId = 0;
	unsigned int mSpecPrefilteredProgramId = 0;
	unsigned int mSpecEnvBRDFProgramId = 0;
	unsigned int mHighLightProgramId = 0;
	
	
	unsigned int mParticleProgramId = 0;


	//IBL
	unsigned int mHDRTextureId = 0;
	unsigned int mEnvironmentTextureId = 0;
	unsigned int mIrradianceTextureId = 0;
	unsigned int mSpecPrefilteredTexId = 0;
	unsigned int mEnvBRDFTexId = 0;
	

	//Lighting uniforms
	OpenGLBuffer* mDLightUniBuffer = nullptr;
	DirectionalLight mDirLight;
	std::vector<const PointLightComponent*>mPointLights;
	OpenGLBuffer* mPointsBuffer = nullptr;
	std::vector<const SpotLightComponent*>mSpotLights;
	OpenGLBuffer* mSpotsBuffer = nullptr;
	friend class LightningPanel;

	std::vector<const ParticleSystemComponent*> mParticleSystems;

	void BakeEnvironmentBRDF(unsigned int width, unsigned int height);
};

#endif /* _MODULEOPENGL_H_ */