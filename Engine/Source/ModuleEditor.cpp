#include "EngineApp.h"
#include "SDL.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleFileSystem.h"
#include "ModuleEngineScriptManager.h"
#include "ModuleEngineResource.h"

#include "Panel.h"
#include "AboutPanel.h"
#include "ConsolePanel.h"
#include "InspectorPanel.h"
#include "HierarchyPanel.h"
#include "ScenePanel.h"
#include "NavMeshControllerPanel.h"
#include "DebugPanel.h"
#include "ProjectPanel.h"
#include "LightningPanel.h"
#include "ResourcePanel.h"
#include "TimerPanel.h"
#include "SettingsPanel.h"
#include "EditorControlPanel.h"
#include "AnimationSMPanel.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "ImGuiFileDialog.h"


#include "OptickAdapter.h"
#include "IconsFontAwesome6.h"



ModuleEditor::ModuleEditor()
{
	// Panels
	mPanels[ABOUTPANEL] = new AboutPanel();
	mPanels[CONSOLEPANEL] = new ConsolePanel();
	mPanels[INSPECTORPANEL] = new InspectorPanel();
	mPanels[HIERARCHYPANEL] = new HierarchyPanel();
	mPanels[SCENEPANEL] = new ScenePanel();
	mPanels[NAVMESHPANEL] = new NavMeshControllerPanel();
	mPanels[PROJECTPANEL] = new ProjectPanel();
	mPanels[DEBUGPANEL] = new DebugPanel();
	mPanels[LIGHTNINGPANEL] = new LightningPanel();
	mPanels[RESOURCEPANEL] = new ResourcePanel();
	mPanels[TIMERPANEL] = new TimerPanel();
	mPanels[EDITORCONTROLPANEL] = new EditorControlPanel();
	mPanels[SETTINGSPANEL] = new SettingsPanel();
	mPanels[ANIMATIONSMPANEL] = new AnimationSMPanel();

	for (auto panel : mPanels) 
	{
		mPanelNames.push_back(panel.first);
	}
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();

	mIO = &(ImGui::GetIO());
	//mIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	mIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	mIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	mIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	mIO->ConfigDragClickToInputText = true;

	mIO->IniFilename = nullptr;


	ImGui_ImplSDL2_InitForOpenGL(EngineApp->GetWindow()->mWindow, EngineApp->GetOpenGL()->GetOpenGlContext());
	ImGui_ImplOpenGL3_Init("#version 460");

	mIO->Fonts->AddFontDefault();
	float baseFontSize = 34.0f; // 13.0f is the size of the default font. Change to the font size you use.
	float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
	
	// Merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	icons_config.GlyphOffset = ImVec2(0, 5); // This Y offset works with the Guizmo buttons and its pertinent icons, but could be different for other button sizes

	mIO->Fonts->AddFontFromFileTTF("InternalAssets/Fonts/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

	mOptick = new OptickAdapter();

	// Load the saved layout when opening the engine
	((SettingsPanel*)mPanels[SETTINGSPANEL])->LoadUserSettings();
	((SettingsPanel*)mPanels[SETTINGSPANEL])->LoadProjectSettings();
	((SettingsPanel*)mPanels[SETTINGSPANEL])->LoadEditorLayout();
	mPanels[SETTINGSPANEL]->Close();

	Style();
	return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	if (ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode))
	{
		for (auto it = mPanels.cbegin(); it != mPanels.cend(); ++it)
		{
			if (it->second->IsOpen())
			{
				it->second->Draw();
			}
		}
	}

	static bool show = true;
	//ImGui::ShowDemoWindow(&show);

	ShowMainMenuBar();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (mIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	for (auto panel : mPanels) 
	{
		delete panel.second;
	}
	mPanels.clear();
	delete mOptick;

	return true;
}

void ModuleEditor::OpenPanel(const char* name, const bool focus)
{
	if (focus)
	{
		ImGui::SetNextWindowFocus();
	}
	
	Panel* panel = mPanels[name];
	panel->Open();
}

void ModuleEditor::SaveSettings()
{
	((SettingsPanel*)mPanels[SETTINGSPANEL])->SaveUserSettings();
}

void ModuleEditor::ShowMainMenuBar() 
{
	IGFD::FileDialogConfig config;
	config.path = "Assets/Scenes";
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				EngineApp->GetScene()->NewScene();
			}
			if (ImGui::MenuItem("Load Scene"))
			{
				ImGuiFileDialog::Instance()->OpenDialog("LoadScene", "Choose File", ".scn", config);
			}
			if (ImGui::MenuItem("Save"))
			{
				if (!EngineApp->IsPlayMode())
				{
					std::string str = ASSETS_SCENES_PATH;
					str += App->GetScene()->GetName();
					str += ".scn";
					if (App->GetFileSystem()->Exists(str.c_str()))
					{
						App->GetScene()->Save(str.c_str());
					}
					else
					{
						ImGuiFileDialog::Instance()->OpenDialog("SaveScene", "Choose File", ".scn", config);
					}
				}
				else
				{
					LOG("YOU MUST EXIT PLAY MODE BEFORE SAVING");
				}
			}
			if (ImGui::MenuItem("Save As"))
			{
				if (!EngineApp->IsPlayMode())
				{
					ImGuiFileDialog::Instance()->OpenDialog("SaveScene", "Choose File", ".scn", config);
				}
				else
				{
					LOG("YOU MUST EXIT PLAY MODE BEFORE SAVING");
				}
			}
			if (ImGui::MenuItem("Quit"))
			{
				App->Exit();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) 
		{
			Panel* settingsPanel = mPanels[SETTINGSPANEL];
			if (ImGui::MenuItem("Settings", NULL, settingsPanel->IsOpen())) 
			{
				if (settingsPanel)
				{
					settingsPanel->IsOpen() ? settingsPanel->Close() : settingsPanel->Open();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Assets")) 
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject")) 
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Component")) 
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			Panel* timerPanel = mPanels[TIMERPANEL];
			if (ImGui::MenuItem("Timer", NULL, timerPanel->IsOpen())) 
			{
				if (timerPanel)
				{
					timerPanel->IsOpen() ? timerPanel->Close() : timerPanel->Open();
				}
			}
			Panel* navMeshPanel = mPanels[NAVMESHPANEL];
			if (ImGui::MenuItem("NavMeshController", NULL, navMeshPanel->IsOpen())) 
			{
				if (navMeshPanel)
				{
					navMeshPanel->IsOpen() ? navMeshPanel->Close() : navMeshPanel->Open();
				}
			}
			Panel* debugPanel = mPanels[DEBUGPANEL];
			if (ImGui::MenuItem("Debug", NULL, debugPanel->IsOpen())) 
			{
				if (debugPanel)
				{
					debugPanel->IsOpen() ? debugPanel->Close() : debugPanel->Open();
				}
			}
			if (ImGui::MenuItem("Optick", NULL, false, !mOptick->IsOpen())) 
			{
				mOptick->Startup();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::BeginMenu("Panels")) 
			{
				if (ImGui::MenuItem("Close all floating panels")) 
				{
					ResetFloatingPanels(false);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Reset all floating panels")) 
				{
					ResetFloatingPanels(true);
				}
				ImGui::Separator();
				Panel* project = mPanels[PROJECTPANEL];
				if (ImGui::MenuItem("1 Project", NULL, project->IsOpen()))
				{
					if (project)
					{
						project->IsOpen() ? project->Close() : project->Open();
					}
				}
				Panel* console = mPanels[CONSOLEPANEL];
				if (ImGui::MenuItem("2 Console", NULL, console->IsOpen())) 
				{
					if (console)
					{
						console->IsOpen() ? console->Close() : console->Open();
					}
				}
				Panel* hierarchy = mPanels[HIERARCHYPANEL];
				if (ImGui::MenuItem("3 Hierarchy", NULL, hierarchy->IsOpen())) 
				{
					if (hierarchy)
					{
						hierarchy->IsOpen() ? hierarchy->Close() : hierarchy->Open();
					}
				}
				Panel* scene = mPanels[SCENEPANEL];
				if (ImGui::MenuItem("5 Scene", NULL, scene->IsOpen())) 
				{
					if (scene)
					{
						scene->IsOpen() ? scene->Close() : scene->Open();
					}
				}
				Panel* inspector = mPanels[INSPECTORPANEL];
				if (ImGui::MenuItem("6 Inspector", NULL, inspector->IsOpen())) 
				{
					if (inspector)
					{
						inspector->IsOpen() ? inspector->Close() : inspector->Open();
					}
				}
				Panel* editorControlPanel = mPanels[EDITORCONTROLPANEL];
				if (ImGui::MenuItem("7 Editor Control Panel", NULL, editorControlPanel->IsOpen())) 
				{
					if (editorControlPanel)
					{
						editorControlPanel->IsOpen() ? editorControlPanel->Close() : editorControlPanel->Open();
					}
				}
				Panel* lightning = mPanels[LIGHTNINGPANEL];
				if (ImGui::MenuItem("8 Lightning", NULL, lightning->IsOpen())) 
				{
					if (lightning)
					{
						lightning->IsOpen() ? lightning->Close() : lightning->Open();
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			Panel* aboutPanel = mPanels[ABOUTPANEL];
			if (ImGui::MenuItem("About", NULL, aboutPanel->IsOpen()))
			{
				if (aboutPanel)
				{
					aboutPanel->IsOpen() ? aboutPanel->Close() : aboutPanel->Open();
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
		
	OpenSaveScene();
	OpenLoadScene();
		
	

	ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();
	ImVec2 buttonSize(40, height);


	if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags)) {
		if (ImGui::BeginMenuBar()) 
		{
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

}

void ModuleEditor::OpenLoadScene() {
	ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once);
	if (ImGuiFileDialog::Instance()->Display("LoadScene")) 
	{
		if (ImGuiFileDialog::Instance()->IsOk()) 
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			if (EngineApp->IsPlayMode())
			{
				reinterpret_cast<EditorControlPanel*>(mPanels[EDITORCONTROLPANEL])->Stop();
			}
			EngineApp->GetScene()->Load(std::string(ASSETS_SCENES_PATH + filePathName).c_str());

		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void ModuleEditor::OpenSaveScene() 
{
	ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once);
	if (ImGuiFileDialog::Instance()->Display("SaveScene")) 
	{
		if (ImGuiFileDialog::Instance()->IsOk()) 
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			filePathName = filePathName.substr(0, filePathName.find(".scn"));
			std::string str = ASSETS_SCENES_PATH;
			str += filePathName;
			str += ".scn";
			EngineApp->GetScene()->GetRoot()->SetName(filePathName.c_str());
			if (!App->GetFileSystem()->Exists(str.c_str()))
			{
				//EngineApp->GetEngineResource()->ImportFile(str.c_str(), 0u, true); TODO: FIX
				App->GetScene()->Save(str.c_str());
			}
			else
			{
				//EngineApp->GetEngineResource()->ImportFile(str.c_str(), 0u, false); TODO: FIX
				App->GetScene()->Save(str.c_str());
			}

		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void ModuleEditor::Style()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Border
	colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

	// Text
	colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

	// Popups
	colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

	// Slider
	colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
	colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

	// Seperator
	colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
	colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
	colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

	// Resize Grip
	colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
	colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

	// Docking
	colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

	auto& style = ImGui::GetStyle();
	style.TabRounding = 4;
	style.ScrollbarRounding = 9;
	style.WindowRounding = 7;
	style.GrabRounding = 3;
	style.FrameRounding = 3;
	style.PopupRounding = 4;
	style.ChildRounding = 4;
}

void ModuleEditor::ResetFloatingPanels(bool openPanels) 
{
	Panel* timerPanel = mPanels[TIMERPANEL];
	Panel* debugPanel = mPanels[DEBUGPANEL];
	Panel* navMeshController = mPanels[NAVMESHPANEL];
	Panel* projectPanel = mPanels[PROJECTPANEL];
	Panel* console = mPanels[CONSOLEPANEL];
	Panel* hierarchy = mPanels[HIERARCHYPANEL];
	Panel* scenePanel = mPanels[SCENEPANEL];
	Panel* inspector = mPanels[INSPECTORPANEL];
	Panel* editorControlPanel = mPanels[EDITORCONTROLPANEL];
	Panel* lightningPanel = mPanels[LIGHTNINGPANEL];
	Panel* resourcePanel = mPanels[RESOURCEPANEL];
	Panel* settingsPanel = mPanels[SETTINGSPANEL];
	
	Panel* aboutPanel = mPanels[ABOUTPANEL];

	if (openPanels == true) 
	{
		timerPanel->Open();
		debugPanel->Open();
		navMeshController->Open();
		projectPanel->Open();
		console->Open();
		hierarchy->Open();
		scenePanel->Open();
		inspector->Open();
		editorControlPanel->Open();
		lightningPanel->Open();
		resourcePanel->Open();
		settingsPanel->Open();
	}
	else 
	{
		timerPanel->Close();
		navMeshController->Close();
		debugPanel->Close();
		projectPanel->Close();
		console->Close();
		hierarchy->Close();
		scenePanel->Close();
		inspector->Close();
		editorControlPanel->Close();
		lightningPanel->Close();
		resourcePanel->Close();
		settingsPanel->Close();
		aboutPanel->Close();
	}
}

bool ModuleEditor::WantToCaptureKeyboard()
{
	return mIO->WantCaptureKeyboard;
}
