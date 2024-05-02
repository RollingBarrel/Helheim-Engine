#pragma once
#include <vector>
#include "Globals.h"

class GeometryBatch;
class MeshRendererComponent;

class ENGINE_API BatchManager
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

