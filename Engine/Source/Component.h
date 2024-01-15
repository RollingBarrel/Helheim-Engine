#pragma once

enum class ComponentType : unsigned int
{
	MESHRENDERER, TEST, NONE
};

class GameObject;

class Component
{
	friend class InspectorPanel;
public:
	virtual	void Enable() { mIsEnabled = true; };
	virtual	void Disable() { mIsEnabled = false; }
	virtual	void Update() = 0;
	
	virtual Component* Clone(GameObject* owner) = 0;

	Component(const char* name ,GameObject* owner, ComponentType type);
	virtual ~Component() {}
	
	const ComponentType GetType() const { return mType; }
	GameObject* GetOwner() const { return mOwner; }

	int GetID() { return mID; }

protected:
	virtual	void Reset() = 0;
	const char* const mName = "";
	const unsigned int mID = 0;
	GameObject* mOwner = nullptr;
private:
	ComponentType mType = ComponentType::NONE;
	bool mIsEnabled = true;
};

