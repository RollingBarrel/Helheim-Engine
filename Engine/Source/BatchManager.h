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

private:

	std::vector<GeometryBatch*> mBatches;



	
};

