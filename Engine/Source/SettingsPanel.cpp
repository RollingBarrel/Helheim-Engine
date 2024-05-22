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

		ImGui::Unindent();

		ImGui::SeparatorText("Editor settings");
		if (ImGui::Checkbox("Draw Grid", &mGrid)) 
		{
			EngineApp->GetDebugDraw()->SetRenderGrid(mGrid);
		}

		if (ImGui::Button("Save settings")) 
		{
			SaveSettings();
		}
		if (ImGui::Button("Load settings")) 
		{
			LoadSettings();
		}
	}
	ImGui::End();
}

void SettingsPanel::SaveSettings()
{
	//Archive* archive = new Archive();

	//Archive engine;
	//engine.AddBool("Draw Grid", mGrid);
	//engine.AddBool("Frustum Culling", mCulling);
	//engine.AddBool("Vsync", mEngineVsyncEnabled);
	//engine.AddBool("Enable FPS Limit", mEngineFpsLimitEnabled);
	//engine.AddFloat("FPS Limit", mEngineFpsLimit);
	//archive->AddObject("Engine Settings", engine);
	//
	//Archive game;
	//game.AddBool("Vsync", mGameVsyncEnabled);
	//game.AddBool("Enable FPS", mGameFpsLimitEnabled);
	//game.AddFloat("FPS Limit", mGameFpsLimit);
	//archive->AddObject("Game Settings", game);
	//
	//Archive camera;
	//camera.AddFloat3("Position", EngineApp->GetEngineCamera()->mEditorCameraGameObject->GetPosition());
	//camera.AddFloat3("Rotation", EngineApp->GetEngineCamera()->mEditorCameraGameObject->GetEulerAngles());
	//archive->AddObject("Camera Settings", camera);
	//
	//Archive scene;
	//scene.AddString("Name", App->GetScene()->GetName().c_str());
	//
	//Archive tag;
	//for (int i = 0; i < App->GetScene()->GetSystemTag().size(); ++i)
	//{
	//	tag.AddInt(App->GetScene()->GetSystemTag()[i]->GetName().c_str(), App->GetScene()->GetSystemTag()[i]->GetID());
	//}
	//scene.AddObject("Tags", tag);
	//archive->AddObject("Scene", scene);
	//
	//std::string fileBuffer = archive->Serialize();
	//
	//std::string savePath = "projectSettings.txt";
	//App->GetFileSystem()->Save(savePath.c_str(), fileBuffer.c_str(), static_cast<unsigned int>(fileBuffer.length() + 1));
	//
	//delete archive;
}

void SettingsPanel::LoadSettings()
{
	char* fileBuffer;
	App->GetFileSystem()->Load("projectSettings.txt", &fileBuffer);

	Archive* archive = new Archive();
	//archive->Deserialize(&fileBuffer);
	//
	//if (archive->GetDocument()->HasMember("Engine Settings"))
	//{
	//	LOG("hi");
	//}
	//std::ifstream userSettings("userSettings.txt");
	//
	//std::string line;
	//
	//float3 cameraPosition;
	//float3 cameraRotation;
	//
	//if (userSettings.is_open())
	//{
	//	std::getline(userSettings, line);
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		if (std::getline(userSettings, line))
	//		cameraPosition[i] = std::stof(line);
	//	}
	//	std::getline(userSettings, line);
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		if (std::getline(userSettings, line))
	//		cameraRotation[i] = std::stof(line);
	//	}
	//
	//	EngineApp->GetEngineCamera()->mEditorCameraGameObject->SetPosition(cameraPosition);
	//	EngineApp->GetEngineCamera()->mEditorCameraGameObject->SetRotation(cameraRotation);
	//	
	//	std::getline(userSettings, line);
	//	std::getline(userSettings, line);
	//	App->GetScene()->Load(line.c_str());
	//
	//}
}	

//void SettingsPanel::LoadProjectSettings()
//{
	//mOpenedWindowsInfo.clear();
	//// Load the settings for all the windows
	//std::ifstream in_file("projectSettings.txt");
	//if (in_file.is_open()) 
	//{
	//	std::string line;
	//
	//	// Load the settings variables
	//	if (std::getline(in_file, line))
	//	{
	//		mCulling = std::stoi(line.substr(line.find(":") + 1));
	//	}
	//	if (std::getline(in_file, line))
	//	{
	//		mEngineVsyncEnabled = std::stoi(line.substr(line.find(":") + 1));
	//	}
	//	if (std::getline(in_file, line))
	//	{
	//		mEngineFpsLimit = std::stoi(line.substr(line.find(":") + 1));
	//	}
	//	if (std::getline(in_file, line))
	//	{
	//		mGameVsyncEnabled = std::stoi(line.substr(line.find(":") + 1));
	//	}
	//	if (std::getline(in_file, line))
	//	{
	//		mGameFpsLimit = std::stoi(line.substr(line.find(":") + 1));
	//	}
	//	if (std::getline(in_file, line))
	//	{
	//		mGrid = std::stoi(line.substr(line.find(":") + 1));
	//	}
	//
	//	std::vector<const char*> panelNames = EngineApp->GetEditor()->GetPanelNames();
	//
	//	// Load the windows state
	//	if (std::getline(in_file, line))
	//	{
	//		int numWindows = std::stoi(line);
	//
	//		for (int i = 0; i < numWindows; ++i)
	//		{
	//			WindowState* windowState = new WindowState();
	//
	//			if (std::getline(in_file, line)) windowState->name = line.c_str();
	//			if (std::getline(in_file, line)) windowState->IsOpen = std::stoi(line);
	//			if (std::getline(in_file, line))
	//			{
	//				std::istringstream iss(line);
	//				iss >> windowState->position.x >> windowState->position.y;
	//			}
	//			if (std::getline(in_file, line))
	//			{
	//				std::istringstream iss(line);
	//				iss >> windowState->size.x >> windowState->size.y;
	//			}
	//			auto it = std::find(panelNames.begin(), panelNames.end(), windowState->name);
	//			bool panelExists = it != panelNames.end();
	//			if (panelExists)
	//			{
	//				const char* panelName = *it;
	//				if (windowState->name == panelName && windowState->IsOpen)
	//				{
	//					EngineApp->GetEditor()->GetPanel(panelName)->Open();
	//				}
	//				mOpenedWindowsInfo.push_back(windowState);
	//			}
	//		}
	//	}
	//
	//	// EngineApply the loaded settings to the ImGui panel
	//	EngineApp->GetScene()->SetApplyFrustumCulling(mCulling);
	//	EngineApp->GetEngineClock()->SetVsyncStatus(mEngineVsyncEnabled);
	//	EngineApp->GetEngineClock()->SetFpsLimit(mEngineFpsLimit);
	//	EngineApp->GetGameClock()->SetVsyncStatus(mGameVsyncEnabled);
	//	EngineApp->GetGameClock()->SetFpsLimit(mGameFpsLimit);
	//	EngineApp->GetDebugDraw()->SetRenderGrid(mGrid);
	//
	//	// Load the docking layout
	//	std::string settings_str((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
	//	ImGui::LoadIniSettingsFromMemory(settings_str.c_str(), settings_str.size());
	//	in_file.close();
	//}
//}
