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

	//delete mScript; //Memory leack here, this shouldbe fixed.
}

void ScriptComponent::Update()
{
}

Component* ScriptComponent::Clone(GameObject* owner) const
{
	return new ScriptComponent(*this, owner);
}

void ScriptComponent::Reset()
{
}

void::ScriptComponent::Save(Archive& archive) const
{
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddString("ScriptName",mName.c_str());
	
	
	if (mScript)
	{
		std::vector<Archive> objectArray;
		std::vector<Member*> members = mScript->GetMembers();

		for (const Member* member : members)
		{

			Archive dataArchive;
			dataArchive.AddString("VariableName", member->mName);
			dataArchive.AddInt("MemberType", (int)member->mType);
			switch (member->mType)
			{
			case MemberType::INT:
				dataArchive.AddInt("VariableData", *reinterpret_cast<int*>((((char*)mScript) + member->mOffset)));
				break;
			case MemberType::FLOAT:
				dataArchive.AddFloat("VariableData", *reinterpret_cast<float*>((((char*)mScript) + member->mOffset)));
				break;
			case MemberType::BOOL:
				dataArchive.AddBool("VariableData", *reinterpret_cast<bool*>((((char*)mScript) + member->mOffset)));
				break;
			case MemberType::FLOAT3:
				dataArchive.AddFloat3("VariableData", *reinterpret_cast<float3*>((((char*)mScript) + member->mOffset)));
				break;
			case MemberType::GAMEOBJECT:
			{
				GameObject* gameObject = *reinterpret_cast<GameObject**>((((char*)mScript) + member->mOffset));
				gameObject ? dataArchive.AddInt("VariableData", (gameObject->GetID())) : dataArchive.AddInt("VariableData", -1);
				break;
			}
			default:
				break;
			}

			objectArray.push_back(dataArchive);
		}

		archive.AddObjectArray("ScriptVariables", objectArray);
	}
	
}

void::ScriptComponent::LoadFromJSON(const rapidjson::Value & data, GameObject * owner)
{
	if (data.HasMember("ScriptName") && data["ScriptName"].IsString())
	{
		mName = data["ScriptName"].GetString();
		
		if (!mName.empty()) 
		{
			LoadScript(mName.c_str());
		}
		
	}
	
	if (mScript && data.HasMember("ScriptVariables") && data["ScriptVariables"].IsArray())
	{
		const auto& array = data["ScriptVariables"].GetArray();
		
		for (unsigned int i = 0; i < array.Size(); ++i) 
		{
			const char* name;
			if (array[i].HasMember("VariableName") && array[i]["VariableName"].IsString()) 
			{
				name = array[i]["VariableName"].GetString();

				std::vector<Member*> members = mScript->GetMembers();
				for (const Member* member : members) 
				{
					if (strcmp(member->mName, name) == 0) 
					{
						if (array[i].HasMember("VariableData")) 
						{
							switch (member->mType)
							{
							case MemberType::INT:
								*reinterpret_cast<int*>((((char*)mScript) + member->mOffset)) = array[i]["VariableData"].GetInt();
								break;
							case MemberType::FLOAT:
								*reinterpret_cast<float*>((((char*)mScript) + member->mOffset)) = array[i]["VariableData"].GetFloat();
								break;
							case MemberType::BOOL:
								*reinterpret_cast<bool*>((((char*)mScript) + member->mOffset)) = array[i]["VariableData"].GetBool();
								break;
							case MemberType::FLOAT3:
							{
								const auto& floatArray = array[i]["VariableData"].GetArray();
								*reinterpret_cast<float3*>((((char*)mScript) + member->mOffset)) = float3(floatArray[0].GetFloat(), floatArray[1].GetFloat(), floatArray[2].GetFloat());
								break;
							}
							case MemberType::GAMEOBJECT:
							{
								int  UID = array[i]["VariableData"].GetInt();
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
	}
	//App->GetScriptManager()->AddScript(this);
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
