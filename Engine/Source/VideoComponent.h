#pragma once
#include "Component.h"


class VideoComponent : public Component
{
public:
	explicit VideoComponent(GameObject* owner);
	VideoComponent(const VideoComponent& original) = delete;
	VideoComponent(const VideoComponent& original, GameObject* owner);
	~VideoComponent();


	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

protected:

	void Reset() override;

private:
	void Enable() override;
	void Disable() override;
};

