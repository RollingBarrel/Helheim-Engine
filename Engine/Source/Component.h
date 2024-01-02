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
	virtual	void Update() = 0;
	virtual	void DrawEditor() = 0; //Do not call ImGui::Begin() inside this function.
	virtual Component* Clone() = 0;

	virtual ~Component() {}

	// Create a new component linked to this GameObject
	static Component* CreateComponent(ComponentType type, GameObject* owner);

	// Delete a component linked to this GameObject
	static void DeleteComponent(Component* component);

	// Get the type of the component
	ComponentType GetType() const { return mType; }


private:
	ComponentType mType;
	GameObject* Owner;
	bool mIsEnabled;

};

