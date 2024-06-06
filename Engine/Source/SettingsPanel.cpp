#include "SettingsPanel.h"
#include "EngineApp.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleEngineCamera.h"
#include "ModuleFileSystem.h"
#include "Archive.h"
#include "GameObject.h"
#include "Timer.h"

#include "imgui.h"




SettingsPanel::SettingsPanel() : Panel(SETTINGSPANEL, false)
{
}

SettingsPanel::~SettingsPanel()
{
}

void SettingsPanel::Draw(int windowFlags) 
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags)) 
	{
		//Config settings update
		mCulling = EngineApp->GetScene()->GetApplyFrustumCulling();
		mEngineVsyncEnabled = EngineApp->GetEngineClock()->GetVsyncStatus();
		mGameVsyncEnabled = EngineApp->GetGameClock()->GetVsyncStatus();
		mEngineFpsLimitEnabled = EngineApp->GetEngineClock()->IsFpsLimitEnabled();
		mGameFpsLimitEnabled = EngineApp->GetGameClock()->IsFpsLimitEnabled();
		mEngineFpsLimit = EngineApp->GetEngineClock()->GetFpsLimit();
		mGameFpsLimit = EngineApp->GetGameClock()->GetFpsLimit();
		mGrid = EngineApp->GetDebugDraw()->GetShouldRenderGrid();

		ImGui::SeparatorText("Scene");
		{
			ImGui::Text("Current Scene: ");
			ImGui::SameLine();
			ImGui::Text(EngineApp->GetScene()->GetName().c_str());
		}
		//TODO: Redoo this to delete Timer and other windows 
		if(ImGui::CollapsingHeader("Graphics"))
		{
			ImGui::SeparatorText("Graphic settings");
			if (ImGui::Checkbox("EngineApply frustum culling", &mCulling))
			{
				EngineApp->GetScene()->SetApplyFrustumCulling(mCulling);
			}
			ImGui::Indent();
			ImGui::SeparatorText("Engine");
			ImGui::Checkbox("Engine Vsync enabled", &mEngineVsyncEnabled);
			if (mEngineVsyncEnabled != EngineApp->GetEngineClock()->GetVsyncStatus())
			{
				EngineApp->GetEngineClock()->SetVsyncStatus(mEngineVsyncEnabled);
			}

			if (mEngineVsyncEnabled)
			{
				ImGui::BeginDisabled();
			}

			if (ImGui::Checkbox("Enable FPS Limit##1", &mEngineFpsLimitEnabled))
			{
				EngineApp->GetEngineClock()->EnableFpsLimit(mEngineFpsLimitEnabled);
			}

			if (ImGui::SliderInt("FPS Limit##1", &mEngineFpsLimit, 10, 240))
			{
				EngineApp->GetEngineClock()->SetFpsLimit(mEngineFpsLimit);
			}

			if (mEngineVsyncEnabled)
			{
				ImGui::EndDisabled();
			}
			ImGui::Spacing();
			ImGui::SeparatorText("Game");
			ImGui::Checkbox("Game Vsync enabled", &mGameVsyncEnabled);
			if (mGameVsyncEnabled != EngineApp->GetGameClock()->GetVsyncStatus())
			{
				EngineApp->GetGameClock()->SetVsyncStatus(mGameVsyncEnabled);
			}

			if (mGameVsyncEnabled)
			{
				ImGui::BeginDisabled();
			}

			if (ImGui::Checkbox("Enable FPS Limit##2", &mGameFpsLimitEnabled))
			{
				EngineApp->GetGameClock()->EnableFpsLimit(mGameFpsLimitEnabled);
			}

			if (ImGui::SliderInt("FPS Limit##2", &mGameFpsLimit, 10, 240))
			{
				EngineApp->GetGameClock()->SetFpsLimit(mGameFpsLimit);
			}

			if (mGameVsyncEnabled)
			{
				ImGui::EndDisabled();
			}
		}

		ImGui::SeparatorText("Editor settings");

		if (ImGui::Checkbox("Draw Grid", &mGrid)) 
		{
			EngineApp->GetDebugDraw()->SetRenderGrid(mGrid);
		}

		if (ImGui::Button("Save settings")) 
		{
			SaveUserSettings();
		}
		if (ImGui::Button("Load settings")) 
		{
			LoadUserSettings();
		}

	}
	ImGui::End();
}

void SettingsPanel::SaveUserSettings() const
{
	Archive doc;
	JsonObject root = doc.GetRootObject();
	JsonObject engine = root.AddNewJsonObject("Engine Settings");
	engine.AddBool("Draw Grid", mGrid);
	engine.AddBool("Frustum Culling", mCulling);
	engine.AddBool("Vsync", mEngineVsyncEnabled);
	engine.AddBool("Enable FPS Limit", mEngineFpsLimitEnabled);
	engine.AddFloat("FPS Limit", mEngineFpsLimit);

	JsonObject game = root.AddNewJsonObject("Game Settings");
	game.AddBool("Vsync", mGameVsyncEnabled);
	game.AddBool("Enable FPS Limit", mGameFpsLimitEnabled);
	game.AddFloat("FPS Limit", mGameFpsLimit);

	JsonObject camera = root.AddNewJsonObject("Camera Settings");
	camera.AddFloats("Position", EngineApp->GetEngineCamera()->mEditorCameraGameObject->GetPosition().ptr(), 3);
	camera.AddFloats("Rotation", EngineApp->GetEngineCamera()->mEditorCameraGameObject->GetRotation().ptr(), 4);

	JsonObject scene = root.AddNewJsonObject("Scene Settings");
	scene.AddString("Name", EngineApp->GetScene()->GetName().c_str());

	std::string buffer = doc.Serialize();
	App->GetFileSystem()->Save("userSettings.json", buffer.c_str(), buffer.length());
}

