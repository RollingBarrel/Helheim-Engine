#include "EditorControlPanel.h"

#include "EngineApp.h"
#include "ModuleCamera.h"
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Timer.h"
#include "PreciseTimer.h"
#include "ModuleEngineScriptManager.h"
#include "NavMeshController.h"
#include "IconsFontAwesome6.h"
#include "ModuleAudio.h"


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

	ImVec2 buttonSize(30, 25);
	ImVec4 pressedColor = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

	if (mCurrentGuizmoOperation == ImGuizmo::TRANSLATE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressedColor);
	}

	if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, buttonSize)) 
	{
		mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	}
	else if (mCurrentGuizmoOperation == ImGuizmo::TRANSLATE) 
	{
		ImGui::PopStyleColor(3);
	}
	
	ImGui::SameLine();
	if (mCurrentGuizmoOperation == ImGuizmo::ROTATE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressedColor);
	}
	if (ImGui::Button(ICON_FA_ROTATE, buttonSize)) 
	{
		mCurrentGuizmoOperation = ImGuizmo::ROTATE;
		bool change = true;
	}
	else if (mCurrentGuizmoOperation == ImGuizmo::ROTATE) 
	{
		ImGui::PopStyleColor(3);
	}

	ImGui::SameLine();
	if (mCurrentGuizmoOperation == ImGuizmo::SCALE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressedColor);
	}
	if (ImGui::Button(ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER, buttonSize)) 
	{
		mCurrentGuizmoOperation = ImGuizmo::SCALE;
	}
	else if (mCurrentGuizmoOperation == ImGuizmo::SCALE)
	{
		ImGui::PopStyleColor(3);
	}

	ImGui::SameLine();
	const char* buttonLabel = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ICON_FA_CUBE : ICON_FA_GLOBE;
	if (ImGui::Button(buttonLabel, buttonSize)) 
	{
		mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
	}


	//Change the Guizmo operation using W,E & R keywords and the coordinate mode with G
	if (!ImGui::IsKeyDown(ImGuiKey_MouseRight) && (mState != GameState::PLAY && mState != GameState::PLAY_PAUSE)) 
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
	
	float menuBarWidth = ImGui::GetWindowWidth();
	float spacing = (menuBarWidth - (buttonSize.x * 14)) * 0.5f;
	
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(spacing, 0.0f));

	ImGui::SameLine();
	GameState currentState = mState;
	if (mState == GameState::PLAY || mState == GameState::PLAY_PAUSE) 
	{
		ImGui::PushStyleColor(ImGuiCol_Button, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressedColor);
	}
	if (ImGui::Button(ICON_FA_PLAY, buttonSize)) 
	{	
		(mState == GameState::PLAY || mState == GameState::PLAY_PAUSE) ? Stop() : Play();
	}
	if (currentState == GameState::PLAY || currentState == GameState::PLAY_PAUSE)
	{
		ImGui::PopStyleColor(3);
	}

	currentState = mState;
	ImGui::SameLine();
	if (mState == GameState::PAUSE || mState == GameState::PLAY_PAUSE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pressedColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressedColor);
	}
	if (ImGui::Button(ICON_FA_PAUSE, buttonSize)) 
	{
		Pause();
	}
	if (currentState == GameState::PAUSE || currentState == GameState::PLAY_PAUSE)
	{
		ImGui::PopStyleColor(3);
	}

	ImGui::SameLine();
	if ( mState != GameState::PLAY && mState != GameState::PLAY_PAUSE)
	{ 
		ImGui::BeginDisabled();
	}
	if (ImGui::Button(ICON_FA_FORWARD_STEP, buttonSize)) 
	{
		Step();
	}
	if (mState != GameState::PLAY && mState != GameState::PLAY_PAUSE)
	{
		ImGui::EndDisabled();
	}
	ImGui::End();
}


void EditorControlPanel::Play() 
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.05f, 0.05f, 0.07f, 1.0f };
	
	EngineApp->Start();
	switch (mState)
	{
	case GameState::PAUSE:
		mState = GameState::PLAY_PAUSE;
		EngineApp->GetEngineScriptManager()->Pause(true);
		EngineApp->GetAudio()->PauseAllChannels();
		break;
	default:
		mState = GameState::PLAY;
		break;
	}
	

	ImGui::SetWindowFocus("Game");
	
}

void EditorControlPanel::Pause() 
{
	switch (mState)
	{
	case GameState::PLAY:
		mState = GameState::PLAY_PAUSE;
		EngineApp->GetEngineScriptManager()->Pause(true);
		EngineApp->GetAudio()->PauseAllChannels();
		break;
	case GameState::PAUSE:
		mState = GameState::STOP;
		break;
	case GameState::PLAY_PAUSE:
		mState = GameState::PLAY;
		EngineApp->GetEngineScriptManager()->Pause(false);
		break;
	default:
		mState = GameState::PAUSE;
		break;
	}
}

void EditorControlPanel::Stop() 
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };

	EngineApp->Stop();
	mState = GameState::STOP;

}

void EditorControlPanel::Step()
{
	switch (mState)
	{
	case GameState::PLAY:
		mState = GameState::PLAY_PAUSE;
		[[fallthrough]];
	case GameState::PLAY_PAUSE:
		EngineApp->GetEngineScriptManager()->Pause(false);
		EngineApp->GetEngineScriptManager()->Update(EngineApp->GetRealDt());
		EngineApp->GetEngineScriptManager()->Pause(true);
		EngineApp->GetAudio()->PauseAllChannels();
		break;
	default:
		break;
	}
}
