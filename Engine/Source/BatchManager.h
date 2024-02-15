#pragma once
#include <vector>

class GeometryBatch;
class MeshRendererComponent;
struct ResourceMesh;
class BatchManager
{
public:
	BatchManager();
	~BatchManager();
	void AddMeshRendererComponent(MeshRendererComponent* meshComponent);

	void Draw();

private:
	std::vector<GeometryBatch*> mBatches;
};

