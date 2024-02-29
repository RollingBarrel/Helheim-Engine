#pragma once
#include "Component.h"

class ComponentCanvas;

class Transform2DComponent :
    public Component
{
public:
	Transform2DComponent(const bool active, GameObject* owner);
	~Transform2DComponent();

	void Update() override;
};

