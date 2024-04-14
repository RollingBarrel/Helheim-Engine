#include "EditorControlPanel.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Timer.h"
#include "PreciseTimer.h"
#include "ModuleScriptManager.h"

#include "IconsFontAwesome6.h"


EditorControlPanel::EditorControlPanel() : Panel(EDITORCONTROLPANEL, true)
{
}

EditorControlPanel::~EditorControlPanel()
{
}

void EditorControlPanel::Draw(int windowFlags)
{
	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 80), ImGuiCond_Once);
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	ImVec2 buttonSize(40, 20);

	if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_ROTATE, buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER, buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::SCALE;
	}
	ImGui::SameLine();
	const char* buttonLabel = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ICON_FA_CUBE : ICON_FA_GLOBE;
	if (ImGui::Button(buttonLabel, buttonSize)) {
		mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
	}

	//Change the Guizmo operation using W,E & R keywords and the coordinate mode with G
	if (!ImGui::IsKeyDown(ImGuiKey_MouseRight)) 
	{
		if (ImGui::IsKeyPressed(ImGuiKey_W))
		{
			mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_E))
		{
			mCurrentGuizmoOperation = ImGuizmo::ROTATE;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_R))
		{
			mCurrentGuizmoOperation = ImGuizmo::SCALE;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_G))
		{
			mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
		}
	}


	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_PLAY, buttonSize) && mState != GameState::PLAYING) {
		Play();
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PAUSE, buttonSize) && mState == GameState::PLAYING) {
		Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FORWARD_STEP, buttonSize) && mState == GameState::PAUSED) {
		Play();
		mState = GameState::RUN_ONCE;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_STOP, buttonSize) && (mState == GameState::PLAYING || mState == GameState::PAUSED)) {
		Stop();
	}

	ImGui::End();
}




void EditorControlPanel::Play() {
	App->GetEngineClock()->Pause();				//Pauses Engine Clock (variables don't reset)
	App->GetGameClock()->Start();				//Starts GameClock

	App->SetCurrentClock(App->GetGameClock());	//Changes the current clock from Engine to Game

	mState = GameState::PLAYING;
	App->GetScene()->Save("TemporalScene");

	App->GetScriptManager()->Play();
}

void EditorControlPanel::Pause() {
	App->GetGameClock()->SetTimeScale(0.0f);
	mState = GameState::PAUSED;
}

void EditorControlPanel::Stop() {

	//Adds the frames from the game execution to the engine timer (The real time adds automatically)
	App->GetEngineClock()->SetTotalFrames(App->GetGameClock()->GetTotalFrames());

	App->GetGameClock()->Stop();			//Resets the Game clock time and variables

	App->SetCurrentClock(App->GetEngineClock());	//Changes the current clock from Game to Engine so it updates
	App->GetEngineClock()->Resume();				//Engine clock execution resumes -> mSpeed = 1.0f

	mState = GameState::STOP;

	App->GetScriptManager()->Stop();

	App->GetScene()->Load("TemporalScene");


}