#include "ImageComponent.h"
#include "glew.h"

ImageComponent::ImageComponent(bool active, GameObject* owner) : Component(owner, ComponentType::IMAGE) {
}

ImageComponent:: ~ImageComponent() {
}

void ImageComponent::Draw() const
{
	
}

Component* ImageComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void ImageComponent::Save(Archive& archive) const
{
}

void ImageComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
