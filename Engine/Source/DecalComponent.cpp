#include "DecalComponent.h"
#include "Application.h"

#include "ModuleResource.h"
#include "ModuleOpenGL.h"

#include "ResourceTexture.h"

DecalComponent::DecalComponent(GameObject* owner) : Component(owner, ComponentType::DECAL)
{
	App->GetOpenGL()->AddDecal(*this);

	mDiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	mEmisiveColor = { 1.0f, 1.0f, 1.0f, 1.0f };
}

DecalComponent::DecalComponent(const DecalComponent& other, GameObject* owner) : Component(owner, ComponentType::DECAL), 
mDiffuseName(other.mDiffuseName), mDiffuseTexture(other.mDiffuseTexture), mSpecularName(other.mSpecularName), mSpecularTexture(other.mSpecularTexture),
mNormalName(other.mNormalName), mNormalTexture(other.mNormalTexture), mEmisiveName(other.mEmisiveName), mEmisiveTexture(other.mEmisiveTexture),
mDiffuseColor(other.mDiffuseColor), mEmisiveColor(other.mEmisiveColor)
{
	App->GetOpenGL()->AddDecal(*this);
}

DecalComponent::~DecalComponent()
{
	App->GetOpenGL()->RemoveDecal(*this);
	Reset();
}

unsigned int DecalComponent::GetDiffuseId() const
{
	if (mDiffuseTexture)
	{
		return mDiffuseTexture->GetOpenGLId();
	}
	return 0;
}

unsigned int DecalComponent::GetSpecularId() const
{
	if (mSpecularTexture)
	{
		return mSpecularTexture->GetOpenGLId();
	}
	return 0;
}

unsigned int DecalComponent::GetNormalId() const
{
	if (mNormalTexture)
	{
		return mNormalTexture->GetOpenGLId();
	}
	return 0;
}

unsigned int DecalComponent::GetEmisiveId() const
{
	if (mEmisiveTexture)
	{
		return mEmisiveTexture->GetOpenGLId();
	}
	return 0;
}

void DecalComponent::Reset()
{
	mDiffuseName = "";
	if (mDiffuseTexture)
		App->GetResource()->ReleaseResource(mDiffuseTexture->GetUID());
	mDiffuseTexture = nullptr;

	mSpecularName = "";
	if (mSpecularTexture)
		App->GetResource()->ReleaseResource(mSpecularTexture->GetUID());
	mSpecularTexture = nullptr;

	mNormalName = "";
	if (mNormalTexture)
		App->GetResource()->ReleaseResource(mNormalTexture->GetUID());
	mNormalTexture = nullptr;

	mEmisiveName = "";
	if (mEmisiveTexture)
		App->GetResource()->ReleaseResource(mEmisiveTexture->GetUID());
	mEmisiveTexture = nullptr;

	mDiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	mEmisiveColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	
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
		obj.AddString("DiffuseName", mDiffuseName.c_str());
		obj.AddFloats("DiffuseColor", mDiffuseColor.ptr(), 4);
	}
	
	if (mSpecularTexture)
	{
		obj.AddInt("SpecularID", mSpecularTexture->GetUID());
		obj.AddString("SpecularName", mSpecularName.c_str());
	}
	
	if (mNormalTexture)
	{
		obj.AddInt("NormalID", mNormalTexture->GetUID());
		obj.AddString("NormalName", mNormalName.c_str());
	}
	
	if (mEmisiveTexture)
	{
		obj.AddInt("EmisiveID", mEmisiveTexture->GetUID());
		obj.AddString("EmisiveName", mEmisiveName.c_str());
		obj.AddFloats("EmisiveColor", mEmisiveColor.ptr(), 4);
	}

}

void DecalComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
	
	if (data.HasMember("DiffuseID"))
	{
		mDiffuseTexture = (ResourceTexture*)App->GetResource()->RequestResource(data.GetInt("DiffuseID"), Resource::Type::Texture);
		mDiffuseTexture->GenerateMipmaps();
		mDiffuseName = data.GetString("DiffuseName");
		data.GetFloats("DiffuseColor", mDiffuseColor.ptr());
		
	}
	
	if (data.HasMember("SpecularID"))
	{
		mSpecularTexture = (ResourceTexture*)App->GetResource()->RequestResource(data.GetInt("SpecularID"), Resource::Type::Texture);
		mSpecularTexture->GenerateMipmaps();
		mSpecularName = data.GetString("SpecularName");
		
	}
	
	if (data.HasMember("NormalID"))
	{
		mNormalTexture = (ResourceTexture*)App->GetResource()->RequestResource(data.GetInt("NormalID"), Resource::Type::Texture);
		mNormalTexture->GenerateMipmaps();
		mNormalName = data.GetString("NormalName");
		
	}

	if (data.HasMember("EmisiveID"))
	{
		mEmisiveTexture = (ResourceTexture*)App->GetResource()->RequestResource(data.GetInt("EmisiveID"), Resource::Type::Texture);
		mEmisiveTexture->GenerateMipmaps();
		mEmisiveName = data.GetString("EmisiveName");
		data.GetFloats("EmisiveColor", mEmisiveColor.ptr());
	}
	
}

void DecalComponent::Enable()
{
	App->GetOpenGL()->AddDecal(*this);
}

void DecalComponent::Disable()
{
	App->GetOpenGL()->RemoveDecal(*this);
}
