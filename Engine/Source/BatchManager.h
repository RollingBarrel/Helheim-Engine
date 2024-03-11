#pragma once
#include <vector>

class GeometryBatch;
class MeshRendererComponent;
class ResourceMesh;
class BatchManager
{
public:
	BatchManager();
	~BatchManager();
	void AddMeshComponent(MeshRendererComponent* meshComponent);
	void RemoveMeshComponent(MeshRendererComponent* meshComponent);

	void Draw();

private:
	std::vector<GeometryBatch*> mBatches;
};

