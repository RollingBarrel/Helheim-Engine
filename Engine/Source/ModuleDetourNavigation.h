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
	std::vector<float3> FindNavPath(float3 startPos, float3 endPos);
	void CreateQuery(unsigned int resourceId);
	void FindDebugPoint();
	float3 FindNearestPoint(float3 center, float3 halfSize);


	float3 GetQueryCenter() const { return mQueryCenter; }
	float3 GetQueryHalfSize() const { return mQueryHalfSize; }
	float3 GetQueryResult() const { return mQueryResult; }

	void SetQueryCenter(float3 center) { mQueryCenter = center; }
	void SetQueryHalfSize(float3 halfsize) { mQueryHalfSize = halfsize; }
	unsigned int GetResourceId() const;
	void ReleaseResource();

private:
	dtNavMeshQuery* mNavQuery = nullptr;

	float3 mQueryCenter = float3(10.0f,0.0f,-3.0f);
	float3 mQueryHalfSize = float3(5.0f);
	float3 mQueryResult = float3(0.0f);

	ResourceNavMesh* mRNavMesh = nullptr;
};

