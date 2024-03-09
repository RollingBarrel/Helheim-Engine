#pragma once
#include "vector"
#include "Geometry/OBB.h"
#include "Recast.h"
class Material;
struct ResourceMesh;
class MeshRendererComponent;
class AIAgentComponent;
class GameObject;

class NavMeshController
{
public:
	NavMeshController();
	void Reset();
	~NavMeshController();
	void HandleBuild();
	void Update();


	//IMGUI VALUES
	float GetCellSize() const { return mCellSize; }
	void SetCellSize(float value) { mCellSize = value; }

	float GetCellHeight() const { return mCellHeight; }
	void SetCellHeight(float value) { mCellHeight = value; }

	unsigned int GetMaxSlopeAngle() const { return mMaxSlopeAngle; }
	void SetMaxSlopeAngle(unsigned int value) { mMaxSlopeAngle = value; }

	float GetWalkableRadius() const { return mWalkableRadius; }
	void SetWalkableRadius(float value) { mWalkableRadius = value; }

	int GetMinRegionArea() const { return mMinRegionArea; }
	void SetMinRegionArea(int value) { mMinRegionArea = value; }

	int GetMergeRegionArea() const { return mMergeRegionArea; }
	void SetMergeRegionArea(int value) { mMergeRegionArea = value; }

	float GetMaxSimplificationError() const { return mMaxSimplificationError; }
	void SetMaxSimplificationError(float value) { mMaxSimplificationError = value; }

	int GetMaxEdgeLen() const { return mMaxEdgeLen; }
	void SetMaxEdgeLen(int value) { mMaxEdgeLen = value; }

	int GetMaxVertsPerPoly() const { return mMaxVertsPerPoly; }
	void SetMaxVertsPerPoly(int value) { mMaxVertsPerPoly = value; }

	float GetDetailSampleDist() const { return mDetailSampleDist; }
	void SetDetailSampleDist(float value) { mDetailSampleDist = value; }

	float GetDetailSampleMaxError() const { return mDetailSampleMaxError; }
	void SetDetailSampleMaxError(float value) { mDetailSampleMaxError = value; }



private:
	void GetGOMeshes(const GameObject* gameObj);
	std::vector<const ResourceMesh*> mMeshesToNavMesh;
	std::vector<const MeshRendererComponent*> mMeshRendererComponents;
	std::vector<const AIAgentComponent*>mAIAgentComponents;
	void TranslateIndices();
	void DebugDrawPolyMesh();
	void LoadDrawMesh();
	int FindVertexIndex(float3 vert);

	rcHeightfield* mHeightField = nullptr;
	rcCompactHeightfield* mCompactHeightField = nullptr;
	rcContourSet* mContourSet = nullptr;
	rcPolyMesh* mPolyMesh = nullptr;
	rcPolyMeshDetail* mPolyMeshDetail = nullptr;
	unsigned char* mTriangleAreas = nullptr;
	rcContext mRecastContext;
	bool mKeepInterResults = false;
	bool mFilterLowHangingObstacles;
	bool mFilterLedgeSpans;
	bool mFilterWalkableLowHeightSpans;

	int mWalkableClimb = 1;   // no imgui
	int mWalkableHeight = 1; // no imgui


	//IMGUI VALUES
	float mCellSize = 0.30f;  // 0.1 - 1.0
	float mCellHeight = 0.20f; // 0.1 - 1.0

	unsigned int mMaxSlopeAngle = 20; // 0 - 90
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

