#include "SettingsPanel.h"
#include "Application.h"
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
		mCulling = App->GetScene()->GetApplyFrustumCulling();
		mEngineVsyncEnabled = App->GetEngineClock()->GetVsyncStatus();
		mGameVsyncEnabled = App->GetGameClock()->GetVsyncStatus();
		mEngineFpsLimitEnabled = App->GetEngineClock()->IsFpsLimitEnabled();
		mGameFpsLimitEnabled = App->GetGameClock()->IsFpsLimitEnabled();
		mEngineFpsLimit = App->GetEngineClock()->GetFpsLimit();
		mGameFpsLimit = App->GetGameClock()->GetFpsLimit();
		mGrid = App->GetDebugDraw()->GetShouldRenderGrid();

		ImGui::SeparatorText("Graphic settings");
		if (ImGui::Checkbox("Apply frustum culling", &mCulling))
		{
			App->GetScene()->SetApplyFrustumCulling(mCulling);
		}
		ImGui::Indent();
		ImGui::SeparatorText("Engine");
		ImGui::Checkbox("Engine Vsync enabled", &mEngineVsyncEnabled);
		if (mEngineVsyncEnabled != App->GetEngineClock()->GetVsyncStatus())
		{
			App->GetEngineClock()->SetVsyncStatus(mEngineVsyncEnabled);
		}

		if (mEngineVsyncEnabled) 
		{
			ImGui::BeginDisabled();
		}

		if (ImGui::Checkbox("Enable FPS Limit##1", &mEngineFpsLimitEnabled))
		{
			App->GetEngineClock()->EnableFpsLimit(mEngineFpsLimitEnabled);
		}

		if (ImGui::SliderInt("FPS Limit##1", &mEngineFpsLimit, 10, 240))
		{
			App->GetEngineClock()->SetFpsLimit(mEngineFpsLimit);
		}
		
		if (mEngineVsyncEnabled) 
		{
			ImGui::EndDisabled();
		}

		ImGui::Spacing();
		ImGui::SeparatorText("Game");
		ImGui::Checkbox("Game Vsync enabled", &mGameVsyncEnabled);
		if (mGameVsyncEnabled != App->GetGameClock()->GetVsyncStatus()) 
		{
			App->GetGameClock()->SetVsyncStatus(mGameVsyncEnabled);
		}

		if (mGameVsyncEnabled) 
		{
			ImGui::BeginDisabled();
		}

		if (ImGui::Checkbox("Enable FPS Limit##2", &mGameFpsLimitEnabled))
		{
			App->GetGameClock()->EnableFpsLimit(mGameFpsLimitEnabled);
		}

		if (ImGui::SliderInt("FPS Limit##2", &mGameFpsLimit, 10, 240))
		{
			App->GetGameClock()->SetFpsLimit(mGameFpsLimit);
		}

		if (mGameVsyncEnabled) 
		{
			ImGui::EndDisabled();
		}

		ImGui::Unindent();

		ImGui::SeparatorText("Editor settings");
		if (ImGui::Checkbox("Draw Grid", &mGrid)) 
		{
			App->GetDebugDraw()->SetRenderGrid(mGrid);
		}

		if (ImGui::Button("Save settings")) 
		{
			SaveProjectSettings();
		}
		if (ImGui::Button("Load settings")) 
		{
			LoadProjectSettings();
		}

		if (ImGui::Button("camara"))
		{
			SaveCameraPosition();
			LoadCameraPosition();
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
		out_file << "Apply frustum culling: " << mCulling << "\n";
		out_file << "Engine Vsync enabled: " << mEngineVsyncEnabled << "\n";
		out_file << "Engine FPS Limit: " << mEngineFpsLimit << "\n";
		out_file << "Game Vsync enabled: " << mGameVsyncEnabled << "\n";
		out_file << "Game FPS Limit: " << mGameFpsLimit << "\n";
		out_file << "Draw Grid: " << mGrid << "\n";
	}

	// Window states
	out_file << App->GetEditor()->GetPanelList().size() << "\n";
	for (const auto& panels : App->GetEditor()->GetPanelList())	
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

void SettingsPanel::SaveCameraPosition()
{
	std::ofstream userSettings("userSettings.txt");
	float3 cameraPosition = App->GetCamera()->GetEditorCamera()->GetFrustum().pos;
	float3 cameraFront = App->GetCamera()->GetEditorCamera()->GetFrustum().front;
	float3 cameraUp = App->GetCamera()->GetEditorCamera()->GetFrustum().up;

	if (userSettings.is_open())
	{
		userSettings << "Camera Position:\n" << cameraPosition.x << '\n' << cameraPosition.y << '\n' << cameraPosition.z << "\n";
		userSettings << "Camera Front:\n" << cameraFront.x << '\n' << cameraFront.y << '\n' << cameraFront.z << "\n";
		userSettings << "Camera Up:\n" << cameraUp.x << '\n' << cameraUp.y << '\n' << cameraUp.z << "\n";
	}
}

void SettingsPanel::LoadCameraPosition()
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
		std::getline(userSettings, line);
		for (int i = 0; i < 3; ++i)
		{
			if (std::getline(userSettings, line))
				rotation[i] = std::stof(line);
		}

		App->GetCamera()->SetPosition(cameraPosition);
		App->GetCamera()->SetFrontUp(cameraFront, cameraUp);
		
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

		std::vector<const char*> panelNames = App->GetEditor()->GetPanelNames();

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
						App->GetEditor()->GetPanel(panelName)->Open();
					}
					mOpenedWindowsInfo.push_back(windowState);
				}
			}
		}

		// Apply the loaded settings to the ImGui panel
		App->GetScene()->SetApplyFrustumCulling(mCulling);
		App->GetEngineClock()->SetVsyncStatus(mEngineVsyncEnabled);
		App->GetEngineClock()->SetFpsLimit(mEngineFpsLimit);
		App->GetGameClock()->SetVsyncStatus(mGameVsyncEnabled);
		App->GetGameClock()->SetFpsLimit(mGameFpsLimit);
		App->GetDebugDraw()->SetRenderGrid(mGrid);

		// Load the docking layout
		std::string settings_str((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
		ImGui::LoadIniSettingsFromMemory(settings_str.c_str(), settings_str.size());
		in_file.close();
	}
}