void SettingsPanel::LoadUserSettings()
{
	char* fileBuffer;
	
	if (App->GetFileSystem()->Load("userSettings.json", &fileBuffer))
	{
		Archive doc(fileBuffer);

		JsonObject root = doc.GetRootObject();

		JsonObject engine = root.GetJsonObject("Engine Settings");
		mGrid = engine.GetBool("Draw Grid");
		EngineApp->GetDebugDraw()->SetRenderGrid(mGrid);
		mCulling = engine.GetBool("Frustum Culling");
		EngineApp->GetScene()->SetApplyFrustumCulling(mCulling);
		mEngineVsyncEnabled = engine.GetBool("Vsync");
		EngineApp->GetEngineClock()->SetVsyncStatus(mEngineVsyncEnabled);
		mEngineFpsLimitEnabled = engine.GetBool("Enable FPS Limit");
		EngineApp->GetEngineClock()->EnableFpsLimit(mEngineFpsLimitEnabled);
		mEngineFpsLimit = engine.GetFloat("FPS Limit");
		EngineApp->GetEngineClock()->SetFpsLimit(mEngineFpsLimit);

		JsonObject game = root.GetJsonObject("Game Settings");
		mGameVsyncEnabled = game.GetBool("Vsync");
		EngineApp->GetGameClock()->SetVsyncStatus(mGameVsyncEnabled);
		mGameFpsLimitEnabled = game.GetBool("Enable FPS Limit");
		EngineApp->GetGameClock()->EnableFpsLimit(mGameFpsLimitEnabled);
		mGameFpsLimit = game.GetFloat("FPS Limit");
		EngineApp->GetGameClock()->SetFpsLimit(mGameFpsLimit);

		JsonObject camera = root.GetJsonObject("Camera Settings");
		float position[3];
		camera.GetFloats("Position", position);
		EngineApp->GetEngineCamera()->mEditorCameraGameObject->SetPosition(float3(position));

		float rotation[4];
		camera.GetFloats("Rotation", rotation);
		EngineApp->GetEngineCamera()->mEditorCameraGameObject->SetRotation(Quat(rotation));

		JsonObject scene = root.GetJsonObject("Scene Settings");
		std::string name = scene.GetString("Name");
		//App->GetScene()->Load(name.c_str());		//TODO: Uncomment this
	}	
}

void SettingsPanel::AddTag(const char* newTag)
{
	mTags.push_back(newTag);
	SaveProjectSettings();
}

void SettingsPanel::DeleteTag(const char* tagToDelete)
{
	std::vector<GameObject*> foundGameObjects;
	App->GetScene()->FindGameObjectsWithTag(tagToDelete, foundGameObjects);

	if (foundGameObjects.empty())
	{
		for (std::vector<std::string>::iterator it = mTags.begin(); it != mTags.end(); ++i)
		{
			if ((*it).compare("tagToDelete") == 0)
			{
				mTags.erase(it);
				SaveProjectSettings();
				break;
			}
		}
	}
	else 
	{
		//TODO:: POPUP TAG CANT BE DELETED
	}

}

void SettingsPanel::SaveProjectSettings() const
{
	Archive document;
	JsonObject root = document.GetRootObject();
	JsonArray tags = root.AddNewJsonArray("Tags");

	for (int i = NUM_ENGINE_TAGS; i < mTags.size(); ++i)
	{
		tags.PushBackString(mTags[i].c_str());
	}

	std::string buffer = document.Serialize();
	App->GetFileSystem()->Save("projectSettings.json", buffer.c_str(), buffer.length());
}

void SettingsPanel::LoadProjectSettings()
{
	char* fileBuffer;

	if (App->GetFileSystem()->Load("projectSettings.json", &fileBuffer))
	{
		Archive document(fileBuffer);

		JsonObject root = document.GetRootObject();
		JsonArray tags = root.GetJsonArray("Tags");

		mTags.reserve(tags.Size() + NUM_ENGINE_TAGS);

		mTags.push_back("Untagged");
		mTags.push_back("MainCamera");
		mTags.push_back("Player");

		for (int i = 0; i < tags.Size(); ++i)
		{
			mTags.push_back(tags.GetString(i));
		}
	}
	else
	{
		mTags.reserve(NUM_ENGINE_TAGS);

		mTags.push_back("Untagged");
		mTags.push_back("MainCamera");
		mTags.push_back("Player");
	}

}

