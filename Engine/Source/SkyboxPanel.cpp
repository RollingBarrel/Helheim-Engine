#include "SkyboxPanel.h"
#include <Windows.h>
#include <imgui.h>


SkyboxPanel::SkyboxPanel() : Panel(SKYBOXPANEL, true) {}

void SkyboxPanel::Draw(int windowFlags)
{
	//OPENFILENAME ofn;
	//GetOpenFileName(&ofn);
	static char* name[100];
	//Faces are ordered as follows: positive-x, negative-x, positive-y, negative-y, positive-z, negative-z.
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	static bool open = true;
	ImGui::Text("Right");
	if (ImGui::Button("Select File",ImVec2(10,10))) {

	}
	ImGui::Text("Left");
	if (ImGui::Button("Select File", ImVec2(100, 20))) {

	}
	ImGui::Text("Top");
	if (ImGui::Button("Select File", ImVec2(100, 20))) {

	}
	ImGui::Text("Down");
	if (ImGui::Button("Select File", ImVec2(100, 20))) {

	}
	ImGui::Text("Front");
	if (ImGui::Button("Select File", ImVec2(100, 20))) {

	}
	ImGui::Text("Back");
	if (ImGui::Button("Select File", ImVec2(100, 20))) {

	}

	ImGui::End();

}
