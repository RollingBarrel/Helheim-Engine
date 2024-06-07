#include "DecalComponent.h"
#include "Application.h"

#include "ModuleResource.h"

#include "ResourceTexture.h"

DecalComponent::DecalComponent(GameObject* owner) : Component(owner, ComponentType::DECAL)
{

}

DecalComponent::DecalComponent(const DecalComponent& other, GameObject* owner) : Component(owner, ComponentType::DECAL)
{

}

DecalComponent::~DecalComponent()
{
}

void DecalComponent::Update()
{
}

Component* DecalComponent::Clone(GameObject* owner) const
{
	return new DecalComponent(*this, owner);
}

void DecalComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);

	if (mDiffuseTexture)
	{
		obj.AddInt("DiffuseID", mDiffuseTexture->GetUID());
	}
	obj.AddString("DiffuseName", mDiffuseName.c_str());
	
	if (mSpecularTexture)
	{
		obj.AddInt("SpecularID", mSpecularTexture->GetUID());
	}
	obj.AddString("SpecularName", mSpecularName.c_str());

	if (mNormalTexture)
	{
		obj.AddInt("NormalID", mNormalTexture->GetUID());
	}
	obj.AddString("NormalName", mNormalName.c_str());

}

void DecalComponent::Load(const JsonObject& data)
{
	Component::Load(data);
	
	if (data.HasMember("DiffuseID"))
	{
		mDiffuseTexture = (ResourceTexture*)App->GetResource()->RequestResource(data.GetInt("DiffuseID"), Resource::Type::Texture);
		if (data.HasMember("DiffuseName"))
		{
			mDiffuseName = data.GetString("DiffuseName");
		}
	}
	
	if (data.HasMember("SpecularID"))
	{
		mSpecularTexture = (ResourceTexture*)App->GetResource()->RequestResource(data.GetInt("SpecularID"), Resource::Type::Texture);
		if (data.HasMember("SpecularName"))
		{
			mDiffuseName = data.GetString("SpecularName");
		}
	}
	
	if (data.HasMember("NormalID"))
	{
		mNormalTexture = (ResourceTexture*)App->GetResource()->RequestResource(data.GetInt("NormalID"), Resource::Type::Texture);
		if (data.HasMember("NormalName"))
		{
			mDiffuseName = data.GetString("NormalName");
		}
	}

	
}
