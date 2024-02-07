#pragma once
#include <vector>

class GeometryBatch;
struct ResourceMesh;
class BatchManager
{
	std::vector<GeometryBatch*> mBatches;



	GeometryBatch* RequiredBatch(ResourceMesh*  meshComponent);
};

