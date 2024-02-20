#pragma once

#include <vector>
#include <string>

#include "Geometry/AABB.h"
#include "MathGeoLibFwd.h"

class GameObject;
class MeshRendererComponent;

#define MAX_DEPTH 6
#define CAPACITY 1 //8

class Quadtree
{
public:
	Quadtree(const AABB& boundingBox);
	~Quadtree();

	bool AddObject(GameObject* object);
	void RemoveObject(const GameObject* object);
	bool Intersects(const OBB* bounding_box) const;
	bool Intersects(const Ray* bounding_box) const;
	const bool IsFilled() const { return mFilled; }
	const int GetNumGameObjs() const { return mGameObjects.size(); }
	const std::vector<GameObject*>& GetGameObjects() const { return mGameObjects; }
	const Quadtree* GetChildren() const { return *mChildren; }
	const GameObject* RayCast(Ray* ray) const;
	void UpdateTree();
	void Draw() const;
	const void RenderTreeImGui() const;
	const char* GetName() const { return mName.c_str(); }

	void UpdateDrawableGameObjects(const Frustum& myCamera);
	void CleanUp();

private:
	Quadtree(const AABB& boundingBox, int depth, const char* name);
	void SplitNode();
	void AddHierarchyObjects(GameObject* child);

	AABB mBoundingBox;
	Quadtree* mChildren[4];
	std::vector<GameObject*> mGameObjects;
	int mDepthLevel;
	bool mFilled;
	bool mHasChildren = false;
	std::string mName;
	

};

