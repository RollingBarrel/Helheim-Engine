#pragma once
#include "Module.h"
#include "vector"
#include "float3.h"

struct dtNavMeshCreateParams;
//class AIAgentComponent;
class dtNavMesh;
class dtNavMeshQuery;
class ModuleDetourNavigation :public Module
{
public:
	ModuleDetourNavigation();
	~ModuleDetourNavigation();
	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;
	void CreateDetourData();
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

