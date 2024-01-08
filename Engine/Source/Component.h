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
	
	virtual	void DrawEditor() = 0;
	virtual Component* Clone(GameObject* owner) = 0;

	Component(const char* name ,GameObject* owner, ComponentType type);
	virtual ~Component() {}
	

	const ComponentType GetType() const { return mType; }
	GameObject* GetOwner() const { return mOwner; }

protected:
	bool IsComponentOpen();
	virtual	void Reset() = 0;
	virtual void RightClickPopup();
	const char* const mName;
	const char* mPopupID;
	static int lastcomponentIndex;
	GameObject* mOwner;
private:
	ComponentType mType;
	
	bool mIsEnabled;
};

