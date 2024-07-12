#ifndef _MODULE_OPENGL_H_
#define _MODULE_OPENGL_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "BatchManager.h"
#include <vector>

#define NUM_SHADOW_MAPS 16
#define SHADOW_MAPS_SIZE 1024


typedef struct Shadow
{
	float4x4 viewProjMatrix;
	uint64_t shadowMapHandle;
	float bias;
	float padding;

}Shadow;

typedef struct DirectionalLight 
{
	float mDir[4] = { 0.0f, -1.0f, -1.0f, 0.0f }; //w is padding
	float mCol[4] = { 1.f, 1.f, 1.f, 0.05f }; //w is the intensity  1.2f
}DirectionalLight;

class PointLightComponent;
class SpotLightComponent;
class ParticleSystemComponent;
class TrailComponent;
class CameraComponent;
class DecalComponent;
struct PointLight;
struct SpotLight;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
class ResourceIBL;
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

	void* GetOpenGlContext() { return context; }
	void WindowResized(unsigned width, unsigned height);
	void SceneFramebufferResized(unsigned int width, unsigned int height);
	unsigned int GetFramebufferTexture() const { return sceneTexture; }
	void BindSceneFramebuffer();
	//void BindGFramebuffer();
	void UnbindFramebuffer();
	unsigned int GetGBufferDiffuse() const { return mGDiffuse; }
	unsigned int GetGBufferSpecularRough() const { return mGSpecularRough; }
	unsigned int GetGBufferEmissive() const { return mGEmissive; }
	unsigned int GetGBufferNormals() const { return mGNormals; }
	unsigned int GetGBufferDepth() const { return mGDepth; }
	unsigned int GetGBufferPos() const { return mGPosition; }
	void SetOpenGlCameraUniforms() const;

	unsigned int GetDebugDrawProgramId() const { return mDebugDrawProgramId; }
	unsigned int GetParticleProgramId() const { return mParticleProgramId; }
	unsigned int GetTrailProgramId() const { return mTrailProgramId; }
	unsigned int GetUIImageProgram() const { return mUIImageProgramId; }
	unsigned int GetTextProgram() const { return mTextProgramId; }
	unsigned int GetSkinningProgramId() const { return mSkinningProgramId; }
	unsigned int GetSelectSkinsProgramId() const { return mSelectSkinsProgramId; }
	unsigned int GetTileLightCUllingProgramId() const { return mTileLightCullingProgramId; }
	unsigned int GetHighLightProgramId() const { return mHighLightProgramId; }
	unsigned int GetPbrGeoPassProgramId() const { return mPbrGeoPassProgramId; }
	unsigned int GetPbrLightingPassProgramId() const { return mPbrLightingPassProgramId; }
	unsigned int GetUIMaskProgramId() const { return mUIMaskProgramId; }
	unsigned int GetSelectCommandsProgramId() const { return mSelectCommandsProgramId; }
	unsigned int GetEnvironmentProgramId() const { return mEnvironmentProgramId; }
	unsigned int GetIrradianceProgramId() const { return mIrradianceProgramId; }
	unsigned int GetSpecPrefilteredProgramId() const { return mSpecPrefilteredProgramId; }
	unsigned int GetSpecEnvBRDFProgramId() const { return mSpecEnvBRDFProgramId; }

	//TODO: put all this calls into one without separating for light type??
	void AddPointLight(const PointLightComponent& component);
	void UpdatePointLightInfo(const PointLightComponent& ptrPointLight);
	void RemovePointLight(const PointLightComponent& cPointLight);
	void AddSpotLight(const SpotLightComponent& component);
	void UpdateSpotLightInfo(const SpotLightComponent& ptrSpotLight);
	void RemoveSpotLight(const SpotLightComponent& cSpotLight);

	void BatchAddMesh(const MeshRendererComponent& mesh);
	void BatchRemoveMesh(const MeshRendererComponent& mesh);
	void BatchEditMaterial(const MeshRendererComponent& mesh);
	void Draw();
	void SetWireframe(bool wireframe);

	void AddDecal(const DecalComponent& decal);
	void RemoveDecal(const DecalComponent& decal);

	void AddParticleSystem(const ParticleSystemComponent* component) { mParticleSystems.push_back(component); }
	void RemoveParticleSystem(const ParticleSystemComponent* component);

	void AddTrail(const TrailComponent* trail) { mTrails.push_back(trail); }
	void RemoveTrail(const TrailComponent* trail);

	unsigned int CreateShaderProgramFromPaths(const char** shaderNames, int* type, unsigned int numShaderSources) const;

	//void BakeIBL(const char* hdrTexPath, unsigned int irradianceSize = 256, unsigned int specEnvBRDFSize = 512, unsigned int specPrefilteredSize = 256);
	void SetSkybox(unsigned int uid);
	inline unsigned int GetSkyboxID() const;
	unsigned int GetSkyboxVAO() const { return mSkyVao; }
	unsigned int GetSceneWidth() const { return mSceneWidth; }
	unsigned int GetSceneHeight() const { return mSceneHeight; }
