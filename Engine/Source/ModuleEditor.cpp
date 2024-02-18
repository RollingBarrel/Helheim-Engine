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
	Panel* quadtreeDebug = mPanels[QUADTREEPANEL];
	Panel* debugPanel = mPanels[DEBUGPANEL];
	Panel* timerPanel = mPanels[TIMERPANEL];

	Panel* project = mPanels[PROJECTPANEL];
	Panel* console = mPanels[CONSOLEPANEL];
	Panel* hierarchy = mPanels[HIERARCHYPANEL];
	Panel* pause = mPanels[PAUSEPANEL];
	Panel* scene = mPanels[SCENEPANEL];
	Panel* inspector = mPanels[INSPECTORPANEL];
	Panel* lightning = mPanels[LIGHTNINGPANEL];

	Panel* aboutPanel = mPanels[ABOUTPANEL];

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
			if (ImGui::MenuItem("Timer", NULL, &mTimerPanel)) {
				if (timerPanel)
				{
					timerPanel->IsOpen() ? timerPanel->Close() : timerPanel->Open();
				}
			}
			if (ImGui::MenuItem("Quadtree", NULL, &mQuadtreePanel)) {
				if (quadtreeDebug)
				{
					quadtreeDebug->IsOpen() ? quadtreeDebug->Close() : quadtreeDebug->Open();
				}
			}

			if (ImGui::MenuItem("Debug", NULL, &mDebugPanel)) {
				if (debugPanel)
				{
					debugPanel->IsOpen() ? debugPanel->Close() : debugPanel->Open();
				}
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
				if (ImGui::MenuItem("1 Project", NULL, &mProjectPanel)) 
				{
					if (project)
					{
						project->IsOpen() ? project->Close() : project->Open();
					}
				}
				if (ImGui::MenuItem("2 Console", NULL, &mConsolePanel)) {
					if (console)
					{
						console->IsOpen() ? console->Close() : console->Open();
					}
				}
				if (ImGui::MenuItem("3 Hierarchy", NULL, &mHierarchyPanel)) {
					if (hierarchy)
					{
						hierarchy->IsOpen() ? hierarchy->Close() : hierarchy->Open();
					}
				}
				if (ImGui::MenuItem("4 Pause", NULL, &mPausePanel)) {
					if (pause)
					{
						pause->IsOpen() ? pause->Close() : pause->Open();
					}
				}
				if (ImGui::MenuItem("5 Scene", NULL, &mScenePanel)) {
					if (scene)
					{
						scene->IsOpen() ? scene->Close() : scene->Open();
					}
				}
				if (ImGui::MenuItem("6 Inspector", NULL, &mInspectorPanel)) {
					if (inspector)
					{
						inspector->IsOpen() ? inspector->Close() : inspector->Open();
					}
				}
				if (ImGui::MenuItem("7 Lightning", NULL, &mLightingPanel)) {
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
			if (ImGui::MenuItem("About", NULL, &mAboutPanel))
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

	if (mLoadSceneOpen) {
		OpenLoadScene();
	}

	if (quadtreeDebug->IsOpen() == false) {
		mQuadtreePanel = false;
	}
	else {
		mQuadtreePanel = true;
	}

	if (debugPanel->IsOpen() == false) {
		mDebugPanel = false;
	}
	else {
		mDebugPanel = true;
	}

	if (timerPanel->IsOpen() == false) {
		mTimerPanel = false;
	}
	else {
		mTimerPanel = true;
	}

	if (project->IsOpen() == false) {
		mProjectPanel = false;
	}
	else { 
		mProjectPanel = true; 
	}

	if (console->IsOpen() == false) {
		mConsolePanel = false;
	}
	else {
		mConsolePanel = true; 
	}

	if (hierarchy->IsOpen() == false) {
		mHierarchyPanel = false;
	}
	else{ 
		mHierarchyPanel = true; 
	}

	if (pause->IsOpen() == false) {
		mPausePanel = false;
	}
	else{ 
		mPausePanel = true; 
	}

	if (scene->IsOpen() == false) {
		mScenePanel = false;
	}
	else{ 
		mScenePanel = true; 
	}

	if (inspector->IsOpen() == false) {
		mInspectorPanel = false;
	}
	else {
		mInspectorPanel = true;
	}

	if (lightning->IsOpen() == false) {
		mLightingPanel = false;
	}
	else {
		mLightingPanel = true;
	}

	if (aboutPanel->IsOpen() == false) {
		mAboutPanel = false;
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
	Panel* timerPanel = mPanels[TIMERPANEL];
	Panel* quadTree = mPanels[QUADTREEPANEL];
	Panel* debugPanel = mPanels[DEBUGPANEL];

	Panel* projectPanel = mPanels[PROJECTPANEL];
	Panel* console = mPanels[CONSOLEPANEL];
	Panel* hierarchy = mPanels[HIERARCHYPANEL];
	Panel* pause = mPanels[PAUSEPANEL];
	Panel* scenePanel = mPanels[SCENEPANEL];
	Panel* inspector = mPanels[INSPECTORPANEL];
	Panel* lightningPanel = mPanels[LIGHTNINGPANEL];
	
	Panel* aboutPanel = mPanels[ABOUTPANEL];

	if (openPanels == true) {
		timerPanel->Open();
		quadTree->Open();
		debugPanel->Open();
		
		projectPanel->Open();
		console->Open();
		hierarchy->Open();
		pause->Open();
		scenePanel->Open();
		inspector->Open();
		lightningPanel->Open();
		
		mTimerPanel = true;
		mQuadtreePanel = true;
		mDebugPanel = true;

		mProjectPanel = true;
		mConsolePanel = true;
		mHierarchyPanel = true;
		mPausePanel = true;
		mScenePanel = true;
		mInspectorPanel = true;
		mLightingPanel = true;
	}
	else {
		timerPanel->Close();
		quadTree->Close();
		debugPanel->Close();

		projectPanel->Close();
		console->Close();
		hierarchy->Close();
		pause->Close();
		scenePanel->Close();
		inspector->Close();
		lightningPanel->Close();

		aboutPanel->Close();
		
		mTimerPanel = false;
		mQuadtreePanel = false;
		mDebugPanel = false;

		mProjectPanel = false;
		mConsolePanel = false;
		mHierarchyPanel = false;
		mPausePanel = false;
		mScenePanel = false;
		mInspectorPanel = false;
		mLightingPanel = false;

		mAboutPanel = false;
	}
}