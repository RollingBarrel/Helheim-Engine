#pragma once
#include "Globals.h"
#include <vector>
#include <string>
#include "Physics.h"
#include "Debug.h"

class GameObject;

#define CLASS(classname) \
	using ClassType = classname;

#define MEMBER(type, member) \
	mMembers.push_back(new Member(#member, type, offsetof(ClassType, member)))

enum class  MemberType : int
{
	// IF THE NUMBERS CHANGES THE SCRIPTS MAY BREAK
	NONE = -1,
	INT = 0, 
	FLOAT = 1, 
	BOOL = 2, 
	GAMEOBJECT = 3, 
	FLOAT3 = 4
	
};

struct ScriptVariable {

	const char* mName = nullptr;
	MemberType mType = MemberType::NONE;
	void* mData = nullptr;

	ScriptVariable() {}
	ScriptVariable(const char* name, MemberType type, void* data) : mName(name), mType(type), mData(data) {}
};


struct Member {
	const char* mName = nullptr;
	MemberType mType = MemberType::NONE;
	size_t mOffset = 0;
	Member(const char* name, MemberType type, size_t offset) : mName(name), mType(type), mOffset(offset) {}
};

class ENGINE_API Script
{
	friend class InspectorPanel;
	friend class ScriptComponent;

public:

	Script(GameObject* owner);
	virtual ~Script() {}
	void SetName(const std::string& name) { mName = name; }
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Serialize() = 0;
	//virtual void OnButtonClick() = 0;

protected:

	GameObject* mGameObject = nullptr;
	std::vector<Member*> mMembers;

private:

	std::string mName;
	
};

