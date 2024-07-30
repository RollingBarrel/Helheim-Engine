#include "MeshRendererComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "ModuleScene.h"

#include "float4.h"
#include "float3.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Algorithm/Random/LCG.h"


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
		if (other.mUniqueMaterial)
		{
			mMaterial = new ResourceMaterial(LCG().Int(), nullptr,
				other.mMaterial->GetBaseColorFactor().ptr(), other.mMaterial->GetMetallicFactor(), other.mMaterial->GetRoughnessFactor(), other.mMaterial->GetEmissiveFactor().ptr(),
				other.mMaterial->GetBaseColorTexture()->GetUID(), other.mMaterial->GetMetallicRoughnessTexture()->GetUID(), other.mMaterial->GetNormalTexture()->GetUID(), other.mMaterial->GetEmissiveTexture()->GetUID(),
				other.mMaterial->IsBaseColorEnabled(), other.mMaterial->IsMetallicRoughnessEnabled(), other.mMaterial->IsNormalMapEnabled(), other.mMaterial->IsEmissiveEnabled());
			mUniqueMaterial = true;
		}
		else
		{
			SetMaterial(other.mMaterial->GetUID());
		}
	}
	mHasSkinning = other.mHasSkinning;
	mPaletteOwner = other.mPaletteOwner;
	for (int i = 0; i < other.mGameobjectsInverseMatrices.size(); ++i)
	{
		mGameobjectsInverseMatrices.push_back(other.mGameobjectsInverseMatrices[i]);
	}
}

MeshRendererComponent::~MeshRendererComponent()
{
	App->GetOpenGL()->BatchRemoveMesh(*this);
	if (mMesh)
	{
		App->GetResource()->ReleaseResource(mMesh->GetUID());
		mMesh = nullptr;
	}
	if (mMaterial)
	{
		if (mUniqueMaterial)
		{
			delete mMaterial;
		}
		else 
		{
			App->GetResource()->ReleaseResource(mMaterial->GetUID());
		}

		mMaterial = nullptr;
	}

	mGameobjectsInverseMatrices.clear();
	mPalette.clear();
}

