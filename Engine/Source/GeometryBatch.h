#pragma once
#include <vector>;
#include "float3.h"
#include "float4x4.h"

class MeshRendererComponent;
struct ResourceMesh;
typedef struct Attribute;

typedef struct Command;

typedef struct Material {
	
	//Diffuse
	float3 diffuseColor = float3::zero;
	unsigned int diffuseTexture = 0;

	//Specular
	float3 specularColor = float3::zero;
	unsigned int specularTexture = 0;

	//Normal
	unsigned int normalTexture = 0;

	//Shininess
	float shininess = 0.0f;

	//Options
	bool hasDiffuseMap = false;
	bool hasSpecularMap = false;
	bool hasShininessMap = false;
	bool hasNormalMap = false;


}Material;

class GeometryBatch
{
public:

	GeometryBatch(MeshRendererComponent* mesh);
	~GeometryBatch();

	const std::vector<Attribute*>& GetAttributes() const { return mAttributes; }
	
	void AddMesh(MeshRendererComponent* component);
	void AddCommand(Command* command);
	void Draw();

private:

	

	std::vector<MeshRendererComponent*> mMeshComponents;
	std::vector<ResourceMesh*> mUniqueMeshes;
	std::vector<Attribute*> mAttributes;
	std::vector<Command*> mCommands;
	std::vector<Material*> mMaterials;
	unsigned int mVertexSize = 0;

	unsigned int mVboSize = 0;
	unsigned int mEboSize = 0;
	unsigned int mVaoSize = 0;

	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;
	unsigned int mIbo = 0;
	unsigned int mSsboModels = 0;
	unsigned int mSsboMaterials = 0;
	

};

