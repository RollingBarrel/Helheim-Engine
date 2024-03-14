#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleScriptManager.h"
#include <Windows.h>
#include "Script.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner, ComponentType::SCRIPT)
{

}

ScriptComponent::~ScriptComponent()
{
	for (auto data : mData) {

		delete data;
	}
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
			dataArchive.AddString("VariableData" , (const char*)data->mData);
			break;
		case VariableType::BOOL:
			dataArchive.AddBool("VariableData", *(bool*)data->mData);
			break;
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
					if (data->mName == name) {
						if (array[i].HasMember("VariableData") && array[i]["VariableData"].IsFloat()) {
							data->mData = (void*)array[i]["VariableData"].GetString();
						}

					}
				}
			}
			
		}
	}

}

void ScriptComponent::LoadScript(const char* scriptName)
{
	// TODO: Deberia recibir el UID del resource y con el resorce se puede obtener el scriptName y los attributos a mostrar en el inspector
	mName = scriptName;

	for (auto data : mData) {

		delete data;
	}

	mData.clear();

	Script* (*script)(GameObject*, std::vector<ScriptVariable*>&) = (Script * (*)(GameObject*, std::vector<ScriptVariable*>&))GetProcAddress(static_cast<HMODULE>(App->GetScriptManager()->GetDLLHandle()), (std::string("Create") + std::string(mName)).c_str());
	
	if (script != nullptr) {
		mScript = script(mOwner , mData);
		//mScript->mGameObject = owner;
		App->GetScriptManager()->AddScript(mScript);
		LOG("LOADING SCRIPT SUCCESS");
	}
	else {
		LOG("LOADING SCRIPT ERROR");
	}
}
