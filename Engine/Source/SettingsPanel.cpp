#include "SettingsPanel.h"
#include "EngineApp.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "Quadtree.h"
#include "Timer.h"

#include <fstream>
#include <string>
#include <sstream>

#include "imgui.h"

SettingsPanel::SettingsPanel() : Panel(SETTINGSPANEL, false)
{

}

SettingsPanel::~SettingsPanel()
{
	for (auto window : mOpenedWindowsInfo) {
		delete window;
	}
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
			SaveProjectSettings();
		}
		if (ImGui::Button("Load settings")) 
		{
			LoadProjectSettings();
		}
	}
	ImGui::End();
}

void SettingsPanel::SaveProjectSettings()
{
	mOpenedWindowsInfo.clear();
	std::ofstream out_file("projectSettings.txt");

	// Settings variables we want to store
	if (out_file.is_open())
	{
		out_file << "EngineApply frustum culling: " << mCulling << "\n";
		out_file << "Engine Vsync enabled: " << mEngineVsyncEnabled << "\n";
		out_file << "Engine FPS Limit: " << mEngineFpsLimit << "\n";
		out_file << "Game Vsync enabled: " << mGameVsyncEnabled << "\n";
		out_file << "Game FPS Limit: " << mGameFpsLimit << "\n";
		out_file << "Draw Grid: " << mGrid << "\n";
	}

	// Window states
	out_file << EngineApp->GetEditor()->GetPanelList().size() << "\n";
	for (const auto& panels : EngineApp->GetEditor()->GetPanelList())	
	{
		WindowState* windowState = new WindowState();
		ImGui::Begin(panels.first);
		windowState->name = panels.first;
		windowState->IsOpen = panels.second->IsOpen();
		windowState->position = ImGui::GetWindowPos();
		windowState->size = ImGui::GetWindowSize();
		ImGui::End();
		mOpenedWindowsInfo.push_back(windowState);

		if (out_file.is_open())	
		{
			out_file << windowState->name << "\n";
			out_file << windowState->IsOpen << "\n";
			out_file << windowState->position.x << " " << windowState->position.y << "\n";
			out_file << windowState->size.x << " " << windowState->size.y << "\n";
		}
	}

	// Saves the docking layout
	if (out_file.is_open())	
	{
		size_t settings_len;
		const char* settings = ImGui::SaveIniSettingsToMemory(&settings_len);
		out_file.write(settings, settings_len);
		out_file.close();
	}
}

void SettingsPanel::SaveUserSettings()
{
	std::ofstream userSettings("userSettings.txt");
	float3 cameraPosition = EngineApp->GetCamera()->GetEditorCamera()->GetFrustum().pos;
	float3 cameraFront = EngineApp->GetCamera()->GetEditorCamera()->GetFrustum().front;
	float3 cameraUp = EngineApp->GetCamera()->GetEditorCamera()->GetFrustum().up;

	if (userSettings.is_open())
	{
		userSettings << "Camera Position:\n" << cameraPosition.x << '\n' << cameraPosition.y << '\n' << cameraPosition.z << "\n";
		userSettings << "Camera Front:\n" << cameraFront.x << '\n' << cameraFront.y << '\n' << cameraFront.z << "\n";
		userSettings << "Camera Up:\n" << cameraUp.x << '\n' << cameraUp.y << '\n' << cameraUp.z << "\n";

		userSettings << "Scene: \n" << App->GetScene()->GetName();
	}
}

void SettingsPanel::LoadUserSettings()
{
	std::ifstream userSettings("userSettings.txt");

	std::string line;

	float3 cameraPosition;
	float3 cameraFront;
	float3 cameraUp;

	float3 rotation;

	if (userSettings.is_open())
	{
		std::getline(userSettings, line);
		for (int i = 0; i < 3; ++i)
		{
			if (std::getline(userSettings, line))
			cameraPosition[i] = std::stof(line);
		}
		std::getline(userSettings, line);
		for (int i = 0; i < 3; ++i)
		{
			if (std::getline(userSettings, line))
			cameraFront[i] = std::stof(line);
		}
		std::getline(userSettings, line);
		for (int i = 0; i < 3; ++i)
		{
			if (std::getline(userSettings, line))
			cameraUp[i] = std::stof(line);
		}

		EngineApp->GetCamera()->SetPosition(cameraPosition);
		EngineApp->GetCamera()->SetFrontUp(cameraFront, cameraUp);
		
		std::getline(userSettings, line);
		std::getline(userSettings, line);
		App->GetScene()->Load(line.c_str());

	}

	


}

void SettingsPanel::LoadProjectSettings()
{
	mOpenedWindowsInfo.clear();
	// Load the settings for all the windows
	std::ifstream in_file("projectSettings.txt");
	if (in_file.is_open()) 
	{
		std::string line;

		// Load the settings variables
		if (std::getline(in_file, line))
		{
			mCulling = std::stoi(line.substr(line.find(":") + 1));
		}
		if (std::getline(in_file, line))
		{
			mEngineVsyncEnabled = std::stoi(line.substr(line.find(":") + 1));
		}
		if (std::getline(in_file, line))
		{
			mEngineFpsLimit = std::stoi(line.substr(line.find(":") + 1));
		}
		if (std::getline(in_file, line))
		{
			mGameVsyncEnabled = std::stoi(line.substr(line.find(":") + 1));
		}
		if (std::getline(in_file, line))
		{
			mGameFpsLimit = std::stoi(line.substr(line.find(":") + 1));
		}
		if (std::getline(in_file, line))
		{
			mGrid = std::stoi(line.substr(line.find(":") + 1));
		}

		std::vector<const char*> panelNames = EngineApp->GetEditor()->GetPanelNames();

		// Load the windows state
		if (std::getline(in_file, line))
		{
			int numWindows = std::stoi(line);

			for (int i = 0; i < numWindows; ++i)
			{
				WindowState* windowState = new WindowState();

				if (std::getline(in_file, line)) windowState->name = line.c_str();
				if (std::getline(in_file, line)) windowState->IsOpen = std::stoi(line);
				if (std::getline(in_file, line))
				{
					std::istringstream iss(line);
					iss >> windowState->position.x >> windowState->position.y;
				}
				if (std::getline(in_file, line))
				{
					std::istringstream iss(line);
					iss >> windowState->size.x >> windowState->size.y;
				}
				auto it = std::find(panelNames.begin(), panelNames.end(), windowState->name);
				bool panelExists = it != panelNames.end();
				if (panelExists)
				{
					const char* panelName = *it;
					if (windowState->name == panelName && windowState->IsOpen)
					{
						EngineApp->GetEditor()->GetPanel(panelName)->Open();
					}
					mOpenedWindowsInfo.push_back(windowState);
				}
			}
		}

		// EngineApply the loaded settings to the ImGui panel
		EngineApp->GetScene()->SetApplyFrustumCulling(mCulling);
		EngineApp->GetEngineClock()->SetVsyncStatus(mEngineVsyncEnabled);
		EngineApp->GetEngineClock()->SetFpsLimit(mEngineFpsLimit);
		EngineApp->GetGameClock()->SetVsyncStatus(mGameVsyncEnabled);
		EngineApp->GetGameClock()->SetFpsLimit(mGameFpsLimit);
		EngineApp->GetDebugDraw()->SetRenderGrid(mGrid);

		// Load the docking layout
		std::string settings_str((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
		ImGui::LoadIniSettingsFromMemory(settings_str.c_str(), settings_str.size());
		in_file.close();
	}
}
