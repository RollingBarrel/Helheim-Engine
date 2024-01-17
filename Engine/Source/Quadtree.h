#pragma once

#include <vector>
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "string"


class GameObject;
class MeshRendererComponent;

#define MAX_DEPTH 8
#define CAPACITY 4

class Quadtree
{
public:
	Quadtree(const AABB& boundingBox);
	~Quadtree();

	bool AddObject(GameObject* object);
	void RemoveObject(const GameObject* object);
	bool Intersects(const OBB* bounding_box) const;
	const bool hasGameObjects() const { return mFilled; }
	const int GetNumGameObjs() const { return mGameObjects.size(); }
	void CleanUp();
	void LoadHierarchy(GameObject* child);
	void Draw() const;
	const void RenderTreeImGui() const;
	const char* GetName() const { return mName.c_str(); }

	void UpdateDrawableGameObjects(const Frustum& myCamera);

private:
	Quadtree(const AABB& boundingBox, int depth, const char* name);
	void SplitNode();

	AABB mBoundingBox;
	Quadtree* mChildren[4];
	std::vector<GameObject*> mGameObjects;
	int mDepthLevel;
	bool mFilled;
	std::string mName;
	

};

