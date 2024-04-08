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
	Enable();
}

ScriptComponent::ScriptComponent(const ScriptComponent& other, GameObject* owner) : Component(owner, ComponentType::SCRIPT)
{
	mName = other.mName;
	LoadScript(mName.c_str());

	for (int i = 0; i < mData.size(); i++) {
		switch (mData[i]->mType)
		{
		case MemberType::INT:
			*(int*)mData[i]->mData = *(int*)other.mData[i]->mData;
			break;
		case MemberType::FLOAT:
			*(float*)mData[i]->mData = *(float*)other.mData[i]->mData;
			break;
		case MemberType::BOOL:
			*(bool*)mData[i]->mData = *(bool*)other.mData[i]->mData;
			break;
		case MemberType::FLOAT3:
			*(float3*)mData[i]->mData = *(float3*)other.mData[i]->mData;
			break;
		case MemberType::GAMEOBJECT:
			*(GameObject**)mData[i]->mData = *(GameObject**)other.mData[i]->mData;
			break;
		}
	}

	Enable();

}

ScriptComponent::~ScriptComponent()
{
	for (auto data : mData) {

		delete data;
	}

	//delete mScript; //Memory leack here, this shouldbe fixed.

	App->GetScriptManager()->RemoveScript(this);
	if (mResourceScript)
	{
		delete mResourceScript;
	}
	
}

void ScriptComponent::Update()
{

	float jorge;

	LOG("%s", typeid(jorge).name());
	LOG("%s", typeid(jorge).raw_name());;
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

	std::vector<Archive> objectArray;

	for (const auto& data : mData) {

		Archive dataArchive;	
		dataArchive.AddString("VariableName", data->mName);
		dataArchive.AddInt("MemberType", (int)data->mType);
		switch (data->mType)
		{
		case MemberType::INT:
			dataArchive.AddInt("VariableData", *(int*)data->mData);
			break;
		case MemberType::FLOAT:
			dataArchive.AddFloat("VariableData", *(float*)data->mData);
			break;
		case MemberType::BOOL:
			dataArchive.AddInt("VariableData", *(bool*)data->mData);
			break;
		case MemberType::FLOAT3:
			dataArchive.AddFloat3("VariableData", *(float3*)data->mData);
			break;
		case MemberType::GAMEOBJECT:
		{
			(*(GameObject**)data->mData) ? dataArchive.AddInt("VariableData", (*(GameObject**)data->mData)->GetID()) : dataArchive.AddInt("VariableData", -1);
			break;
		}
		default:
			break;
		}
		

		objectArray.push_back(dataArchive);
	}

	archive.AddObjectArray("ScriptVariables", objectArray);
}

void::ScriptComponent::LoadFromJSON(const rapidjson::Value & data, GameObject * owner)
{
	bool check = data.HasMember("ScriptVariables");
	bool check2 = data["ScriptVariables"].IsString();

	LOG("%i", check);
	LOG("%i", check2);

	if (data.HasMember("ScriptName") && data["ScriptName"].IsString())
	{
		mName = data["ScriptName"].GetString();
		
		if (!mName.empty()) {
			LoadScript(mName.c_str());
		}
		
	}
	
	if (data.HasMember("ScriptVariables") && data["ScriptVariables"].IsArray())
	{
		const auto& array = data["ScriptVariables"].GetArray();
		
		for (unsigned int i = 0; i < array.Size(); ++i) {
			const char* name;
			if (array[i].HasMember("VariableName") && array[i]["VariableName"].IsString()) {
				name = array[i]["VariableName"].GetString();

				for (auto data : mData) {
					if (strcmp(data->mName, name) == 0) {
						if (array[i].HasMember("VariableData")) {
							switch (data->mType)
							{
							case MemberType::INT:
								*(int*)data->mData = array[i]["VariableData"].GetInt();
								break;
							case MemberType::FLOAT:
								*(float*)data->mData = array[i]["VariableData"].GetFloat();
								break;
							case MemberType::BOOL:
								*(bool*)data->mData = array[i]["VariableData"].GetBool();
								break;
							case MemberType::FLOAT3:
							{
								const auto& floatArray = array[i]["VariableData"].GetArray();
								*(float3*)data->mData = float3(floatArray[0].GetFloat(), floatArray[1].GetFloat(), floatArray[2].GetFloat());
								break;
							}
							case MemberType::GAMEOBJECT:
							{
								int  UID = array[i]["VariableData"].GetInt();
								if (UID != -1) {
									App->GetScene()->AddGameObjectToLoadIntoScripts(std::pair<unsigned int, GameObject**>(UID, (GameObject**)data->mData));
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

	mScript = nullptr;
	for (auto data : mData) 
	{
		delete data;
	}
	mData.clear();

	mName = scriptName;
	Script* (*script)(GameObject*, std::vector<ScriptVariable*>&) = (Script * (*)(GameObject*, std::vector<ScriptVariable*>&))GetProcAddress(static_cast<HMODULE>(App->GetScriptManager()->GetDLLHandle()), (std::string("Create") + std::string(mName)).c_str());
	if (script != nullptr) {	
		mScript = script(mOwner , mData);
		mScript->SetName(mName);
		mResourceScript = (ResourceScript*)(App->GetResource()->RequestResource(scriptPath.c_str()));
		if (mResourceScript == nullptr) {
			LOG("SCRIPT RESOURCE NOT FOUND");
		}
		//App->GetScriptManager()->AddScript(mScript);
		LOG("LOADING SCRIPT SUCCESS");
	}
	else {
		LOG("LOADING SCRIPT ERROR");
	}
}

void ScriptComponent::Enable()
{
	App->GetScriptManager()->AddScript(this);
		
}

void ScriptComponent::Disable()
{
		App->GetScriptManager()->RemoveScript(this);
}
