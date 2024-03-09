#pragma once
#include "Module.h"
#include "vector"
struct dtNavMeshCreateParams;
class AIAgentComponent;
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
	std::vector<AIAgentComponent*>& GetAiAgentComponent() { return mAIAgentComponents; };
private:
	std::vector<AIAgentComponent*>mAIAgentComponents;
	dtNavMeshCreateParams* mNavMeshParams = nullptr;
	dtNavMesh* mDetourNavMesh=nullptr;
	dtNavMeshQuery* mNavQuery = nullptr;
};

