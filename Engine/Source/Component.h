#pragma once
#include "Archive.h"

enum class ENGINE_API ComponentType : unsigned int
{
	MESHRENDERER, POINTLIGHT, SPOTLIGHT, SCRIPT, NAVMESHOBSTACLE, AIAGENT, CAMERA, CANVAS, IMAGE, TRANSFORM2D, ANIMATION, BUTTON, TEST, PARTICLESYSTEM, NONE
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
	virtual void Save(Archive& archive) const;
	virtual void LoadFromJSON(const rapidjson::Value& data, GameObject* owner);

	ComponentType GetType() const { return mType; }
	GameObject* const GetOwner() const { return mOwner; }

	unsigned int GetID() const { return mID; }

	virtual	void Enable() { }
	virtual	void Disable() { }
	bool IsEnabled() const { return mIsEnabled; }

	static const char* GetNameFromType(ComponentType type);

	void SetEnable(bool enable)
	{
		if (enable == mIsEnabled)
			return;
		mIsEnabled = enable;
		if (mIsEnabled)
			Enable();
		else
			Disable();
	}

protected:
	virtual	void Reset() = 0;
	GameObject* mOwner = nullptr;
private:
	bool mIsEnabled = true;
	unsigned int mID = 0;
	ComponentType mType = ComponentType::NONE;
};

