#include "VideoComponent.h"

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
}

VideoComponent::VideoComponent(GameObject* owner) : Component(owner, ComponentType::VIDEO)
{
}

VideoComponent::VideoComponent(const VideoComponent& original, GameObject* owner) : Component(owner, ComponentType::VIDEO)
{
}

VideoComponent::~VideoComponent()
{
}

void VideoComponent::Update()
{
}

Component* VideoComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void VideoComponent::Save(JsonObject& obj) const
{
}

void VideoComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
}

void VideoComponent::Reset()
{
}

void VideoComponent::Enable()
{
}

void VideoComponent::Disable()
{
}
