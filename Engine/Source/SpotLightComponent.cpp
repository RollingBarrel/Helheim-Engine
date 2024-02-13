#include "SpotLightComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleDebugDraw.h"
#include "MathFunc.h"

SpotLightComponent::SpotLightComponent(GameObject* owner, const SpotLight& light) : Component(owner, ComponentType::SPOTLIGHT), mData(light) {
	const float3& pos = owner->GetWorldPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
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
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetDirection(const float dir[3])
{
	mData.aimD[0] = dir[0];
	mData.aimD[1] = dir[1];
	mData.aimD[2] = dir[2];
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

void SpotLightComponent::SetRadius(float radius)
{
	mData.radius = radius;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

float SpotLightComponent::GetOuterAngle() const {
	return acos(mData.col[3]);
}

void SpotLightComponent::SetOuterAngle(float angle)
{
	mData.col[3] = cos(angle);
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

void SpotLightComponent::Update()
{
	//TODO: No mirarlo cada frame ??
	const float* pos = mOwner->GetWorldPosition().ptr();
	for (int i = 0; i < 3; ++i)
	{
		if (pos[i] != mData.pos[i])
		{
			SetPosition(pos);
		}
	}
	if (debugDraw)
	{
		//App->GetDebugDraw()->DrawLineSphere(mData.pos, mData.col, mData.pos[3]);
	}
}

void SpotLightComponent::Save(Archive& archive) const {
	//TODO: Do we need id???
	//archive.AddInt("ID", mID);
	archive.AddFloat4("Position", mData.pos);
	archive.AddFloat4("Direction", mData.aimD);
	archive.AddFloat4("Color", mData.col);
	archive.AddFloat("Radius", mData.radius);
}

//TODO: why is the GO owner passed here??
void SpotLightComponent::LoadFromJSON(const rapidjson::Value& componentJson, GameObject* owner) {
	//int id = 0;
	//if (componentJson.HasMember("ID") && componentJson["ID"].IsInt()) {
	//	id = componentJson["ID"].GetInt();
	//}
	if (componentJson.HasMember("Position") && componentJson["Position"].IsArray())
	{
		const auto& posArray = componentJson["Position"].GetArray();
		for (int i = 0; i < posArray.Size(); ++i)
		{
			mData.pos[i] = posArray[i].GetFloat();
		}
	}
	if (componentJson.HasMember("Direction") && componentJson["Direction"].IsArray())
	{
		const auto& posArray = componentJson["Direction"].GetArray();
		for (int i = 0; i < posArray.Size(); ++i)
		{
			mData.aimD[i] = posArray[i].GetFloat();
		}
	}
	if (componentJson.HasMember("Color") && componentJson["Color"].IsArray())
	{
		const auto& posArray = componentJson["Color"].GetArray();
		for (int i = 0; i < posArray.Size(); ++i)
		{
			mData.col[i] = posArray[i].GetFloat();
		}
	}
	if (componentJson.HasMember("Radius") && componentJson["Radius"].IsArray())
	{
		mData.radius = componentJson["Radius"].GetFloat();
	}
}