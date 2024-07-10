#include "DecalComponent.h"
#include "Application.h"

#include "ModuleResource.h"
#include "ModuleOpenGL.h"

#include "ResourceTexture.h"

DecalComponent::DecalComponent(GameObject* owner) : Component(owner, ComponentType::DECAL)
{
	if (IsEnabled())
	{
		App->GetOpenGL()->AddDecal(*this);
	}
	

	mDiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	mEmisiveColor = { 1.0f, 1.0f, 1.0f, 1.0f };
}

DecalComponent::DecalComponent(const DecalComponent& other, GameObject* owner) : Component(owner, ComponentType::DECAL), 
mDiffuseName(other.mDiffuseName), mSpecularName(other.mSpecularName),
mNormalName(other.mNormalName), mEmisiveName(other.mEmisiveName),
mDiffuseColor(other.mDiffuseColor), mEmisiveColor(other.mEmisiveColor)
{
	if(other.mDiffuseTexture)
		App->GetResource()->RequestResource(other.mDiffuseTexture->GetUID(), Resource::Type::Texture);
	if (other.mSpecularTexture)
		App->GetResource()->RequestResource(other.mSpecularTexture->GetUID(), Resource::Type::Texture);
	if (other.mNormalTexture)
		App->GetResource()->RequestResource(other.mNormalTexture->GetUID(), Resource::Type::Texture);
	if (other.mEmisiveTexture)
		App->GetResource()->RequestResource(other.mEmisiveTexture->GetUID(), Resource::Type::Texture);
	if (IsEnabled())
	{
		App->GetOpenGL()->AddDecal(*this);
	}
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

void DecalComponent::GetSpriteSheetSize(int& rows, int& columns) const
{
	if (mIsSpriteSheet)
	{
		rows = mRows;
		columns = mColumns;
	}
	else
	{
		rows = 1;
		columns = 1;
	}
	
}

void DecalComponent::GetSpriteSheetCurrentPosition(int& row, int& column) const
{

	if (mIsSpriteSheet)
	{
		row = mCurrentRow;
		column = mCurrentColumn;
	}
	else
	{
		row = 0;
		column = 0;
	}

}



void DecalComponent::Stop()
{
	mIsPlaying = false;
	mElapsedTime = 0;

	mCurrentRow = mDefaultRow;
	mCurrentColumn = mDefaultColumn;
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
	if (mIsSpriteSheet && mIsPlaying)
	{
		mElapsedTime += App->GetDt();
		float frameDuration = 1.0f / mFPS;

		if (mElapsedTime > frameDuration)
		{
			mCurrentColumn = (mCurrentColumn + 1) % mColumns;
			if (mCurrentColumn == 0)
			{
				mCurrentRow = (mCurrentRow + 1) % mRows;
			}
			
			mElapsedTime = 0;
		}
	}
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


	obj.AddBool("IsSpriteSheet", mIsSpriteSheet);

	int size[2] = { mRows , mColumns };
	int defaultPosition[2] = {mDefaultRow, mDefaultColumn};
	obj.AddInts("SpriteSheetSize", size, 2);
	obj.AddInts("DefaultPosition", defaultPosition, 2);
	obj.AddInt("Speed", mFPS);
	obj.AddFloat("Fade", mFade);
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
	
	mIsSpriteSheet = data.GetBool("IsSpriteSheet");

	int size[2];
	data.GetInts("SpriteSheetSize", size);
	mRows = size[0];
	mColumns = size[1];

	int defaultPosition[2];
	data.GetInts("DefaultPosition", defaultPosition);
	mDefaultRow = defaultPosition[0];
	mDefaultColumn = defaultPosition[1];
	
	mFPS = data.GetInt("Speed");
	mFade = data.GetFloat("Fade");
}

void DecalComponent::Enable()
{
	App->GetOpenGL()->AddDecal(*this);
}

void DecalComponent::Disable()
{
	App->GetOpenGL()->RemoveDecal(*this);
}
