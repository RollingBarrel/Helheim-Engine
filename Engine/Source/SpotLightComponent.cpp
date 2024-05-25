#include "SpotLightComponent.h"
#include "ModuleOpenGL.h"
#include "Application.h"
#include "glew.h"
#include "MathFunc.h"

SpotLightComponent::SpotLightComponent(GameObject* owner) : Component(owner, ComponentType::SPOTLIGHT)
{

	const float3& pos = owner->GetWorldPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
	mData.pos[3] = 50.0f; //intensity

	const float3& dir = owner->GetFront();
	mData.aimD[0] = dir.x;
	mData.aimD[1] = dir.y;
	mData.aimD[2] = dir.z;
	mData.aimD[3] = cos(DegToRad(25.f)); //cos inner angle

	mData.color[0] = 1.0f;
	mData.color[1] = 1.0f;
	mData.color[2] = 1.0f;
	mData.color[3] = cos(DegToRad(38.f)); //cos outer angle

	mData.range = 15.0f;
	//mData.bias = 0.00001f;
	mBias = 0.00001f;

	mShadowFrustum.type = FrustumType::PerspectiveFrustum;
	mShadowFrustum.pos = owner->GetWorldPosition();
	mShadowFrustum.front = owner->GetFront();
	mShadowFrustum.up = owner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.1f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * acos(mData.color[3]);
	mShadowFrustum.verticalFov = 2.0f * acos(mData.color[3]);

	//mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();

	//mShadowMapSize = 512;
	//
	//glGenTextures(1, &mShadowMapId);
	//glBindTexture(GL_TEXTURE_2D, mShadowMapId);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mShadowMapSize, mShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//mData.shadowMapHandle = glGetTextureHandleARB(mShadowMapId);
	//glMakeTextureHandleResidentARB(mData.shadowMapHandle);

	App->GetOpenGL()->AddSpotLight(*this);
}

SpotLightComponent::SpotLightComponent(const SpotLightComponent* original, GameObject* owner)
	: Component(owner, ComponentType::SPOTLIGHT), mData(original->mData), mShadowFrustum(original->mShadowFrustum), mCastShadow(original->mCastShadow)
{
	//glGenTextures(1, &mShadowMapId);
	//glBindTexture(GL_TEXTURE_2D, mShadowMapId);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mShadowMapSize, mShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//mData.shadowMapHandle = glGetTextureHandleARB(mShadowMapId);
	//glMakeTextureHandleResidentARB(mData.shadowMapHandle);

	App->GetOpenGL()->AddSpotLight(*this);
}

SpotLightComponent::~SpotLightComponent()
{
	App->GetOpenGL()->RemoveSpotLight(*this);
	//glMakeTextureHandleNonResidentARB(mData.shadowMapHandle);
	//glDeleteTextures(1, &mShadowMapId);
}

const float* SpotLightComponent::GetPosition() const
{
	return mOwner->GetWorldPosition().ptr();
}

void SpotLightComponent::SetColor(float color[3])
{
	mData.color[0] = color[0];
	mData.color[1] = color[1];
	mData.color[2] = color[2];
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
	//mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

float SpotLightComponent::GetOuterAngle() const {
	return acos(mData.color[3]);
}

void SpotLightComponent::SetOuterAngle(float angle)
{
	mData.color[3] = cos(angle);
	mShadowFrustum.horizontalFov = 2.0f * angle;
	mShadowFrustum.verticalFov = 2.0f * angle;
	//mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
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
	mBias = bias;
	//App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetShadowMapSize(unsigned int shadowMapSize)
{
	//mShadowMapSize = shadowMapSize;
	//glMakeTextureHandleNonResidentARB(mData.shadowMapHandle);
	//glDeleteTextures(1, &mShadowMapId);
	//glGenTextures(1, &mShadowMapId);
	//glBindTexture(GL_TEXTURE_2D, mShadowMapId);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mShadowMapSize, mShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//mData.shadowMapHandle = glGetTextureHandleARB(mShadowMapId);
	//glMakeTextureHandleResidentARB(mData.shadowMapHandle);
	//App->GetOpenGL()->UpdateSpotLightInfo(*this);
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

		//mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
		App->GetOpenGL()->UpdateSpotLightInfo(*this);
	}
}

inline Component* SpotLightComponent::Clone(GameObject* owner) const
{
	return new SpotLightComponent(this, owner);
}

void SpotLightComponent::Save(Archive& archive) const
{
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddFloat4("Position", mData.pos);
	archive.AddFloat4("Direction", mData.aimD);
	archive.AddFloat4("Color", mData.color);
	archive.AddFloat("Range", mData.range);
	archive.AddBool("CastShadow", mCastShadow);
	//archive.AddFloat("Bias", mData.bias);
	//archive.AddInt("ShadowMapSize", mShadowMapSize);
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
			mData.color[i] = posArray[i].GetFloat();
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

	//if (componentJson.HasMember("Bias") && componentJson["Bias"].IsFloat())
	//{
	//	mData.bias = componentJson["Bias"].GetFloat();
	//}
	//
	//if (componentJson.HasMember("ShadowMapSize") && componentJson["ShadowMapSize"].IsInt())
	//{
	//	SetShadowMapSize(componentJson["ShadowMapSize"].GetInt());
	//}

	mShadowFrustum.pos = owner->GetWorldPosition();
	mShadowFrustum.front = owner->GetFront();
	mShadowFrustum.up = owner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.01f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * acos(mData.color[3]);
	mShadowFrustum.verticalFov = 2.0f * acos(mData.color[3]);
	//mData.viewProjMatrix = mShadowFrustum.ViewProjMatrix().Transposed();
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