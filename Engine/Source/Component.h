#pragma once
#include "Archive.h"

enum class ComponentType : unsigned int
{

	MESHRENDERER, POINTLIGHT, SPOTLIGHT,NAVMESHCONTROLLER, CANVAS, TRANSFORM2D ,TEST, NONE
};

class GameObject;

class Component
{
	friend class InspectorPanel;
public:
	Component(GameObject* owner, ComponentType type);
	virtual ~Component() {}

	virtual	void Update() = 0;
	
	virtual Component* Clone(GameObject* owner) const = 0;
	virtual void Save(Archive& archive) const = 0;
	virtual void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) = 0;

	ComponentType GetType() const { return mType; }
	const GameObject* const GetOwner() const { return mOwner; }

	int GetID() { return mID; }

	virtual	void Enable() { mIsEnabled = true; };
	virtual	void Disable() { mIsEnabled = false; }
	bool IsEnabled() const { return mIsEnabled; }

	const char* GetNameFromType() const;

protected:
	virtual	void Reset() = 0;
	const unsigned int mID = 0;
	GameObject* mOwner = nullptr;
	bool mIsEnabled = true;
private:
	ComponentType mType = ComponentType::NONE;
};

