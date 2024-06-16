#pragma once
#include <vector>
#include "ResourceMesh.h"
#include "MathGeoLibFwd.h"

class AnimationComponent;
class MeshRendererComponent;
class ResourceMaterial;
namespace math {
	class Frustum;
};
typedef struct __GLsync* GLsync;

class Command
{
public:
	Command(unsigned int count = 0, unsigned int instanceCount = 0, unsigned int fIndex = 0, unsigned int bVertex = 0, unsigned int bInstance = 0)
		: mCount(count), mInstanceCount(instanceCount), firstIndex(fIndex), baseVertex(bVertex), baseInstance(bInstance) {}
	unsigned int mCount;		 // Number of indices in the mesh
	unsigned int mInstanceCount; // Number of instances to render
	unsigned int firstIndex;	 // Index offset in the EBO
	unsigned int baseVertex;	 // Vertex offset in the VBO
	unsigned int baseInstance;   // Instance Index

};

typedef struct Material {
	float baseColor[4] = {1.f}; //Ultim es de padding
	uint64_t baseColorTex;
	uint64_t metalRoughTex;
	uint64_t normalTex;
	uint32_t hasMetalRoughTex;
	uint32_t hasBaseColorTex;
	uint32_t hasNormalMap;
	float metalness;
	float roughness;
	uint32_t hasEmissiveTex;
	float emissiveFactor[4] = {0.f}; //Ultim es de padding
	uint64_t emissiveTex;
}Material;

typedef struct BufferIndices {
	uint32_t Metarial;
}ShaderIndices;

#define NUM_BUFFERS 3
class GeometryBatch
{

	class BatchMaterialResource
	{
	public:
		BatchMaterialResource(const ResourceMaterial* res)
			: resource(res), referenceCount(1) {}
		const ResourceMaterial* resource;
		unsigned int referenceCount;
	};

	class BatchMeshResource
	{
	public:
		BatchMeshResource(const ResourceMesh* res, unsigned int fIndex = 0, unsigned int bVertex = 0)
			: resource(res), firstIndex(fIndex), baseVertex(bVertex), referenceCount(1), skinOffset(-1) {}
		const ResourceMesh* resource;
		unsigned int firstIndex;
		unsigned int baseVertex;
		unsigned int referenceCount;
		unsigned int skinOffset;
		bool HasSkinning() const { return skinOffset != -1; }
	};

	class BatchMeshRendererComponent
	{
	public:
		BatchMeshRendererComponent() : bMeshIdx(0), bMaterialIdx(0), baseInstance(-1), component(nullptr) {}
		BatchMeshRendererComponent(unsigned int meshIdx, unsigned int materialIdx, const MeshRendererComponent* mComponent) :
			bMeshIdx(meshIdx), bMaterialIdx(materialIdx), baseInstance(-1), component(mComponent) {}
		uint32_t bMeshIdx;
		uint32_t bMaterialIdx;
		unsigned int baseInstance;
		const MeshRendererComponent* component;
	};

public:

	GeometryBatch(const MeshRendererComponent& mesh);
	~GeometryBatch();

	void GetAttributes(std::vector<Attribute>& attributes) const;
	unsigned int GetVertexSize() const { return mVertexSize; };
	void AddMeshComponent(const MeshRendererComponent& component);
	bool EditMaterial(const MeshRendererComponent& component);
	bool RemoveMeshComponent(const MeshRendererComponent& component);
	void Update();
	void Draw(const math::Frustum& frustum, unsigned int programId);
	void EndFrameDraw();

	bool HasMeshesToDraw() const { return mMeshComponents.size() != 0; }
	void ComputeSkinning(const BatchMeshRendererComponent& cMesh);

private:
	unsigned int GetCommandsSsbo() const;
	void ComputeCommands(unsigned int bufferIdx, const math::Frustum& frustum);

	void RecreatePersistentSsbos();
	void RecreateSkinningSsbos();
	void RecreateVboAndEbo();
	void RecreateMaterials();
	void AddUniqueMesh(const MeshRendererComponent& cMesh, unsigned int& meshIdx);

	bool mMaterialFlag = false;
	bool mPersistentsFlag = false;
	bool mVBOFlag = false;
	bool mIboFlag = false;
	bool mSkinningFlag = false;
	
	std::vector<BatchMeshRendererComponent> mMeshComponents;
	std::vector<BatchMeshResource> mUniqueMeshes;
	std::vector<BatchMaterialResource> mUniqueMaterials;
	std::vector<Attribute> mAttributes;

	unsigned int mVertexSize = 0;

	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;
	//unsigned int mIbo = 0;

	uint64_t mDrawCount = 0;
	unsigned int mSsboModelMatrices = 0;
	float* mSsboModelMatricesData[NUM_BUFFERS];
	unsigned int mSsboIndicesCommands = 0;
	uint32_t* mSsboMatIndicesCommandsData = nullptr;
	unsigned int mSsboObbs = 0;
	float* mSsboObbsData = nullptr;
	GLsync mSync[NUM_BUFFERS];
	unsigned int mParameterBuffer = 0;

	unsigned int mSsboMaterials = 0;
	unsigned int mFrustumBuffer = 0;

	float* mVboData = nullptr;
	unsigned int mVboDataSize = 0;
	unsigned int mVboNumElements = 0;
	unsigned int* mEboData = nullptr;
	unsigned int mEboDataSize = 0;
	unsigned int mEboNumElements = 0;

	//Animation
	bool mSkinningApplied = false;
	unsigned int mPaletteSsbo = 0;
	unsigned int mBiggestPaletteSize = 0;
	unsigned int mSkinSsbo = 0;
	unsigned int mSkinBufferSize = 0;

	int mSsboAligment = 0;
};

