#include "pch.h" 
#include <utility>
#include <limits.h>
#include "TestScript.h"
#include <string>
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

TestScript::TestScript(GameObject* owner) : Script(owner)
{

}

void TestScript::Start()
{
	LOG("El nombre de mi gameobject es: %s ", mGameObject->GetName().c_str());
}

void TestScript::Update()
{
	if (App->GetInput()->GetKey(Keys::Keys_T) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, -1) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(-1, 0, 0) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(1, 0, 0) * App->GetGameDt());
	}

}

