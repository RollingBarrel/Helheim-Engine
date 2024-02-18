#include "Application.h"
#include "SDL.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "Quadtree.h"

#include "Panel.h"
#include "AboutPanel.h"
#include "ConsolePanel.h"
#include "InspectorPanel.h"
#include "HierarchyPanel.h"
#include "ScenePanel.h"
#include "QuadtreePanel.h"
#include "DebugPanel.h"
#include "PausePanel.h"
#include "ProjectPanel.h"
#include "LightningPanel.h"
#include "TimerPanel.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"
#include "ImGuizmo.h"


ModuleEditor::ModuleEditor()
{
	mPanels[ABOUTPANEL] = new AboutPanel();
	mPanels[CONSOLEPANEL] = new ConsolePanel();
	mPanels[INSPECTORPANEL] = new InspectorPanel();
	mPanels[HIERARCHYPANEL] = new HierarchyPanel();
	mPanels[SCENEPANEL] = new ScenePanel();
	mPanels[QUADTREEPANEL] = new QuadtreePanel();
	mPanels[PAUSEPANEL] = new PausePanel();
	mPanels[PROJECTPANEL] = new ProjectPanel();
	mPanels[DEBUGPANEL] = new DebugPanel();
	mPanels[LIGHTNINGPANEL] = new LightningPanel();
	mPanels[TIMERPANEL] = new TimerPanel();
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();

	io = &(ImGui::GetIO());
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io->ConfigDragClickToInputText = true;
	ImGui_ImplSDL2_InitForOpenGL(App->GetWindow()->window, App->GetOpenGL()->GetOpenGlContext());
	ImGui_ImplOpenGL3_Init("#version 460");

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

	//static bool show = true;
	//ImGui::ShowDemoWindow(&show);

	ShowMainMenuBar();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
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
	for (auto panel : mPanels) {
		delete panel.second;
	}
	mPanels.clear();

	return true;
}

void ModuleEditor::ShowMainMenuBar() {
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load Scene"))
			{
				mLoadSceneOpen = true;
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				App->GetScene()->Save("scene.json");
			}
			if (ImGui::MenuItem("Quit"))
			{
				exit(0);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Assets")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Component")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Quadtree")) {
				Panel* quadtreeDebug = mPanels[QUADTREEPANEL];
				if (quadtreeDebug)
				{
					quadtreeDebug->IsOpen() ? quadtreeDebug->Close() : quadtreeDebug->Open();
				}
			}

			if (ImGui::MenuItem("Debug")) {
				Panel* debugPanel = mPanels[DEBUGPANEL];
				if (debugPanel)
				{
					debugPanel->IsOpen() ? debugPanel->Close() : debugPanel->Open();
				}
			}
			if (ImGui::MenuItem("Timer")) {
				Panel* timerPanel = mPanels[TIMERPANEL];
				if (timerPanel)
				{
					timerPanel->IsOpen() ? timerPanel->Close() : timerPanel->Open();
				}
			}

			if (ImGui::Checkbox("Draw Mouse Picking RayCast", &mDrawRaycast)) {
				App->GetCamera()->DrawRayCast(mDrawRaycast);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::BeginMenu("Panels")) {
				if (ImGui::MenuItem("Close all floating panels")) {
					ResetFloatingPanels(false);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Reset all floating panels")) {
					ResetFloatingPanels(true);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("1 Console")) {
					Panel* console = mPanels[CONSOLEPANEL];
					if (console)
					{
						console->IsOpen() ? console->Close() : console->Open();
					}
				}
				if (ImGui::MenuItem("2 Hierarchy")) {
					Panel* hierarchy = mPanels[HIERARCHYPANEL];
					if (hierarchy)
					{
						hierarchy->IsOpen() ? hierarchy->Close() : hierarchy->Open();
					}
				}
				if (ImGui::MenuItem("3 Inspector")) {
					Panel* inspector = mPanels[INSPECTORPANEL];
					if (inspector)
					{
						inspector->IsOpen() ? inspector->Close() : inspector->Open();
					}
				}
				if (ImGui::MenuItem("4 Pause")) {
					Panel* pause = mPanels[PAUSEPANEL];
					if (pause)
					{
						pause->IsOpen() ? pause->Close() : pause->Open();
					}
				}
				if (ImGui::MenuItem("5 Lightning")) {
					Panel* lightning = mPanels[LIGHTNINGPANEL];
					if (lightning)
					{
						lightning->IsOpen() ? lightning->Close() : lightning->Open();
					}
				}
				//if (ImGui::MenuItem("5 Scene")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				Panel* aboutPanel = mPanels[ABOUTPANEL];
				if (aboutPanel)
				{
					aboutPanel->IsOpen() ? aboutPanel->Close() : aboutPanel->Open();
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (mLoadSceneOpen) {
		OpenLoadScene();
	}
}

void ModuleEditor::OpenLoadScene() {
	ImGui::OpenPopup("LoadSceneWindow");
	if (ImGui::BeginPopupModal("LoadSceneWindow", &mLoadSceneOpen))
	{
		ImGui::Text("Which file you wish to load?");
		static char fileName[128] = "";
		ImGui::InputText(".json", fileName, IM_ARRAYSIZE(fileName));
		if (ImGui::Button("Load")) {
			if (!strcmp(fileName, "scene")) {
				App->GetScene()->Load(fileName);
				ImGui::CloseCurrentPopup();
				mLoadSceneOpen = false;
			}
		}
		ImGui::EndPopup();
	}
}

void ModuleEditor::ResetFloatingPanels(bool openPanels) {
	Panel* console = mPanels[CONSOLEPANEL];
	Panel* hierarchy = mPanels[HIERARCHYPANEL];
	Panel* inspector = mPanels[INSPECTORPANEL];
	Panel* pause = mPanels[PAUSEPANEL];
	Panel* aboutPanel = mPanels[ABOUTPANEL];
	Panel* scenePanel = mPanels[SCENEPANEL];
	Panel* quadTree = mPanels[QUADTREEPANEL];
	Panel* projectPanel = mPanels[PROJECTPANEL];
	Panel* lightningPanel = mPanels[LIGHTNINGPANEL];
	Panel* timerPanel = mPanels[TIMERPANEL];

	if (openPanels == true) {
		console->Open();
		hierarchy->Open();
		inspector->Open();
		pause->Open();
		aboutPanel->Open();
		scenePanel->Open();
		quadTree->Open();
		projectPanel->Open();
		lightningPanel->Open();
		timerPanel->Open();
	}
	else {
		console->Close();
		hierarchy->Close();
		inspector->Close();
		pause->Close();
		aboutPanel->Close();
		scenePanel->Close();
		quadTree->Close();
		projectPanel->Close();
		lightningPanel->Close();
		timerPanel->Close();
	}
}