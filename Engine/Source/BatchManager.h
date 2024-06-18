#pragma once
#include <vector>
#include "Globals.h"
#include "MathGeoLibFwd.h"

class GeometryBatch;
class Component;
class MeshRendererComponent;

class ENGINE_API BatchManager
{
public:
	BatchManager();
	~BatchManager();
	void AddMeshComponent(const MeshRendererComponent& meshComponent);
	void RemoveMeshComponent(const MeshRendererComponent& meshComponent);
	void EditMaterial(const MeshRendererComponent& meshComponent);

	//Once per draw call
	void Update(const std::vector<const math::Frustum*>& frustums);
	void Draw(unsigned int programId, const math::Frustum& frustum);
	void EndFrameDraw();
	unsigned int GetNumBatches() const { return mBatches.size(); }

private:
	std::vector<GeometryBatch*> mBatches;
};

