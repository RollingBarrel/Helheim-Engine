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

	const std::vector<Member*>& members = mScript->GetMembers();

	for (unsigned int i = 0; i < members.size(); ++i)
	{
		if (members[i]->mType == MemberType::GAMEOBJECT)
		{
			GameObject** gameObject = reinterpret_cast<GameObject**>((((char*)mScript) + members[i]->mOffset));
			App->GetScriptManager()->RemoveGameObjectFromMap(gameObject);
		}
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

	if (mScript)
	{
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
	
}

void::ScriptComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
	
	if (data.HasMember("ScriptName"))
	{
		mName = data.GetString("ScriptName");
		if (!mName.empty())
		{
			LoadScript(mName.c_str());
		}
	}

	if (data.HasMember("ScriptVariables"))
	{
		JsonArray scriptData = data.GetJsonArray("ScriptVariables");
		if (mScript)
		{
			for (unsigned int i = 0; i < scriptData.Size(); ++i)
			{
				JsonObject data = scriptData.GetJsonObject(i); 
				if (data.HasMember("VariableName"))
				{
					std::string name = data.GetString("VariableName");
					const std::vector<Member*> members = mScript->GetMembers();
					bool toFind = true;
					for (std::vector<Member*>::const_iterator it = members.cbegin(); toFind && it != members.cend(); ++it)
					{
						const Member* member = *it;
						if (strcmp(member->mName, name.c_str()) == 0)
						{
							toFind = false;
							if (data.HasMember("MemberType"))
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
									if (data.HasMember("VariableData"))
									{
										float tmp[3];
										data.GetFloats("VariableData", tmp);
										*reinterpret_cast<float3*>((((char*)mScript) + member->mOffset)) = float3(tmp);
									}
									break;
								}
								case MemberType::GAMEOBJECT:
								{
									if (data.HasMember("VariableData"))
									{
										int UID = data.GetInt("VariableData");
										if (UID != -1)
										{		
											const std::unordered_map<unsigned int, unsigned int>& uids = App->GetScene()->GetPrefabUIDMap();
											std::unordered_map<unsigned int, unsigned int>::const_iterator has = uids.find(UID);
											if (has != uids.end())
											{
												std::unordered_map<unsigned int, GameObject*>::const_iterator got = uidPointerMap.find(uids.at(UID));
												if (got != uidPointerMap.end())
												{
													*reinterpret_cast<GameObject**>((((char*)mScript) + member->mOffset)) = uidPointerMap.at(uids.at(UID));
													App->GetScriptManager()->AddGameObjectToMap(reinterpret_cast<GameObject**>((((char*)mScript) + member->mOffset)));
												}
											}
											else
											{
												std::unordered_map<unsigned int, GameObject*>::const_iterator got = uidPointerMap.find(UID);
												if (got != uidPointerMap.end())
												{
													*reinterpret_cast<GameObject**>((((char*)mScript) + member->mOffset)) = uidPointerMap.at(UID);
													App->GetScriptManager()->AddGameObjectToMap(reinterpret_cast<GameObject**>((((char*)mScript) + member->mOffset)));
												}
											}
											
										}
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

		if (IsEnabled())
		{
			App->GetScriptManager()->AddScript(this);
		}
		
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
	
	if (!mHasStarted && App->IsPlayMode())
	{
		mHasStarted = true;
		mScript->Start();
	}
}

void ScriptComponent::Disable()
{
	App->GetScriptManager()->RemoveScript(this);
}
