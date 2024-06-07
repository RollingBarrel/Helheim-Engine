#include "MeshRendererComponent.h"
#include "AnimationComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "DebugPanel.h"
#include "GeometryBatch.h"

#include "ImporterMaterial.h"

#include "float4.h"
#include "float3.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceModel.h"


MeshRendererComponent::MeshRendererComponent(GameObject* owner) : Component(owner, ComponentType::MESHRENDERER), mMesh(nullptr), mMaterial(nullptr)
{
	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();

	mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& other, GameObject* owner) : Component(owner, ComponentType::MESHRENDERER)
{
	if (other.mMesh)
	{
		SetMesh(other.mMesh->GetUID());
	}
	mOBB = other.mOBB;
	mAABB = other.mAABB;
	if (other.mMaterial)
	{
		SetMaterial(other.mMaterial->GetUID());
	}
}

MeshRendererComponent::~MeshRendererComponent()
{
	if (mMesh && mMaterial)
	{
		App->GetScene()->GetQuadtreeRoot()->RemoveObject(*this->GetOwner());
	}
	App->GetOpenGL()->BatchRemoveMesh(*this);
	if (mMesh)
	{
		App->GetResource()->ReleaseResource(mMesh->GetUID());
		mMesh = nullptr;
	}
	if (mMaterial)
	{
		App->GetResource()->ReleaseResource(mMaterial->GetUID());
		mMaterial = nullptr;
	}

	mGameobjectsInverseMatrices.clear();
	mPalette.clear();
}

void MeshRendererComponent::SetMesh(unsigned int uid)
{
	ResourceMesh* tmpMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(uid, Resource::Type::Mesh));
	if (tmpMesh)
	{
		if (mMesh)
		{
			if (mMaterial)
			{
				App->GetOpenGL()->BatchRemoveMesh(*this);
				App->GetScene()->GetQuadtreeRoot()->RemoveObject(*this->GetOwner());
			}
			App->GetResource()->ReleaseResource(mMesh->GetUID());
			mMesh = nullptr;
		}
		mMesh = tmpMesh;

		const float3* positions = reinterpret_cast<const float3*>((mMesh->GetAttributeData(Attribute::POS)));
		mAABB.SetFrom(positions, mMesh->GetNumberVertices());
		mOriginalAABB = mAABB;
		mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
		if (mMaterial)
		{
			App->GetOpenGL()->BatchAddMesh(*this);
			App->GetScene()->GetQuadtreeRoot()->AddObject(*this);
		}

	}
}


void MeshRendererComponent::SetMaterial(unsigned int uid)
{
	ResourceMaterial* tmpMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(uid, Resource::Type::Material));
	if (tmpMaterial)
	{
		if (mMaterial)
		{
			if (mMesh)
			{
				App->GetOpenGL()->BatchRemoveMesh(*this);
				App->GetScene()->GetQuadtreeRoot()->RemoveObject(*this->GetOwner());
			}
			App->GetResource()->ReleaseResource(mMaterial->GetUID());
			mMaterial = nullptr;
		}

		mMaterial = tmpMaterial;
		if (mMesh)
		{
			App->GetOpenGL()->BatchAddMesh(*this);
			App->GetScene()->GetQuadtreeRoot()->AddObject(*this);
		}
	}
	//TODO: Material Default
	//else
	//{
	//	mMaterial = new ResourceMaterial(0, float4(0.1f,0.1f,0.1f,0.1f), float3(1.0f), 1.0f ,-1,-1,-1);
	//}
}

void MeshRendererComponent::SetInvBindMatrices(std::vector<std::pair<GameObject*, float4x4>>&& bindMatrices, const MeshRendererComponent* palette)
{
	if (palette != nullptr)
	{
		mPaletteOwner = palette;
	}
	else
	{
		mGameobjectsInverseMatrices = std::move(bindMatrices);
	}
	mHasSkinning = true;
}

void MeshRendererComponent::UpdateSkeletonObjects(const std::unordered_map<const GameObject*, GameObject*>& originalToNew)
{
	assert(mHasSkinning && "Component does not have skinning");
	if (mPaletteOwner != nullptr)
	{
		assert(originalToNew.find(mPaletteOwner->GetOwner()) != originalToNew.end() && originalToNew.at(mPaletteOwner->GetOwner())->GetComponent(ComponentType::MESHRENDERER) != nullptr);
		mPaletteOwner = reinterpret_cast<MeshRendererComponent*>(originalToNew.at(mPaletteOwner->GetOwner())->GetComponent(ComponentType::MESHRENDERER));
	}
	else
	{
		assert(mGameobjectsInverseMatrices.size() && "Component does not have skeleton");
		for (auto& pair : mGameobjectsInverseMatrices)
		{
			assert(originalToNew.find(pair.first) == originalToNew.end());
			pair.first = originalToNew.at(pair.first);
		}
	}
}


void MeshRendererComponent::Update() 
{
	if (mOwner->HasUpdatedTransform())
	{
		RefreshBoundingBoxes();
	}

	UpdatePalette();
}

//void MeshRendererComponent::Enable()
//{
//	if(mMaterial && mMesh)
//		App->GetOpenGL()->BatchAddMesh(this);
//}
//
//void MeshRendererComponent::Disable()
//{
//	if (mMaterial && mMesh)
//		App->GetOpenGL()->BatchRemoveMesh(this);
//}

Component* MeshRendererComponent::Clone(GameObject* owner) const
{

	return new MeshRendererComponent(*this, owner);
}

void MeshRendererComponent::RefreshBoundingBoxes()
{	
	mOBB = OBB(mOriginalAABB);
	mOBB.Transform(mOwner->GetWorldTransform());

	mAABB.SetFrom(mOBB);
}

void MeshRendererComponent::Save(JsonObject& obj) const 
{
	Component::Save(obj);
	obj.AddInt("MeshID", mMesh->GetUID());
	obj.AddInt("MaterialID", mMaterial->GetUID());
}

void MeshRendererComponent::Load(const JsonObject& data) 
{
	Component::Load(data);

	SetMesh(data.GetInt("MeshID"));
	SetMaterial(data.GetInt("MaterialID"));
}

void MeshRendererComponent::UpdatePalette()
{
	mPalette.clear();
	mPalette.reserve(mGameobjectsInverseMatrices.size());
	for (unsigned i = 0; i < mGameobjectsInverseMatrices.size(); ++i)
	{
		mPalette.push_back((mGameobjectsInverseMatrices[i].first->GetWorldTransform() * mGameobjectsInverseMatrices[i].second).Transposed());
	}

}

