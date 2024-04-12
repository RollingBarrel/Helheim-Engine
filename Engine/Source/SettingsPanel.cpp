#include "SettingsPanel.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
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

		ImGui::Checkbox("Enable FPS Limit##1", &mEngineFpsLimitEnabled);
		mEngineFpsLimit = App->GetEngineClock()->GetFpsLimit();
		ImGui::SliderInt("FPS Limit##1", &mEngineFpsLimit, 10, 240);
		if (mEngineVsyncEnabled) 
		{
			ImGui::EndDisabled();
		}

		App->GetEngineClock()->SetFpsLimit(mEngineFpsLimit);
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

		ImGui::Checkbox("Enable FPS Limit##2", &mGameFpsLimitEnabled);
		ImGui::SliderInt("FPS Limit##2", &mGameFpsLimit, 10, 240);
		if (mGameVsyncEnabled) 
		{
			ImGui::EndDisabled();
		}

		App->GetGameClock()->SetFpsLimit(mGameFpsLimit);
		ImGui::Unindent();

		ImGui::SeparatorText("Editor settings");
		if (ImGui::Checkbox("Draw Grid", &mGrid)) 
		{
			App->GetDebugDraw()->SetRenderGrid(mGrid);
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
	mOpenedWindowsInfo.clear();
	std::ofstream out_file("config.txt");

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

void SettingsPanel::LoadSettings()
{
	mOpenedWindowsInfo.clear();
	// Load the settings for all the windows
	std::ifstream in_file("config.txt");
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
				const char* panelName = App->GetEditor()->GetPanelNames().at(i);
				if (windowState->name == panelName && windowState->IsOpen)
				{
					App->GetEditor()->GetPanel(panelName)->Open();
				}

				mOpenedWindowsInfo.push_back(windowState);
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
