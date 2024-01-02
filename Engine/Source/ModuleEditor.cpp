#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"

#include "Panel.h"
#include "AboutPanel.h"
#include "ConsolePanel.h"
#include "InspectorPanel.h"
#include "HierarchyPanel.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"

ModuleEditor::ModuleEditor()
{	
	mPanels[ABOUTPANEL] = new AboutPanel();
	mPanels[CONSOLEPANEL] = new ConsolePanel();
	mPanels[INSPECTORPANEL] = new InspectorPanel();
	mPanels[HIERARCHYPANEL] = new HierarchyPanel();
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

	ImGui_ImplSDL2_InitForOpenGL(App->GetWindow()->window, App->GetOpenGL()->context);
	ImGui_ImplOpenGL3_Init("#version 460");

	return true;
}

update_status ModuleEditor::PreUpdate()
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

	//Create Menu bar for the editor
	ShowMainMenuBar();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
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

update_status ModuleEditor::PostUpdate()
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

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::BeginMenu("Panels")) {
				if (ImGui::MenuItem("Close all floating panels...")) {
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Reset all floating panels")) {
				}
				ImGui::Separator();
				if (ImGui::MenuItem("1 Console")) {
				}
				//if (ImGui::MenuItem("2 Game")) {}
				
				if (ImGui::MenuItem("2 Hierarchy")) {
				}
				if (ImGui::MenuItem("3 Inspector")) {
				}
				//if (ImGui::MenuItem("5 Project")) {}
				//if (ImGui::MenuItem("6 Scene")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About")) {
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}