#include "Application.h"
#include "ModuleOpenGL.h"
//#include "ModuleDebugDraw.h"

#include "PointLightComponent.h"

PointLightComponent::PointLightComponent(GameObject* owner, const PointLight& light) : Component(owner, ComponentType::POINTLIGHT), mData(light) 
{
	const float3& pos = owner->GetPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
	App->GetOpenGL()->AddPointLight(*this);
}

PointLightComponent::~PointLightComponent() { App->GetOpenGL()->RemovePointLight(*this); }

const float* PointLightComponent::GetPosition() const 
{ 
	return mOwner->GetPosition().ptr(); 
}

void PointLightComponent::SetPosition(const float pos[3])
{
	mData.pos[0] = pos[0];
	mData.pos[1] = pos[1];
	mData.pos[2] = pos[2];
	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::SetColor(float col[3])
{
	mData.col[0] = col[0];
	mData.col[1] = col[1];
	mData.col[2] = col[2];
	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::SetIntensity(float intensity)
{
	mData.intensity = intensity;
	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::SetRadius(float radius)
{
	mData.radius = radius;
	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::Update()
{
	//TODO: No mirarlo cada frame ??
	const float* pos = mOwner->GetPosition().ptr();
	for (int i = 0; i < 3; ++i)
	{
		if (pos[i] != mData.pos[i])
		{
			SetPosition(pos);
		}
	}
	//TODO: SEPARATE ENGINE
	//if (debugDraw)
	//{
	//	App->GetDebugDraw()->DrawSphere(mData.pos, mData.col, mData.pos[3]);
	//}
}

inline Component* PointLightComponent::Clone(GameObject* owner) const 
{
	return new PointLightComponent(owner, mData);
}

void PointLightComponent::Save(Archive& archive) const {
	//TODO: Do we need id???
	//archive.AddInt("ID", mID);
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddFloat4("Position", mData.pos);
	archive.AddFloat4("Color", mData.col);

}

//TODO: why is the GO owner passed here??
void PointLightComponent::LoadFromJSON(const rapidjson::Value& componentJson, GameObject* owner) 
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
	if (componentJson.HasMember("Color") && componentJson["Color"].IsArray())
	{
		const auto& posArray = componentJson["Color"].GetArray();
		for (unsigned int i = 0; i < posArray.Size(); ++i)
		{
			mData.col[i] = posArray[i].GetFloat();
		}
	}
}

void PointLightComponent::Enable()
{
	App->GetOpenGL()->AddPointLight(*this);
}

void PointLightComponent::Disable()
{
	App->GetOpenGL()->RemovePointLight(*this);
}