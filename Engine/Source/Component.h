#pragma once
#include "Globals.h"
#include "Archive.h"
#include <unordered_map>

enum class ENGINE_API ComponentType : unsigned int
{
	MESHRENDERER, POINTLIGHT, SPOTLIGHT, SCRIPT, NAVMESHOBSTACLE, AIAGENT, 
	CAMERA, CANVAS, IMAGE, TRANSFORM2D, ANIMATION, BUTTON, SLIDER,
	AUDIOSOURCE, AUDIOLISTENER, PARTICLESYSTEM, TEXT,
	BOXCOLLIDER, TRAIL, DECAL, MASK, NONE
};

class GameObject;

class ENGINE_API Component
{
	friend class InspectorPanel;
public:
	Component(GameObject* owner, ComponentType type);
	virtual ~Component() {}

	virtual	void Update() {}
	
	virtual Component* Clone(GameObject* owner) const = 0;

	virtual void Save(JsonObject& obj) const;
	virtual void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap);

	ComponentType GetType() const { return mType; }
	GameObject* GetOwner() const { return mOwner; }

	unsigned int GetID() const { return mID; }

	bool IsEnabled() const { return mIsEnabled; }

	static const char* GetNameFromType(ComponentType type);

	void SetEnable(bool enable);

protected:
	virtual	void Reset() {}
	GameObject* mOwner = nullptr;
private:
	bool mIsEnabled = true;
	unsigned int mID = 0;
	ComponentType mType = ComponentType::NONE;
	virtual	void Enable() { }
	virtual	void Disable() { }
};

