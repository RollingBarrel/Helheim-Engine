#pragma once
#include <vector>;
#include "float4.h"

class MeshRendererComponent;
struct ResourceMesh;
struct ResourceMaterial;
typedef struct Attribute;
typedef struct __GLsync* GLsync;

class BatchMeshRendererComponent
{
public:
	BatchMeshRendererComponent(const MeshRendererComponent* comp, uint32_t matIdx = 999999999, unsigned int fIndex = 0, unsigned int bVertex = 0) 
		: component(comp), materialIdx(matIdx), firstIndex(fIndex), baseVertex(bVertex) {}
	const MeshRendererComponent* component;
	uint32_t materialIdx;
	unsigned int firstIndex;
	unsigned int baseVertex;
};

class BatchMeshRendererResource
{
public:
	BatchMeshRendererResource(const ResourceMesh* res, unsigned int fIndex = 0, unsigned int bVertex = 0)
		: resource(res), firstIndex(fIndex), baseVertex(bVertex) {}
	const ResourceMesh* resource;
	unsigned int firstIndex;
	unsigned int baseVertex;
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
private:
	uint64_t padding = 0;
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
	void AddMesh(const MeshRendererComponent* component );
	void EditMaterial(const MeshRendererComponent* component);
	void RemoveMesh(const MeshRendererComponent* component);
	void Draw();

private:

	std::vector<BatchMeshRendererComponent> mMeshComponents;
	std::vector<BatchMeshRendererResource> mUniqueMeshes;
	std::vector<const ResourceMaterial*> mUniqueMaterials;
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

