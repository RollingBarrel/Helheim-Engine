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

	void Draw(const std::vector<const MeshRendererComponent*>& renderList);

private:
	std::vector<GeometryBatch*> mBatches;
};

