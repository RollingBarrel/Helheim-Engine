#pragma once

enum class ComponentType
{
	MESHRENDERER, TEST
};

class GameObject;

class Component
{
public:
	virtual	void Enable() { mIsEnabled = true; };
	virtual	void Disable() { mIsEnabled = false; }
	virtual	void Update() = 0;
	virtual	void Reset() = 0;
	virtual	void DrawEditor() = 0;
	virtual Component* Clone() = 0;

	Component(GameObject* owner, ComponentType type);
	virtual ~Component() {}

	const ComponentType GetType() const { return mType; }
	GameObject* GetOwner() const { return mOwner; }

private:
	ComponentType mType;
	GameObject* mOwner;
	bool mIsEnabled;
};

