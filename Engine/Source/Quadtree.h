#pragma once

#include <vector>
#include <string>

#include "Geometry/AABB.h"
#include "MathGeoLibFwd.h"

class GameObject;
class MeshRendererComponent;
class pair;
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
	const int GetNumGameObjs() const { return mGameObjects.size(); }
	const std::vector<GameObject*>& GetGameObjects() const { return mGameObjects; }
	void GetChildren(const Quadtree** out) const { for (int i = 0; i < 3; ++i) out[i] = mChildren[i]; }
	void GetGameObjects(std::vector<GameObject*>& out) const { 
		out.reserve(mGameObjects.size());
		for (GameObject* obj : mGameObjects) 
			out.push_back(obj); 
	}
	const std::pair<float, GameObject*> RayCast(Ray* ray) const;
	void UpdateTree();
	void Draw() const;
	const char* GetName() const { return mName.c_str(); }
	bool GetFilled() const { return mFilled; }

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

