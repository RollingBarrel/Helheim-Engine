#include "RotateObject.h"
#include "Application.h"
#include "GameObject.h"
#include "GameManager.h"

#include "ScriptComponent.h"

CREATE(RotateObject)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT3, mDirection);
	END_CREATE;
}

RotateObject::RotateObject(GameObject* owner) : Script(owner) {}

void RotateObject::Start()
{

}

void RotateObject::Update()
{
	if (GameManager::GetInstance()->IsPaused()) return;
	mDirection.Normalize();

	float3 euler = mGameObject->GetLocalEulerAngles();
	euler += mDirection * App->GetDt() * mSpeed;

	mGameObject->SetLocalRotation(euler);
}
