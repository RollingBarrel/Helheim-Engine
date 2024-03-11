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
}

void SettingsPanel::Draw(int windowFlags) 
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags)) 
	{
		//Config settings update
		culling = App->GetScene()->GetApplyFrustumCulling();
		engineVsyncEnabled = App->GetEngineClock()->GetVsyncStatus();
		gameVsyncEnabled = App->GetGameClock()->GetVsyncStatus();
		engineFpsLimit = App->GetEngineClock()->GetFpsLimit();
		gameFpsLimit = App->GetGameClock()->GetFpsLimit();
		grid = App->GetDebugDraw()->GetShouldRenderGrid();

		ImGui::SeparatorText("Graphic settings");
		if (ImGui::Checkbox("Apply frustum culling", &culling)) 
		{
			App->GetScene()->SetApplyFrustumCulling(culling);
		}
		ImGui::Indent();
		ImGui::SeparatorText("Engine");
		ImGui::Checkbox("Engine Vsync enabled", &engineVsyncEnabled);
		if (engineVsyncEnabled != App->GetEngineClock()->GetVsyncStatus()) 
		{
			App->GetEngineClock()->SetVsyncStatus(engineVsyncEnabled);
		}

		if (engineVsyncEnabled) 
		{
			ImGui::BeginDisabled();
		}

		ImGui::Checkbox("Enable FPS Limit##1", &engineFpsLimitEnabled);
		engineFpsLimit = App->GetEngineClock()->GetFpsLimit();
		ImGui::SliderInt("FPS Limit##1", &engineFpsLimit, 10, 240);
		if (engineVsyncEnabled) 
		{
			ImGui::EndDisabled();
		}

		App->GetEngineClock()->SetFpsLimit(engineFpsLimit);
		ImGui::Spacing();
		ImGui::SeparatorText("Game");
		ImGui::Checkbox("Game Vsync enabled", &gameVsyncEnabled);
		if (gameVsyncEnabled != App->GetGameClock()->GetVsyncStatus()) 
		{
			App->GetGameClock()->SetVsyncStatus(gameVsyncEnabled);
		}

		if (gameVsyncEnabled) 
		{
			ImGui::BeginDisabled();
		}

		ImGui::Checkbox("Enable FPS Limit##2", &gameFpsLimitEnabled);
		ImGui::SliderInt("FPS Limit##2", &gameFpsLimit, 10, 240);
		if (gameVsyncEnabled) 
		{
			ImGui::EndDisabled();
		}

		App->GetGameClock()->SetFpsLimit(gameFpsLimit);
		ImGui::Unindent();

		ImGui::SeparatorText("Editor settings");
		if (ImGui::Checkbox("Draw Grid", &grid)) 
		{
			App->GetDebugDraw()->SetRenderGrid(grid);
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
	// Save the settings for all the windows
	std::ofstream out_file("config.txt");
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
		
		// Write the window state to the file
		if (out_file.is_open())	
		{
			out_file << windowState->name << "\n";
			out_file << windowState->IsOpen << "\n";
			out_file << windowState->position.x << " " << windowState->position.y << "\n";
			out_file << windowState->size.x << " " << windowState->size.y << "\n";
		}
	}
	if (out_file.is_open())	
	{
		// Save the docking layout
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
		int numWindows = 0;
		if (std::getline(in_file, line)) 
		{
			numWindows = std::stoi(line);
		}
		for (int i = 0; i < numWindows; ++i) 
		{
			WindowState* windowState = new WindowState();

			// Read the window state from the file
			if (std::getline(in_file, line)) windowState->name = line;
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
			mOpenedWindowsInfo.push_back(windowState);
		}
		// Load the docking layout
		std::string settings_str((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
		ImGui::LoadIniSettingsFromMemory(settings_str.c_str(), settings_str.size());
		in_file.close();
	}

	// Apply the loaded settings to the windows
	for (const auto& windowState : mOpenedWindowsInfo) 
	{
		if (ImGui::Begin(windowState->name.c_str())) 
		{
			if(windowState->IsOpen) App->GetEditor()->GetPanel(windowState->name.c_str())->Open();
			ImGui::SetWindowPos(windowState->position);
			ImGui::SetWindowSize(windowState->size);
		}
		ImGui::End();
	}
}
