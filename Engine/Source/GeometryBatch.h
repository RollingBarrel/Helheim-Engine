#pragma once
#include <vector>;

class MeshRendererComponent;
struct ResourceMesh;
typedef struct Attribute;

struct Command
{

	unsigned int mCount = 0;		 // Number of indices in the mesh
	unsigned int mInstanceCount = 0; // Number of instances to render
	unsigned int firstIndex = 0;	 // Index offset in the EBO
	unsigned int baseVertex = 0;	 // Vertex offset in the VBO
	unsigned int baseInstance = 0;   // Instance Index

};


class GeometryBatch
{
public:
	const std::vector<Attribute*>& GetAttributes() const { return mAttributes; }
	GeometryBatch(std::vector<Attribute*> attributes);

	void AddComponent(MeshRendererComponent* component);
	void AddResourceMesh(ResourceMesh* mesh);

private:

	std::vector<MeshRendererComponent*> mMeshComponents;
	std::vector<ResourceMesh*> mUniqueMeshes;
	std::vector<Attribute*> mAttributes;
	std::vector<Command*> mCommands;

	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;

};

