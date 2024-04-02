#include "Quadtree.h"
#include "GameObject.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "MeshRendererComponent.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"

#include "Application.h"
#include "ModuleDebugDraw.h"
#include "ModuleScene.h"
#include "ResourceMesh.h"
#include "Geometry/Triangle.h"
#include "imgui.h"
#include <utility>
#include <map>

Quadtree::Quadtree(const AABB& boundingBox) : Quadtree(boundingBox, 0, "R")
{
}

Quadtree::Quadtree(const AABB& boundingBox, int depth, const char* name)
{
	mBoundingBox = boundingBox;
	mDepthLevel = depth;
	mFilled = false;
	mChildren[0] = nullptr;
	mChildren[1] = nullptr;
	mChildren[2] = nullptr;
	mChildren[3] = nullptr;
	mName = name;
}

Quadtree::~Quadtree()
{
	CleanUp();
	mGameObjects.clear();
}

bool Quadtree::AddObject(GameObject* object)
{
	Component* component = object->GetComponent(ComponentType::MESHRENDERER);
	MeshRendererComponent* meshRenderer = reinterpret_cast<MeshRendererComponent*>(component);
	OBB object_BB = meshRenderer->getOBB();

	if (!mBoundingBox.Intersects(object_BB))
	{
		return false;
	}

	if (mGameObjects.size() >= CAPACITY || mFilled) 
	{
		if (mDepthLevel >= MAX_DEPTH) 
		{
			mGameObjects.push_back(object);
			return true;
		}
		if(!mFilled)
		{
			SplitNode();
		}
		mChildren[0]->AddObject(object);
		mChildren[1]->AddObject(object);
		mChildren[2]->AddObject(object);
		mChildren[3]->AddObject(object);
		mFilled = true;
		return true;
	}

	mGameObjects.push_back(object);
	return true;
}

void Quadtree::RemoveObject(const GameObject* object)
{
	if (mFilled) 
	{
		mChildren[0]->RemoveObject(object);
		mChildren[1]->RemoveObject(object);
		mChildren[2]->RemoveObject(object);
		mChildren[3]->RemoveObject(object);
	}
	else 
	{
		mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(),
			[object](GameObject* ptr) { return ptr == object; }),
			mGameObjects.end());
	}
}

bool Quadtree::Intersects(const OBB* boundingBox) const
{
	return boundingBox->Intersects(mBoundingBox);
}

bool Quadtree::Intersects(const Ray* ray) const
{
	return ray->Intersects(mBoundingBox);
}

void Quadtree::CleanUp()
{
	if (mFilled) 
	{
		mChildren[0]->CleanUp();
		mChildren[1]->CleanUp();
		mChildren[2]->CleanUp();
		mChildren[3]->CleanUp();

		delete mChildren[0];
		delete mChildren[1];
		delete mChildren[2];
		delete mChildren[3];

		mFilled = false;
	}
	else 
	{
		mGameObjects.clear();
		return;
	}
}

void Quadtree::Draw() const
{
	App->GetDebugDraw()->DrawCube(mBoundingBox, float3(0.980392f, 0.980392f, 0.823529f)); // LightGoldenYellow

	if (mFilled)
	{
		mChildren[0]->Draw();
		mChildren[1]->Draw();
		mChildren[2]->Draw();
		mChildren[3]->Draw();
	}

}

const void Quadtree::RenderTreeImGui() const
{
	
	if (mName == "")
		return;
	bool treeNodeOpened = ImGui::TreeNode(mName.c_str());
	
	if (mFilled && treeNodeOpened) 
	{
		mChildren[0]->RenderTreeImGui();
		mChildren[1]->RenderTreeImGui();
		mChildren[2]->RenderTreeImGui();
		mChildren[3]->RenderTreeImGui();

	}
	else 
	{
		if (treeNodeOpened)
		{
			for (const auto& object : mGameObjects)
			{
				ImGui::Text(object->GetName().c_str());
			}

		}
	}
	
	
	if(treeNodeOpened)
		ImGui::TreePop();
	
}

const std::set<GameObject*> Quadtree::GetObjectsInFrustum(Frustum* cam) const
{
	std::set<GameObject*> out;
	if (!cam->Intersects(mBoundingBox))
	{
		return out;
	}

	if (mFilled)
	{
		std::set<GameObject*> setA = mChildren[0]->GetObjectsInFrustum(cam);
		std::set<GameObject*> setB = mChildren[1]->GetObjectsInFrustum(cam);
		std::set<GameObject*> setC = mChildren[2]->GetObjectsInFrustum(cam);
		std::set<GameObject*> setD = mChildren[3]->GetObjectsInFrustum(cam);
		out.insert(setA.begin(), setA.end());
		out.insert(setB.begin(), setB.end());
		out.insert(setC.begin(), setC.end());
		out.insert(setD.begin(), setD.end());


	}
	else
	{
		for (auto& object : mGameObjects)
		{

			if (object->GetComponent(ComponentType::MESHRENDERER) != nullptr)
			{
				OBB temp = ((MeshRendererComponent*)object->GetComponent(ComponentType::MESHRENDERER))->getOBB();
				if (cam->Intersects(temp)) {
					out.insert(object);
				}

			}
		}
	}

	return out;
}

void Quadtree::AddHierarchyObjects(GameObject* node)
{
	for (const auto& child : node->GetChildren()) {
		//TODO Detect if the child is already inside to avoid duplicates when pressing button more than twice in a row
		if (child->GetComponent(ComponentType::MESHRENDERER) != nullptr)
		{
			AddObject(child);
		}
		AddHierarchyObjects(child);
	}
}

