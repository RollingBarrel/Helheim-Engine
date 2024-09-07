#pragma once
#include "Globals.h"
#include "Archive.h"
#include <unordered_map>

enum class ENGINE_API ComponentType : unsigned int
{
	// Do not modify existing assignations:
	// this could lead to an error on loading existing components in scenes
	MESHRENDERER = 0, POINTLIGHT = 1, SPOTLIGHT = 2, SCRIPT = 3, NAVMESHOBSTACLE = 4, AIAGENT = 5,
	CAMERA = 6, CANVAS = 7, IMAGE = 8, TRANSFORM2D = 9, ANIMATION = 10, BUTTON = 11, SLIDER = 12,
	AUDIOSOURCE = 13, AUDIOLISTENER = 14, PARTICLESYSTEM = 15, TEXT = 16,
	BOXCOLLIDER = 17, TRAIL = 18, DECAL = 19, MASK = 20, LINE = 21, VIDEO = 22,

	NONE // None has to be allways the last
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

