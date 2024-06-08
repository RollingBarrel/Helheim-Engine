#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleScriptManager.h"
#include <Windows.h>
#include "Script.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"


ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner, ComponentType::SCRIPT)
{
}

ScriptComponent::ScriptComponent(const ScriptComponent& other, GameObject* owner) : Component(owner, ComponentType::SCRIPT)
{
	mName = other.mName;
	mResourceScript = other.mResourceScript;
	LoadScript(mName.c_str());

	if(mScript)
	{
		for (unsigned int i = 0; i < mScript->mMembers.size(); ++i)
		{
			char* originalMemberPos = (reinterpret_cast<char*>(other.mScript)) + mScript->mMembers[i]->mOffset;
			char* newMemberPos = (reinterpret_cast<char*>(mScript)) + mScript->mMembers[i]->mOffset;

			switch (mScript->mMembers[i]->mType)
			{
			case MemberType::INT:
				memcpy(newMemberPos, originalMemberPos, sizeof(int));
				break;
			case MemberType::FLOAT:
				memcpy(newMemberPos, originalMemberPos, sizeof(float));
				break;
			case MemberType::BOOL:
				memcpy(newMemberPos, originalMemberPos, sizeof(bool));
				break;
			case MemberType::FLOAT3:
				memcpy(newMemberPos, originalMemberPos, sizeof(float) * 3);
				break;
			case MemberType::GAMEOBJECT:
				memcpy(newMemberPos, originalMemberPos, sizeof(GameObject*));
				break;
			}
		}
	}
}

ScriptComponent::~ScriptComponent()
{
	App->GetScriptManager()->RemoveScript(this);
	if (mResourceScript)
	{
		App->GetResource()->ReleaseResource(mResourceScript->GetUID());
		mResourceScript = nullptr;
	}

	delete mScript;
}

void ScriptComponent::Update()
{
	//std::vector<Component*> components = App->GetScene()->GetRoot()->FindComponentsInChildren(App->GetScene()->GetRoot(), ComponentType::SCRIPT);
}

Component* ScriptComponent::Clone(GameObject* owner) const
{
	ScriptComponent* scriptComponent = new ScriptComponent(*this, owner);
	if (mScript)
	{
		scriptComponent->mScript->Awake();
	}
	
	return scriptComponent;
}

void ScriptComponent::Reset()
{
}

void::ScriptComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);

	obj.AddString("ScriptName", mName.c_str());

	JsonArray arr = obj.AddNewJsonArray("ScriptVariables");
	const std::vector<Member*>& members = mScript->GetMembers();
	for (const Member* member : members) 
	{
		JsonObject membObj = arr.PushBackNewObject();	
		membObj.AddString("VariableName", member->mName);
		membObj.AddInt("MemberType", static_cast<int>(member->mType));
		switch (member->mType)
		{
		case MemberType::INT:
			membObj.AddInt("VariableData", *reinterpret_cast<int*>((((char*)mScript) + member->mOffset)));
			break;
		case MemberType::FLOAT:
			membObj.AddFloat("VariableData", *reinterpret_cast<float*>((((char*)mScript) + member->mOffset)));
			break;
		case MemberType::BOOL:
			membObj.AddBool("VariableData", *reinterpret_cast<bool*>((((char*)mScript) + member->mOffset)));
			break;
		case MemberType::FLOAT3:
			membObj.AddFloats("VariableData", reinterpret_cast<float3*>((((char*)mScript) + member->mOffset))->ptr(), 3);
			break;
		case MemberType::GAMEOBJECT:
		{
			GameObject* gameObject = *reinterpret_cast<GameObject**>((((char*)mScript) + member->mOffset));
			gameObject ? membObj.AddInt("VariableData", (gameObject->GetID())) : membObj.AddInt("VariableData", -1);
			break;
		}
		default:
			break;
		}
	}
}

void::ScriptComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	mName = data.GetString("ScriptName");
	if (!mName.empty())
	{
		LoadScript(mName.c_str());
	}

	JsonArray scriptData = data.GetJsonArray("ScriptVariables");
	if (mScript && (scriptData.Size() > 0))
	{
		for (unsigned int i = 0; i < scriptData.Size(); ++i)
		{
			JsonObject data = scriptData.GetJsonObject(i);
			const char* name = data.GetString("VariableName").c_str();
			const std::vector<Member*> members = mScript->GetMembers();
			for (const Member* member : members)
			{
				if (strcmp(member->mName, name) == 0)
				{
					switch ((MemberType)data.GetInt("MemberType"))
					{
					case MemberType::INT:
						*reinterpret_cast<int*>((((char*)mScript) + member->mOffset)) = data.GetInt("VariableData");
						break;
					case MemberType::FLOAT:
						*reinterpret_cast<float*>((((char*)mScript) + member->mOffset)) = data.GetFloat("VariableData");
						break;
					case MemberType::BOOL:
						*reinterpret_cast<bool*>((((char*)mScript) + member->mOffset)) = data.GetBool("VariableData");
						break;
					case MemberType::FLOAT3:
					{
						float tmp[3];
						data.GetFloats("VariableData", tmp);
						*reinterpret_cast<float3*>((((char*)mScript) + member->mOffset)) = float3(tmp);
						break;
					}
					case MemberType::GAMEOBJECT:
					{
						int  UID = data.GetInt("VariableData");
						if (UID != -1) 
						{
							App->GetScene()->AddGameObjectToLoadIntoScripts(std::pair<unsigned int, GameObject**>(UID, reinterpret_cast<GameObject**>((((char*)mScript) + member->mOffset))));
						}
						break;
					}
					default:
						break;
					}
				}
			}
		}
	}
}

void ScriptComponent::LoadScript(const char* scriptName)
{
	std::string scriptPath = ASSETS_SCRIPT_PATH + std::string(scriptName) + ".h";
	
	if (!App->GetFileSystem()->Exists(scriptPath.c_str()))
	{
		LOG("SCRIPT ASSET NOT FOUND");
	}
	if (mResourceScript != nullptr) 
	{
		App->GetResource()->ReleaseResource(mResourceScript->GetUID());
		mResourceScript = nullptr;
	}

	if (mScript) 
	{
		//delete mScript; //MEMORY LEAK: Crash do not know why
		mScript = nullptr;
	}
	
	

	mName = scriptName;
	Script* (*script)(GameObject*) = (Script * (*)(GameObject*))GetProcAddress(static_cast<HMODULE>(App->GetScriptManager()->GetDLLHandle()), (std::string("Create") + std::string(mName)).c_str());
	if (script != nullptr) 
	{	
		mScript = script(mOwner);
		mScript->SetName(mName);
		mResourceScript = (ResourceScript*)(App->GetResource()->RequestResource(scriptPath.c_str()));
		if (mResourceScript == nullptr) 
		{
			LOG("SCRIPT RESOURCE NOT FOUND");
		}
		App->GetScriptManager()->AddScript(this);
		LOG("LOADING SCRIPT SUCCESS");
	}
	else 
	{
		LOG("LOADING SCRIPT ERROR");
	}
}

void ScriptComponent::Enable()
{
	App->GetScriptManager()->AddScript(this);
	
	if (mHasStarted)
	{
		mScript->Start();
	}
}

void ScriptComponent::Disable()
{
	App->GetScriptManager()->RemoveScript(this);
}
