#pragma once
#include <vector>

class GeometryBatch;
class MeshRendererComponent;

class BatchManager
{
public:
	BatchManager();
	~BatchManager();
	void AddMeshComponent(const MeshRendererComponent* meshComponent);
	void RemoveMeshComponent(const MeshRendererComponent* meshComponent);
	void EditMaterial(const MeshRendererComponent* meshComponent);

	void Draw();

private:
	std::vector<GeometryBatch*> mBatches;
};

