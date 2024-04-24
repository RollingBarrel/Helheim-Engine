#pragma once
#include "Module.h"
#include "vector"
#include "Math/float3.h"

struct dtNavMeshCreateParams;
//class AIAgentComponent;
class dtNavMesh;
class dtNavMeshQuery;
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
	void CreateDetourData();

	float3 FindNearestPoint(float3 center, float3 halfSize);
	//TODO: SEPARATE ENGINE
	//void DrawDebug();
	//std::vector<AIAgentComponent*>& GetAiAgentComponent() { return mAIAgentComponents; };


	float3 GetQueryCenter() const { return mQueryCenter; }
	float3 GetQueryHalfSize() const { return mQueryHalfSize; }

	void SetQueryCenter(float3 center) { mQueryCenter = center; }
	void SetQueryHalfSize(float3 halfsize) { mQueryHalfSize = halfsize; }






private:
	//std::vector<AIAgentComponent*>mAIAgentComponents;
	dtNavMeshCreateParams* mNavMeshParams = nullptr;
	dtNavMesh* mDetourNavMesh=nullptr;
	dtNavMeshQuery* mNavQuery = nullptr;


	float3 mQueryCenter = float3(10.0f,0.0f,-3.0f);
	float3 mQueryHalfSize = float3(5.0f);
	float3 mQueryResult = float3(0.0f);



};

