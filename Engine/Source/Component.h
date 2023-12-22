#pragma once

enum class ComponentType
{
	MESHRENDERER,
};

class GameObject;

class Component
{
public:
	virtual	void Enable() { mIsEnabled = true; };
	virtual	void Disable() { mIsEnabled = false; }
	virtual	void Update() {};
	virtual	void DrawEditor() {}; //Do not call ImGui::Begin() inside this function.

private:
	ComponentType mType;
	GameObject* Owner;
	bool mIsEnabled;

};

