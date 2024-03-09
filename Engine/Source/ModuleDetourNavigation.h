#pragma once
#include "Module.h"
#include "vector"
struct dtNavMeshCreateParams;
class AIAgentComponent;
class dtNavMesh;
class ModuleDetourNavigation :public Module
{
public:
	ModuleDetourNavigation();
	~ModuleDetourNavigation();
	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	void CreateDetourData();
private:
	std::vector<const AIAgentComponent*>mAIAgentComponents;
	dtNavMeshCreateParams* mNavMeshParams = nullptr;
	dtNavMesh* mDetourNavMesh;
};

