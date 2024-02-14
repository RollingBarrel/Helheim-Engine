#include "PointLightComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleDebugDraw.h"

PointLightComponent::PointLightComponent(GameObject* owner, const PointLight& light) : Component(owner, ComponentType::POINTLIGHT), mData(light) {
	const float3& pos = owner->GetWorldPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
}

PointLightComponent::~PointLightComponent() { App->GetOpenGL()->RemovePointLight(*this); }

const float* PointLightComponent::GetPosition() const 
{ 
	return mOwner->GetWorldPosition().ptr(); 
}

void PointLightComponent::SetPosition(const float pos[3])
{
	mData.pos[0] = pos[0];
	mData.pos[1] = pos[1];
	mData.pos[2] = pos[2];
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void PointLightComponent::SetColor(float col[3])
{
	mData.col[0] = col[0];
	mData.col[1] = col[1];
	mData.col[2] = col[2];
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void PointLightComponent::SetIntensity(float intensity)
{
	mData.col[3] = intensity;
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void PointLightComponent::SetRadius(float radius)
{
	mData.pos[3] = radius;
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void PointLightComponent::Update()
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
		App->GetDebugDraw()->DrawSphere(mData.pos, mData.col, mData.pos[3]);
	}
}

void PointLightComponent::Save(Archive& archive) const {
	//TODO: Do we need id???
	//archive.AddInt("ID", mID);
	archive.AddFloat4("Position", mData.pos);
	archive.AddFloat4("Color", mData.col);
}

//TODO: why is the GO owner passed here??
void PointLightComponent::LoadFromJSON(const rapidjson::Value& componentJson, GameObject* owner) {
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
	if (componentJson.HasMember("Color") && componentJson["Color"].IsArray())
	{
		const auto& posArray = componentJson["Color"].GetArray();
		for (int i = 0; i < posArray.Size(); ++i)
		{
			mData.col[i] = posArray[i].GetFloat();
		}
	}
}