#pragma once
#include <vector>
#include "float4.h"
#include "ResourceMesh.h"

class MeshRendererComponent;
class ResourceMaterial;
typedef struct __GLsync* GLsync;

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
		: resource(res), firstIndex(fIndex), baseVertex(bVertex), referenceCount(1) {}
	const ResourceMesh* resource;
	unsigned int firstIndex;
	unsigned int baseVertex;
	unsigned int referenceCount;
};

class BatchMeshRendererComponent
{
public:
	BatchMeshRendererComponent(const MeshRendererComponent* comp, unsigned int meshIdx, unsigned int materialIdx)
		: component(comp), bMeshIdx(meshIdx), bMaterialIdx(materialIdx) {}
	const MeshRendererComponent* component;
	unsigned int bMeshIdx;
	unsigned int bMaterialIdx;
};


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
	uint32_t  hasShininessMap = false;
	uint32_t hasNormalMap = false;
	uint32_t hasDiffuseMap = false;
	uint32_t hasSpecularMap = false;
	float diffuseColor[3] = {0.0f};
	float shininess = 0.0f;
	float specularColor[4] = {0.0f};
	uint64_t diffuseTexture = 0;
	uint64_t specularTexture = 0;
	uint64_t normalTexture = 0;
}Material;

typedef struct BufferIndices{
	uint32_t Metarial;
}ShaderIndices;

#define NUM_BUFFERS 3
class GeometryBatch
{
public:

	GeometryBatch(const MeshRendererComponent* mesh);
	~GeometryBatch();

	void GetAttributes(std::vector<Attribute>& attributes) const;
	unsigned int GetVertexSize() { return mVertexSize; };
	void AddMeshComponent(const MeshRendererComponent* component);
	void EditMaterial(const MeshRendererComponent* component);
    bool RemoveMeshComponent(const MeshRendererComponent* component);
	void Draw();

private:
	void RecreatePersistentSsbosAndIbo();

	std::vector<BatchMeshRendererComponent> mMeshComponents;
	std::vector<BatchMeshResource> mUniqueMeshes;
	std::vector<BatchMaterialResource> mUniqueMaterials;
	std::vector<Attribute> mAttributes;
	std::vector<Command> mCommands;
	unsigned int mVertexSize = 0;


	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;
	unsigned int mIbo = 0;

	uint64_t mDrawCount = 0;
	unsigned int mSsboModelMatrices = 0;
	float* mSsboModelMatricesData[NUM_BUFFERS];
	unsigned int mSsboIndices = 0;
	uint32_t* mSsboIndicesData[NUM_BUFFERS];
	GLsync mSync[NUM_BUFFERS];

	unsigned int mSsboMaterials = 0;

	float* mVboData = nullptr;
	unsigned int mVboNumElements = 0;
	unsigned int* mEboData = nullptr;
	unsigned int mEboNumElements = 0;

	int mSsboAligment = 0;
};

