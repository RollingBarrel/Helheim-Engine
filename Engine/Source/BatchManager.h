#pragma once
#include <vector>
#include "Globals.h"

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

	void AddCommand(const MeshRendererComponent& meshComponent);
	void CleanUpCommands();
	//TODO: mhan de passar el command buffer
	//void Draw();
	void EndFrameDraw();
	unsigned int GetNumBatches() const { return mBatches.size(); }
	GeometryBatch* operator[](unsigned int idx) { return mBatches[idx]; }

private:
	std::vector<GeometryBatch*> mBatches;
};

