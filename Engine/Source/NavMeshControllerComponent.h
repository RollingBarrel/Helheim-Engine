#pragma once
#include "Component.h"
#include "vector"
#include "Geometry/OBB.h"
#include "Recast.h"
class Material;
struct ResourceMesh;
class MeshRendererComponent;
class NavMeshControllerComponent : public Component
{
public:
	NavMeshControllerComponent(GameObject* ownerGameObject);
	NavMeshControllerComponent(const NavMeshControllerComponent& original, GameObject* owner);
	void Reset();
	~NavMeshControllerComponent();
	void HandleBuild();

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
	void GetGOMeshes(const GameObject* gameObj);
	std::vector<const ResourceMesh*> mMeshesToNavMesh;
	std::vector<OBB> mOBBs;
	std::vector<const MeshRendererComponent*> mMeshRendererComponents;

	rcHeightfield*  mHeightField=nullptr;
	rcCompactHeightfield* mCompactHeightField = nullptr;
	rcContourSet* mContourSet = nullptr;
	rcPolyMesh* mPolyMesh = nullptr;
	rcPolyMeshDetail* mPolyMeshDetail= nullptr;
	unsigned char* mTriangleAreas = nullptr;
	rcContext mRecastContext;
	bool mKeepInterResults = false;
	bool mFilterLowHangingObstacles;
	bool mFilterLedgeSpans;
	bool mFilterWalkableLowHeightSpans;

	//IMGUI VALUES
	float mCellSize = 0.30f;
	float mCellHeight = 0.20f;
	unsigned int mMaxSlopeAngle = 20;
	int mWalkableClimb = 1;
	int mWalkableHeight = 1;
	float mWalkableRadius = 0.6f;
	int mMinRegionArea = 8;
	int mMergeRegionArea = 20;
	float mMaxSimplificationError = 1.3f;
	int mMaxEdgeLen = 12;
	int mMaxVertsPerPoly = 6;
	float mDetailSampleDist = 6;
	float mDetailSampleMaxError = 1;
};