const std::pair<float, GameObject*> Quadtree::RayCast(Ray* ray) const
{
	if (mFilled) 
	{

		std::map<float, GameObject*> map;


		const std::pair<float, GameObject*> p1 = mChildren[0]->RayCast(ray);
		const std::pair<float, GameObject*> p2 = mChildren[1]->RayCast(ray);
		const std::pair<float, GameObject*> p3 = mChildren[2]->RayCast(ray);
		const std::pair<float, GameObject*> p4 = mChildren[3]->RayCast(ray);

		if (p1.second != nullptr) {
			map.insert(p1);
		}
		if (p2.second != nullptr) {
			map.insert(p2);
		}
		if (p3.second != nullptr) {
			map.insert(p3);
		}
		if (p4.second != nullptr) {
			map.insert(p4);
		}
			
		if (!map.empty()) {
			return std::pair<float, GameObject*>(map.begin()->first, map.begin()->second);
		}
		
	}
	else
	{
		std::map<float, GameObject*> map;
		bool intersects = false;
		bool intersectsTriangle = false;

		for (const auto& child : mGameObjects) {

			MeshRendererComponent* rMesh = (MeshRendererComponent*)child->GetComponent(ComponentType::MESHRENDERER);

			if (rMesh != nullptr) {

				Ray localRay(*ray);
				localRay.pos = child->GetWorldTransform().Inverted().MulPos(ray->pos);
				localRay.dir = child->GetWorldTransform().Inverted().MulDir(ray->dir).Normalized();

				intersects = localRay.Intersects(rMesh->GetAABB());
				if (intersects)
				{
					const unsigned int* indices = rMesh->GetResourceMesh()->GetIndices();
					const float* triangles = rMesh->GetResourceMesh()->GetAttributeData(Attribute::POS);

					for (int i = 0; i < rMesh->GetResourceMesh()->GetNumberIndices(); i += 3) {
						float3 verticeA = float3(triangles[indices[i]*3], triangles[indices[i]*3 + 1], triangles[indices[i]*3 + 2]);
						float3 verticeB = float3(triangles[indices[i + 1]*3], triangles[indices[i + 1]*3 + 1], triangles[indices[i + 1]*3 + 2]);
						float3 verticeC = float3(triangles[indices[i + 2]*3], triangles[indices[i + 2]*3 + 1], triangles[indices[i + 2]*3 + 2]);

						float distance;
						float3 hitPoint;
						intersectsTriangle = localRay.Intersects(Triangle(verticeA, verticeB, verticeC), &distance, &hitPoint);

						if (intersectsTriangle) {
							map.insert( std::pair<float, GameObject*>(distance, child));							
						}
					}
				}
			}
		}
		if (!map.empty()) {
			return std::pair<float, GameObject*>(map.begin()->first, map.begin()->second);
		}
	}

	return std::pair<float, GameObject*>(FLT_MAX,nullptr);
}

void Quadtree::UpdateTree()
{
	CleanUp();
	AddHierarchyObjects(App->GetScene()->GetRoot());
}

void Quadtree::UpdateDrawableGameObjects(const Frustum* myCamera)
{
	if (!myCamera->Intersects(mBoundingBox))
	{
		return;
	}

	if (mFilled)
	{
		mChildren[0]->UpdateDrawableGameObjects(myCamera);
		mChildren[1]->UpdateDrawableGameObjects(myCamera);
		mChildren[2]->UpdateDrawableGameObjects(myCamera);
		mChildren[3]->UpdateDrawableGameObjects(myCamera);

	}
	else 
	{
		for (auto& object : mGameObjects)
		{
			
			if (object->GetComponent(ComponentType::MESHRENDERER) != nullptr)
			{
				OBB temp = ((MeshRendererComponent*)object->GetComponent(ComponentType::MESHRENDERER))->getOBB();
				bool intersects = myCamera->Intersects(temp);
				((MeshRendererComponent*)object->GetComponent(ComponentType::MESHRENDERER))->SetInsideFrustum(intersects);

			}
		}
	}

}

void Quadtree::SplitNode()
{
	float3 minPoint = mBoundingBox.minPoint;
	float3 maxPoint = mBoundingBox.maxPoint;
	float3 center = (minPoint + maxPoint) * 0.5f;

	// Calculate points on each side
	float3 bf_x = float3(center.x, minPoint.y, minPoint.z);
	float3 bf_z = float3(minPoint.x, minPoint.y, center.z);
	float3 bf_center = float3(center.x, minPoint.y, center.z);
	float3 uf_center = float3(bf_center.x, maxPoint.y, bf_center.z);
	float3 uf_z = float3(maxPoint.x, maxPoint.y, center.z);
	float3 uf_x = float3(center.x, maxPoint.y, maxPoint.z);


	mChildren[0] = new Quadtree(AABB(minPoint, uf_center), mDepthLevel + 1, (mName + "_A").c_str());
	mChildren[1] = new Quadtree(AABB(bf_x, uf_z), mDepthLevel + 1, (mName + "_B").c_str());
	mChildren[2] = new Quadtree(AABB(bf_z, uf_x), mDepthLevel + 1, (mName + "_C").c_str());
	mChildren[3] = new Quadtree(AABB(bf_center, maxPoint), mDepthLevel + 1, (mName + "_D").c_str());

	for (const auto& object : mGameObjects)
	{
		mChildren[0]->AddObject(object);
		mChildren[1]->AddObject(object);
		mChildren[2]->AddObject(object);
		mChildren[3]->AddObject(object);
	}

	mGameObjects.clear();
}
