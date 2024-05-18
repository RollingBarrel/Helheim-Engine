#include "SpotLightComponent.h"
#include "SpotLightComponent.h"
#include "SpotLightComponent.h"
#include "SpotLightComponent.h"
#include "ModuleOpenGL.h"
#include "Application.h"
#include "SpotLightComponent.h"
#include "glew.h"

SpotLightComponent::SpotLightComponent(GameObject* owner, const SpotLight& light) : Component(owner, ComponentType::SPOTLIGHT), mData(light) 
{
	const float3& pos = owner->GetWorldPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
	mData.range = 15.0f;
	mData.bias = 0.00001f;
	App->GetOpenGL()->AddSpotLight(*this);

	mShadowFrustum.type = FrustumType::PerspectiveFrustum;
	mShadowFrustum.pos = owner->GetWorldPosition();
	mShadowFrustum.front = owner->GetFront();
	mShadowFrustum.up = owner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.1f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * acos(mData.col[3]);
	mShadowFrustum.verticalFov =  2.0f * acos(mData.col[3]);

	mShadowMapSize = 512; // 512

	glGenTextures(1, &mShadowMapId);
	glBindTexture(GL_TEXTURE_2D, mShadowMapId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mShadowMapSize, mShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glGenerateMipmap(GL_TEXTURE_2D);
	mData.shadowMapHandle = glGetTextureHandleARB(mShadowMapId);
	glMakeTextureHandleResidentARB(mData.shadowMapHandle);

}

SpotLightComponent::~SpotLightComponent() 
{ 
	App->GetOpenGL()->RemoveSpotLight(*this);
	glMakeTextureHandleNonResidentARB(mData.shadowMapHandle);
	glDeleteTextures(1, &mShadowMapId);
}

const float* SpotLightComponent::GetPosition() const 
{ 
	return mOwner->GetWorldPosition().ptr(); 
}

void SpotLightComponent::SetColor(float col[3])
{
	mData.col[0] = col[0];
	mData.col[1] = col[1];
	mData.col[2] = col[2];
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetIntensity(float intensity)
{
	mData.pos[3] = intensity;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetRange(float range)
{
	mData.range = range;
	mShadowFrustum.farPlaneDistance = range;
	mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

float SpotLightComponent::GetOuterAngle() const {
	return acos(mData.col[3]);
}

void SpotLightComponent::SetOuterAngle(float angle)
{
	mData.col[3] = cos(angle);
	mShadowFrustum.horizontalFov = 2.0f * angle;
	mShadowFrustum.verticalFov = 2.0f * angle;
	mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

float SpotLightComponent::GetInnerAngle() const {
	return acos(mData.aimD[3]);
}

void SpotLightComponent::SetInnerAngle(float angle)
{
	mData.aimD[3] = cos(angle);
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetBias(float bias) 
{ 
	mData.bias = bias;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetShadowMapSize(unsigned int shadowMapSize) 
{ 
	mShadowMapSize = shadowMapSize;
	glMakeTextureHandleNonResidentARB(mData.shadowMapHandle);
	glDeleteTextures(1, &mShadowMapId);
	glGenTextures(1, &mShadowMapId);
	glBindTexture(GL_TEXTURE_2D, mShadowMapId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mShadowMapSize, mShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	mData.shadowMapHandle = glGetTextureHandleARB(mShadowMapId);
	glMakeTextureHandleResidentARB(mData.shadowMapHandle);
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}


void SpotLightComponent::Update()
{
	if (mOwner->HasUpdatedTransform())
	{
		float3 newPosition = mOwner->GetWorldPosition();
		mData.pos[0] = newPosition[0];
		mData.pos[1] = newPosition[1];
		mData.pos[2] = newPosition[2];

		float3 newDirection = mOwner->GetFront();
		mData.aimD[0] = newDirection[0];
		mData.aimD[1] = newDirection[1];
		mData.aimD[2] = newDirection[2];
		

		mShadowFrustum.pos = newPosition;
		mShadowFrustum.front = newDirection;
		mShadowFrustum.up = mOwner->GetUp();

		mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
		App->GetOpenGL()->UpdateSpotLightInfo(*this);
	}
}

inline Component* SpotLightComponent::Clone(GameObject* owner) const 
{ 
	return new SpotLightComponent(owner, mData);
}

void SpotLightComponent::Save(Archive& archive) const 
{
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddFloat4("Position", mData.pos);
	archive.AddFloat4("Direction", mData.aimD);
	archive.AddFloat4("Color", mData.col);
	archive.AddFloat("Range", mData.range);
	archive.AddBool("CastShadow", mCastShadow);
	archive.AddFloat("Bias", mData.bias);
	archive.AddInt("ShadowMapSize", mShadowMapSize);
}

//TODO: why is the GO owner passed here??
void SpotLightComponent::LoadFromJSON(const rapidjson::Value& componentJson, GameObject* owner) 
{

	if (componentJson.HasMember("Position") && componentJson["Position"].IsArray())
	{
		const auto& posArray = componentJson["Position"].GetArray();
		for (unsigned int i = 0; i < posArray.Size(); ++i)
		{
			mData.pos[i] = posArray[i].GetFloat();
		}
	}

	if (componentJson.HasMember("Direction") && componentJson["Direction"].IsArray())
	{
		const auto& posArray = componentJson["Direction"].GetArray();
		for (unsigned int i = 0; i < posArray.Size(); ++i)
		{
			mData.aimD[i] = posArray[i].GetFloat();
		}
	}

	if (componentJson.HasMember("Color") && componentJson["Color"].IsArray())
	{
		const auto& posArray = componentJson["Color"].GetArray();
		for (unsigned int i = 0; i < posArray.Size(); ++i)
		{
			mData.col[i] = posArray[i].GetFloat();
		}
	}

	if (componentJson.HasMember("Range") && componentJson["Range"].IsFloat())
	{
		mData.range = componentJson["Range"].GetFloat();
	}

	if (componentJson.HasMember("CastShadow") && componentJson["CastShadow"].IsBool())
	{
		mCastShadow = componentJson["CastShadow"].GetBool();
	}

	if (componentJson.HasMember("Bias") && componentJson["Bias"].IsFloat())
	{
		mData.bias = componentJson["Bias"].GetFloat();
	}

	if (componentJson.HasMember("ShadowMapSize") && componentJson["ShadowMapSize"].IsInt())
	{
		SetShadowMapSize(componentJson["ShadowMapSize"].GetInt());
	}

	mShadowFrustum.pos = owner->GetWorldPosition();
	mShadowFrustum.front = owner->GetFront();
	mShadowFrustum.up = owner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.01f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * mData.col[3];
	mShadowFrustum.verticalFov = 2.0f * mData.col[3];
	mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::Enable()
{
	App->GetOpenGL()->AddSpotLight(*this);
}

void SpotLightComponent::Disable()
{
	App->GetOpenGL()->RemoveSpotLight(*this);
}