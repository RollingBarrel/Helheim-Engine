#pragma once
#include "Module.h"
#include "vector"
#include "Math/float3.h"

struct dtNavMeshCreateParams;
class dtNavMesh;
class dtNavMeshQuery;
class ResourceNavMesh;

class ENGINE_API ModuleDetourNavigation :public Module
{
public:
	ModuleDetourNavigation();
	~ModuleDetourNavigation();
	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	std::vector<float3> FindNavPath(float3 startPos, float3 endPos);
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;
	void CreateQuery();
	void LoadResourceData();
	void FindDebugPoint();
	float3 FindNearestPoint(float3 center, float3 halfSize);


	float3 GetQueryCenter() const { return mQueryCenter; }
	float3 GetQueryHalfSize() const { return mQueryHalfSize; }
	float3 GetQueryResult() const { return mQueryResult; }
	dtNavMesh* GetDetourNavMesh()const { return mDetourNavMesh; }

	void SetQueryCenter(float3 center) { mQueryCenter = center; }
	void SetQueryHalfSize(float3 halfsize) { mQueryHalfSize = halfsize; }
	void SetDetourNavMesh(dtNavMesh* detourNavMesh) { mDetourNavMesh = detourNavMesh; }

private:
	dtNavMesh* mDetourNavMesh=nullptr;
	dtNavMeshQuery* mNavQuery = nullptr;

	float3 mQueryCenter = float3(10.0f,0.0f,-3.0f);
	float3 mQueryHalfSize = float3(5.0f);
	float3 mQueryResult = float3(0.0f);

	ResourceNavMesh* mResourceNavMesh = nullptr;
};

