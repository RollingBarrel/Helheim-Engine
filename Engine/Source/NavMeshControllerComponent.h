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
	std::vector<const MeshRendererComponent*> mMeshRendererComponents;
	void TranslateIndices();
	void DebugDrawPolyMesh();
	void LoadDrawMesh();
	int FindVertexIndex(float3 vert);

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
	float mCellSize = 0.30f;  // 0.1 - 1.0
	float mCellHeight = 0.20f; // 0.1 - 1.0

	unsigned int mMaxSlopeAngle = 20; // 0 - 90
	int mWalkableClimb = 1;   // 0.1 - 5.0 ? 
	int mWalkableHeight = 1; // 
	float mWalkableRadius = 0.6f; // 0.0 - 5.0
	int mMinRegionArea = 8; // 0 - 150
	int mMergeRegionArea = 20; // 0 - 150
	float mMaxSimplificationError = 1.3f; // 0.1 - 3.0
	int mMaxEdgeLen = 12;  // 0 - 50
	int mMaxVertsPerPoly = 6; // 3 - 12
	float mDetailSampleDist = 6; // 0 - 16
	float mDetailSampleMaxError = 1; // 0 - 16

	//DEBUG DRAW VARIABLES
	bool mDraw = true;
	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;
	std::vector<float3> mVertices;
	std::vector<int> mIndices;


};

