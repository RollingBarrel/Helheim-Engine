#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>

#include "Geometry/AABB.h"
#include "MathGeoLibFwd.h"

class GameObject;
class MeshRendererComponent;
struct Hit;
#define MAX_DEPTH 6
#define CAPACITY 8

class Quadtree
{
public:
	Quadtree(const AABB& boundingBox);
	~Quadtree();

	bool AddObject(GameObject* object);
	void RemoveObject(const GameObject* object);
	bool Intersects(const OBB* boundingBox) const;
	bool Intersects(const Ray* ray) const;
	const bool IsFilled() const { return mFilled; }
	const unsigned int GetNumGameObjs() const { return static_cast<unsigned int>(mGameObjects.size()); }
	const std::vector<GameObject*>& GetGameObjects() const { return mGameObjects; }
	const Quadtree* GetChildren() const { return *mChildren; }
	const std::map<float, Hit> RayCast(Ray* ray) const;
	void UpdateTree();
	const char* GetName() const { return mName.c_str(); }
	const std::set<GameObject*> GetObjectsInFrustum(Frustum* cam) const;
	const AABB& GetBoundingBox() const { return mBoundingBox; }

	void UpdateDrawableGameObjects(const Frustum* myCamera);
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

