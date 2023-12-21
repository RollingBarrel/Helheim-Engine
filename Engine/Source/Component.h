#pragma once

enum class ComponentType
{
	MESHRENDERER,
};

class GameObject;

class Component
{
public:
	void Enable();
	void Disable();
	void Update();
	void DrawEditor();

private:
	ComponentType mType;
	GameObject* Owner;

};