private:
	void* context = nullptr;

	BatchManager mBatchManager;

	//scene Framebuffer
	unsigned int sFbo;
	unsigned int sceneTexture;
	//unsigned int depthStencil;
	//Gbuffer Framebuffer
	unsigned int mGFbo;
	unsigned int mGDiffuse;
	unsigned int mGSpecularRough;
	unsigned int mGPosition;
	unsigned int mGNormals;
	unsigned int mGEmissive;
	unsigned int mGColDepth;
	unsigned int mGDepth;

	void ResizeGBuffer(unsigned int width, unsigned int height);
	//void Draw();

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
	unsigned int mPassThroughProgramId = 0;
	unsigned int mSkyBoxProgramId = 0;
	unsigned int mDebugDrawProgramId = 0;
	unsigned int mUIImageProgramId = 0;
	unsigned int mTextProgramId = 0;
	unsigned int mSkinningProgramId = 0;
	unsigned int mSelectSkinsProgramId = 0;
	unsigned int mTileLightCullingProgramId = 0;
	unsigned int mSelectCommandsProgramId = 0;
	unsigned int mEnvironmentProgramId = 0;
	unsigned int mIrradianceProgramId = 0;
	unsigned int mSpecPrefilteredProgramId = 0;
	unsigned int mSpecEnvBRDFProgramId = 0;
	unsigned int mHighLightProgramId = 0;
	unsigned int DecalPassProgramId = 0;
	unsigned int mUIMaskProgramId = 0;

	unsigned int mParticleProgramId = 0;
	unsigned int mTrailProgramId = 0;


	//IBL
	ResourceIBL* mCurrSkyBox = nullptr;

	unsigned int mEmptyVAO = 0;
	
	//Shadows
	unsigned int mShadowsFrameBufferId = 0;
	unsigned int mShadowMaps[NUM_SHADOW_MAPS] = { 0 };
	unsigned int mShadowMapsHandle[NUM_SHADOW_MAPS] = { 0 };
	OpenGLBuffer* mShadowsBuffer = nullptr;

	//Decals
	unsigned int mDecalsVao = 0;
	unsigned int mDecalsVbo = 0;
	void InitDecals();
	std::vector<const DecalComponent*> mDecalComponents;


	//Lighting uniforms
	unsigned int mPLightListImgTex;
	unsigned int mPLightListImgBuffer;
	void LightCullingLists(unsigned int screenWidth, unsigned int screeHeight);
	OpenGLBuffer* mDLightUniBuffer = nullptr;
	DirectionalLight mDirLight;
	std::vector<const PointLightComponent*>mPointLights;
	OpenGLBuffer* mPointsBuffer = nullptr;
	std::vector<const SpotLightComponent*>mSpotLights;
	OpenGLBuffer* mSpotsBuffer = nullptr;
	friend class LightningPanel;

	std::vector<const ParticleSystemComponent*> mParticleSystems;
	std::vector<const TrailComponent*> mTrails;

	//void BakeEnvironmentBRDF(unsigned int width, unsigned int height);
	//std::vector<const GameObject*> mHighlightedObjects;

	unsigned int mSceneWidth = 1;
	unsigned int mSceneHeight = 1;
};

#endif /* _MODULEOPENGL_H_ */