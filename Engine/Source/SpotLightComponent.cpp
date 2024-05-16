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
	mData.range = 10.0f;
	App->GetOpenGL()->AddSpotLight(*this);

	mShadowFrustum.type = FrustumType::PerspectiveFrustum;
	mShadowFrustum.pos = owner->GetWorldPosition();
	mShadowFrustum.front = owner->GetFront();
	mShadowFrustum.up = owner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.01f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * mData.col[3];
	mShadowFrustum.verticalFov =  2.0f * mData.col[3];

	glGenTextures(1, &mShadowMapId);
	glBindTexture(GL_TEXTURE_2D, mShadowMapId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glGenerateMipmap(GL_TEXTURE_2D);
	if (mData.shadowMapHandle != 0)
	{
		glMakeTextureHandleNonResidentARB(mData.shadowMapHandle);
	}
	mData.shadowMapHandle = glGetTextureHandleARB(mShadowMapId);
	glMakeTextureHandleResidentARB(mData.shadowMapHandle);

}

SpotLightComponent::~SpotLightComponent() { App->GetOpenGL()->RemoveSpotLight(*this); }

const float* SpotLightComponent::GetPosition() const 
{ 
	return mOwner->GetWorldPosition().ptr(); 
}

void SpotLightComponent::SetPosition(const float pos[3])
{
	mData.pos[0] = pos[0];
	mData.pos[1] = pos[1];
	mData.pos[2] = pos[2];
	mShadowFrustum.pos = float3(pos);
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
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

void SpotLightComponent::MakeShadowMapBindless(unsigned int shadowMapTextureId)
{
	
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
	LOG("offestof: %i", offsetof(SpotLight, shadowMapHandle));
}

inline Component* SpotLightComponent::Clone(GameObject* owner) const 
{ 
	return new SpotLightComponent(owner, mData);
}

void SpotLightComponent::Save(Archive& archive) const 
{
	//TODO: Do we need id???
	//archive.AddInt("ID", mID);
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddFloat4("Position", mData.pos);
	archive.AddFloat4("Direction", mData.aimD);
	archive.AddFloat4("Color", mData.col);
	archive.AddFloat("Range", mData.range);
}

//TODO: why is the GO owner passed here??
void SpotLightComponent::LoadFromJSON(const rapidjson::Value& componentJson, GameObject* owner) 
{
	//int id = 0;
	//if (componentJson.HasMember("ID") && componentJson["ID"].IsInt()) {
	//	id = componentJson["ID"].GetInt();
	//}
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
}

void SpotLightComponent::Enable()
{
	App->GetOpenGL()->AddSpotLight(*this);
}

void SpotLightComponent::Disable()
{
	App->GetOpenGL()->RemoveSpotLight(*this);
}