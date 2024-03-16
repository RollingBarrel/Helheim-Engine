#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleScriptManager.h"
#include <Windows.h>
#include "Script.h"
#include "GameObject.h"
#include "ModuleScene.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner, ComponentType::SCRIPT)
{

}

ScriptComponent::~ScriptComponent()
{
	for (auto data : mData) {

		delete data;
	}

	//delete mScript; //Memory leack here, this shouldbe fixed.

	App->GetScriptManager()->RemoveScript(mScript);

}

void ScriptComponent::Update()
{
}

Component* ScriptComponent::Clone(GameObject* owner) const
{
	return nullptr;
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
		dataArchive.AddInt("VariableType", (int)data->mType);
		switch (data->mType)
		{
		case VariableType::INT:
			dataArchive.AddInt("VariableData", *(int*)data->mData);
			break;
		case VariableType::FLOAT:
			dataArchive.AddInt("VariableData", *(float*)data->mData);
			break;
		case VariableType::BOOL:
			dataArchive.AddInt("VariableData", *(bool*)data->mData);
			break;
		case VariableType::FLOAT3:
			dataArchive.AddFloat3("VariableData", *(float3*)data->mData);
			break;
		case VariableType::GAMEOBJECT:
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
		
		for (int i = 0; i < array.Size(); ++i) {
			const char* name;
			if (array[i].HasMember("VariableName") && array[i]["VariableName"].IsString()) {
				name = array[i]["VariableName"].GetString();

				for (auto data : mData) {
					if (strcmp(data->mName, name) == 0) {
						if (array[i].HasMember("VariableData")) {
							switch (data->mType)
							{
							case VariableType::INT:
								*(int*)data->mData = array[i]["VariableData"].GetInt();
								break;
							case VariableType::FLOAT:
								*(float*)data->mData = array[i]["VariableData"].GetFloat();
								break;
							case VariableType::BOOL:
								*(bool*)data->mData = array[i]["VariableData"].GetBool();
								break;
							case VariableType::FLOAT3:
							{
								const auto& floatArray = array[i]["VariableData"].GetArray();
								*(float3*)data->mData = float3(floatArray[0].GetFloat(), floatArray[1].GetFloat(), floatArray[2].GetFloat());
								break;
							}
							case VariableType::GAMEOBJECT:
							{
								int  UID = array[i]["VariableData"].GetInt();
								if (UID != -1) {
									App->GetScene()->AddGameObjectToLoadIntoScripts(std::pair<unsigned int, GameObject**>(array[i]["VariableData"].GetInt(), (GameObject**)data->mData));
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
	mName = scriptName;

	for (auto data : mData) {

		delete data;
	}
	mData.clear();

	Script* (*script)(GameObject*, std::vector<ScriptVariable*>&) = (Script * (*)(GameObject*, std::vector<ScriptVariable*>&))GetProcAddress(static_cast<HMODULE>(App->GetScriptManager()->GetDLLHandle()), (std::string("Create") + std::string(mName)).c_str());
	if (script != nullptr) {
		mScript = script(mOwner , mData);
		App->GetScriptManager()->AddScript(mScript);
		LOG("LOADING SCRIPT SUCCESS");
	}
	else {
		LOG("LOADING SCRIPT ERROR");
	}
}

void ScriptComponent::Enable()
{
	if (!mIsEnabled) {
		App->GetScriptManager()->AddScript(mScript);
		mIsEnabled = true;
	}
		
}

void ScriptComponent::Disable()
{
	if (mIsEnabled) {
		App->GetScriptManager()->RemoveScript(mScript);
		mIsEnabled = false;
	}
		

}
