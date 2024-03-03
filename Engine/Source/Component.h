#pragma once
#include "Archive.h"

enum class ENGINE_API ComponentType : unsigned int
{
	MESHRENDERER, POINTLIGHT, SPOTLIGHT,NAVMESHCONTROLLER, CAMERA, CANVAS, SCRIPT, TRANSFORM2D ,TEST, NONE
};

class GameObject;

class ENGINE_API Component
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

	int GetID() const { return mID; }

	virtual	void Enable() { mIsEnabled = true; };
	virtual	void Disable() { mIsEnabled = false; }
	bool IsEnabled() const { return mIsEnabled; }

	static const char* GetNameFromType(ComponentType type);

protected:
	virtual	void Reset() = 0;
	const unsigned int mID = 0;
	GameObject* mOwner = nullptr;
	bool mIsEnabled = true;
private:
	ComponentType mType = ComponentType::NONE;
};

