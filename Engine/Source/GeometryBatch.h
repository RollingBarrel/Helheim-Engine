#pragma once
#include <vector>;
#include "float4.h"

class MeshRendererComponent;
struct ResourceMesh;
typedef struct Attribute;

typedef struct Command
{
	unsigned int mCount = 0;		 // Number of indices in the mesh
	unsigned int mInstanceCount = 0; // Number of instances to render
	unsigned int firstIndex = 0;	 // Index offset in the EBO
	unsigned int baseVertex = 0;	 // Vertex offset in the VBO
	unsigned int baseInstance = 0;   // Instance Index

} Command;

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

class GeometryBatch
{
public:

	GeometryBatch(MeshRendererComponent* mesh);
	~GeometryBatch();

	const std::vector<Attribute*>& GetAttributes() const { return mAttributes; }
	unsigned int GetVertexSize() { return mVertexSize; };
	void AddMesh(MeshRendererComponent* component);
	void AddCommand(const Command& command);
	void Draw();

private:

	std::vector<const MeshRendererComponent*> mMeshComponents;
	std::vector<const ResourceMesh*> mUniqueMeshes;
	std::vector<Attribute*> mAttributes;
	std::vector<Command> mCommands;
	//std::vector<const Material*> mMaterials;
	unsigned int mVertexSize = 0;


	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;
	unsigned int mIbo = 0;
	unsigned int mSsboModels = 0;
	unsigned int mSsboMaterials = 0;

	float* mVboData = nullptr;
	unsigned int mVboNumElements = 0;
	unsigned int* mEboData = nullptr;
	unsigned int mEboNumElements = 0;

};

