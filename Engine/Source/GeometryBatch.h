#pragma once
#include <vector>;

class MeshRendererComponent;
struct ResourceMesh;
typedef struct Attribute;

typedef struct Command;


class GeometryBatch
{
public:

	GeometryBatch(MeshRendererComponent* mesh);
	~GeometryBatch();

	const std::vector<Attribute*>& GetAttributes() const { return mAttributes; }
	
	void AddMesh(MeshRendererComponent* component);
	
	void Draw();

private:

	std::vector<MeshRendererComponent*> mMeshComponents;
	std::vector<ResourceMesh*> mUniqueMeshes;
	std::vector<Attribute*> mAttributes;
	std::vector<Command*> mCommands;

	unsigned int mVertexSize = 0;

	unsigned int mVboSize = 0;
	unsigned int mEboSize = 0;
	unsigned int mVaoSize = 0;

	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;
	unsigned int mIbo = 0;

	

};

