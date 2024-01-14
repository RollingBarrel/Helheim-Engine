#include "MeshRendererComponent.h"

MeshRendererComponent::MeshRendererComponent(GameObject* owner) 
	:Component("Mesh Renderer" , owner, ComponentType::MESHRENDERER)
{

}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(original.mName , owner, ComponentType::MESHRENDERER)
{

}

void MeshRendererComponent::Draw()
{

}
void MeshRendererComponent::Reset()
{

}
void MeshRendererComponent::Load()
{
	LoadVBO();
}

void MeshRendererComponent::Update()
{
	Draw();
}


Component* MeshRendererComponent::Clone(GameObject* owner)
{
	return new MeshRendererComponent(*this, owner);
}

void MeshRendererComponent::LoadVBO()
{

}

void MeshRendererComponent::LoadEBO()
{
}

void MeshRendererComponent::LoadVAO()
{
}


void MeshRendererComponent::Save(Archive& archive) const {
	archive.AddString("type", "MeshRenderer");
}

void MeshRendererComponent::Load(const rapidjson::Value& data) {
	// TODO implement by File System Team
}