void MeshRendererComponent::SetMesh(unsigned int uid)
{
	ResourceMesh* tmpMesh = static_cast<ResourceMesh*>(App->GetResource()->RequestResource(uid, Resource::Type::Mesh));
	if (tmpMesh)
	{
		if (mMesh)
		{
			if (mMaterial)
			{
				App->GetOpenGL()->BatchRemoveMesh(*this);
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
		}

	}
}


void MeshRendererComponent::SetMaterial(unsigned int uid)
{
	ResourceMaterial* tmpMaterial = static_cast<ResourceMaterial*>(App->GetResource()->RequestResource(uid, Resource::Type::Material));
	if (tmpMaterial)
	{
		if (mMaterial)
		{
			if (mMesh)
			{
				App->GetOpenGL()->BatchRemoveMesh(*this);
			}
			if (mUniqueMaterial)
				delete mMaterial;
			else
				App->GetResource()->ReleaseResource(mMaterial->GetUID());
			mMaterial = nullptr;
		}

		mMaterial = tmpMaterial;
		if (mMesh)
		{
			App->GetOpenGL()->BatchAddMesh(*this);
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
		mPaletteOwner = static_cast<MeshRendererComponent*>(originalToNew.at(mPaletteOwner->GetOwner())->GetComponent(ComponentType::MESHRENDERER));
	}
	else
	{
		assert(mGameobjectsInverseMatrices.size() && "Component does not have skeleton");
		for (auto& pair : mGameobjectsInverseMatrices)
		{
			assert(originalToNew.find(pair.first) != originalToNew.end());
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

void MeshRendererComponent::Enable()
{
	if(mMaterial && mMesh)
		App->GetOpenGL()->BatchAddMesh(*this);
}

void MeshRendererComponent::Disable()
{
	if (mMaterial && mMesh)
		App->GetOpenGL()->BatchRemoveMesh(*this);
}

void MeshRendererComponent::CreateUiqueMaterial()
{
	assert(mMaterial && mMesh);
	if (!mUniqueMaterial)
	{
		App->GetOpenGL()->BatchRemoveMesh(*this);
		ResourceMaterial* rMat = new ResourceMaterial(LCG().Int(), nullptr,
			mMaterial->GetBaseColorFactor().ptr(), mMaterial->GetMetallicFactor(), mMaterial->GetRoughnessFactor(), mMaterial->GetEmissiveFactor().ptr(),
			mMaterial->GetBaseColorTexture()->GetUID(), mMaterial->GetMetallicRoughnessTexture()->GetUID(), mMaterial->GetNormalTexture()->GetUID(), mMaterial->GetEmissiveTexture()->GetUID(),
			mMaterial->IsBaseColorEnabled(), mMaterial->IsMetallicRoughnessEnabled(), mMaterial->IsNormalMapEnabled(), mMaterial->IsEmissiveEnabled());
		App->GetResource()->ReleaseResource(mMaterial->GetUID());
		mMaterial = rMat;
		App->GetOpenGL()->BatchAddMesh(*this);
		mUniqueMaterial = true;
	}
}

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
	obj.AddBool("HasSkinning", mHasSkinning);
	if (mPaletteOwner)
		obj.AddInt("PaletteOwner", mPaletteOwner->GetOwner()->GetID());
	else
		obj.AddInt("PaletteOwner", 0);
	JsonArray arr = obj.AddNewJsonArray("InverseBindMatrices");
	for (int i = 0; i < mGameobjectsInverseMatrices.size(); ++i)
	{
		JsonObject obj = arr.PushBackNewObject();
		obj.AddInt("GoId", mGameobjectsInverseMatrices[i].first->GetID());
		obj.AddFloats("Matrix", mGameobjectsInverseMatrices[i].second.ptr(), 16);
	}
}

void MeshRendererComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	if(data.HasMember("MeshID"))
		SetMesh(data.GetInt("MeshID"));
	if(data.HasMember("MaterialID"))
		SetMaterial(data.GetInt("MaterialID"));
	if(data.HasMember("HasSkinning"))
		mHasSkinning = data.GetBool("HasSkinning");

	if (data.HasMember("PaletteOwner"))
	{
		unsigned int id = data.GetInt("PaletteOwner");

		const std::unordered_map<unsigned int, unsigned int>& uids = App->GetScene()->GetPrefabUIDMap();
		std::unordered_map<unsigned int, unsigned int>::const_iterator has = uids.find(id);
		if (has != uids.end())
		{
			std::unordered_map<unsigned int, GameObject*>::const_iterator got = uidPointerMap.find(uids.at(id));
			if (got != uidPointerMap.end())
			{
				mPaletteOwner = static_cast<MeshRendererComponent*>(uidPointerMap.at(uids.at(id))->GetComponent(ComponentType::MESHRENDERER));
			}
		}
		else
		{
			std::unordered_map<unsigned int, GameObject*>::const_iterator got = uidPointerMap.find(id);
			if (got != uidPointerMap.end())
			{
				mPaletteOwner = static_cast<MeshRendererComponent*>(uidPointerMap.at(id)->GetComponent(ComponentType::MESHRENDERER));
			}
		}

	}

	if (data.HasMember("InverseBindMatrices"))
	{
		JsonArray arr = data.GetJsonArray("InverseBindMatrices");
		for (unsigned int i = 0; i < arr.Size(); ++i)
		{
			JsonObject obj = arr.GetJsonObject(i);
			if (obj.HasMember("GoId"))
			{
				int UID = obj.GetInt("GoId");
				if (UID != -1)
				{
					const std::unordered_map<unsigned int, unsigned int>& uids = App->GetScene()->GetPrefabUIDMap();
					GameObject* ptr;

					if (!uids.empty())
					{
						std::unordered_map<unsigned int, GameObject*>::const_iterator got = uidPointerMap.find(uids.at(UID));
						if (got != uidPointerMap.end())
						{
							ptr = uidPointerMap.at(uids.at(UID));
						}
					}
					else
					{
						std::unordered_map<unsigned int, GameObject*>::const_iterator got = uidPointerMap.find(UID);
						if (got != uidPointerMap.end())
						{
							ptr = uidPointerMap.at(UID);
						}
					}
					float matrix[16];
					if (obj.HasMember("Matrix"))
					{
						obj.GetFloats("Matrix", matrix);
						mGameobjectsInverseMatrices.emplace_back(ptr,
							float4x4(matrix[0], matrix[1], matrix[2], matrix[3],
								matrix[4], matrix[5], matrix[6], matrix[7],
								matrix[8], matrix[9], matrix[10], matrix[11],
								matrix[12], matrix[13], matrix[14], matrix[15]));
					}
				}
			}
		}
	}
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

void MeshRendererComponent::SetEnableBaseColorTexture(bool baseColorTex)
{
	assert(mMaterial);
	mMaterial->SetEnableBaseColorTexture(baseColorTex);
	App->GetOpenGL()->BatchEditMaterial(*this);
}

void MeshRendererComponent::SetBaseColorFactor(const float4& baseColor)
{ 
	assert(mMaterial);
	mMaterial->SetBaseColorFactor(baseColor);
	App->GetOpenGL()->BatchEditMaterial(*this);
}